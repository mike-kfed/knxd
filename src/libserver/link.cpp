/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2011 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "link.h"

#include <cstdio>

#include "router.h"

bool
LinkRecv::link(LinkBasePtr next)
{
  assert(next);
  if(!next->_link(std::dynamic_pointer_cast<LinkRecv>(shared_from_this())))
    return false;
  assert(send == next); // _link_ was called
  return true;
}

bool
Driver::assureFilter(std::string name, bool first)
{
  if (findFilter(name) != nullptr)
    return true;

  auto c = conn.lock();
  if (c == nullptr)
    return false;

  std::string sn = this->name() + '.' + name;
  IniSectionPtr s = static_cast<Router&>(c->router).ini.add_auto(sn);
  if (s == nullptr)
    return false;
  auto f = static_cast<Router&>(c->router).get_filter(c, s, name);
  if (f == nullptr)
    return false;
  if (!push_filter(f, first))
    return false;

  // push_filter doesn't call setup()
  if (!f->setup())
    return false;
  return true;
}

LinkConnect::~LinkConnect()
{
  if (addr && addr_local)
    static_cast<Router &>(router).release_client_addr(addr);
}

LinkBase::LinkBase(BaseRouter&, IniSectionPtr& s, TracePtr tr) : cfg(s)
{
  t = TracePtr(new Trace(*tr, s));
  t->setAuxName("Base");
}

std::string
LinkBase::info(int)
{
  // TODO add more introspection
  std::string res = "cfg:";
  res += cfg->name;
  return res;
}

LinkConnect_::LinkConnect_(BaseRouter& r, IniSectionPtr& c, TracePtr tr)
  : router(r), LinkRecv(r,c,tr)
{
  t->setAuxName("Conn_");
  //Router& rt = dynamic_cast<Router&>(r);
}

LinkConnect::LinkConnect(BaseRouter& r, IniSectionPtr& c, TracePtr tr)
  : LinkConnect_(r,c,tr)
{
  t->setAuxName("Conn");
  //Router& rt = dynamic_cast<Router&>(r);
}

const char *
LinkConnect::stateName()
{
  switch(state)
    {
    case L_down:
      return "down";
    case L_going_down:
      return ">down";
    case L_up:
      return "up";
    case L_going_up:
      return ">up";
    case L_error:
      return "error";
    case L_going_error:
      return ">error";
    default:
      abort();
      return "?!?";
    }
}

void
LinkConnect::setState(LConnState new_state)
{
  if (state == new_state)
    return;

  LConnState old_state = state;
  const char *osn = stateName();
  state = new_state;
  TRACEPRINTF(t, 5, "%s => %s", osn, stateName());

  switch(old_state)
    {
    case L_down:
      switch(new_state)
        {
        case L_going_up:
          start();
          break;
        case L_going_down: // redundant call to stop()
          state = L_down;
          break;
        default:
          goto inval;
        }
      break;
    case L_going_down:
      switch(new_state)
        {
        case L_error:
          state = L_going_error;
          break;
        case L_down:
          break;
        default:
          goto inval;
        }
      break;
    case L_up:
      switch(new_state)
        {
        case L_going_up: // redundant call to start()
          state = L_up;
          break;
        case L_going_down:
          stop(false);
          break;
        case L_down:
          break;
        case L_error:
          state = L_error;
          break;
        default:
          goto inval;
        }
      break;
    case L_going_up:
      switch(new_state)
        {
        case L_up:
          break;
        case L_going_down:
          stop(false);
          break;
        case L_down:
          break;
        case L_error:
          state = L_error;
          break;
        default:
          goto inval;
        }
      break;
    case L_error:
      switch(new_state)
        {
        case L_error:
          break;
        case L_going_down:
          state = L_error;
          break;
        case L_down:
          break;
        default:
          goto inval;
        }
      break;
    case L_going_error:
      switch(new_state)
        {
        case L_down:
        case L_error:
          break;
        case L_going_down:
          state = L_going_error;
          break;
        default:
          goto inval;
        }
      break;
    }
  static_cast<Router&>(router).linkStateChanged(std::dynamic_pointer_cast<LinkConnect>(shared_from_this()));
  return;

inval:
  ERRORPRINTF (t, E_ERROR | 60, "invalid transition: %s => %s", osn, stateName());
  abort();
  return;
}

void
LinkConnect::setAddress(eibaddr_t addr)
{
  this->addr = addr;
  this->addr_local = false;
}
bool
LinkConnectSingle::setup()
{
  if (!LinkConnectClient::setup())
    return false;
  if (addr == 0)
    addr = static_cast<Router &>(router).get_client_addr(t);
  if (addr == 0)
    return false;
  return true;
}

void
LinkConnect::start()
{
  TRACEPRINTF(t, 5, "Starting");
  send_more = true;
  LinkConnect_::start();
}

void
LinkConnect_::start()
{
  LinkRecv::start();
  send->start();
}

void
LinkConnect::stop(bool err)
{
  TRACEPRINTF(t, 5, "L Stopping");
  LinkConnect_::stop(err);
}

void
LinkConnect_::stop(bool err)
{
  send->stop(err);
  LinkRecv::stop(err);
}

const std::string&
Filter::name()
{
  return cfg->value("filter",cfg->name);
}

const std::string&
Driver::name()
{
  return cfg->value("driver",cfg->name);
}

FilterPtr
Filter::findFilter(std::string name, bool skip_me)
{
  auto r = recv.lock();
  if (r == nullptr)
    return nullptr;
  if (!skip_me && this->name() == name)
    return std::static_pointer_cast<Filter>(shared_from_this());
  return r->findFilter(name, false);
}

FilterPtr
Driver::findFilter(std::string name, bool skip_me)
{
  auto r = recv.lock();
  if (r == nullptr)
    return nullptr;
  return r->findFilter(name);
}

bool
LineDriver::setup()
{
  if(!Driver::setup())
    return false;

  auto c = std::dynamic_pointer_cast<LinkConnect>(conn.lock());
  if (c == nullptr)
    return false;

  _addr = c->addr;
  return true;
}

bool
LinkConnect::setup()
{
  if (!LinkConnect_::setup())
    return false;

  ignore = cfg->value("ignore",false);
  x_may_fail = cfg->value("may-fail",false);
  x_max_retries = cfg->value("max-retries",-1);
  x_retry_delay = cfg->value("retry-delay",1.);
  return true;
}

bool
LinkConnect_::setup()
{
  if (!LinkRecv::setup())
    return false;
  DriverPtr dr = driver; // .lock();
  if(dr == nullptr)
    {
      ERRORPRINTF (t, E_ERROR | 61, "No driver in %s. Refusing.", cfg->name);
      return false;
    }

  std::string x = cfg->value("filters","");
  {
    size_t pos = 0;
    size_t comma = 0;
    while(true)
      {
        comma = x.find(',',pos);
        std::string name = x.substr(pos,comma-pos);
        if (name.size())
          {
            FilterPtr link;
            IniSectionPtr s = static_cast<Router&>(router).ini[name];
            name = s->value("filter",name);
            link = static_cast<Router&>(router).get_filter(std::dynamic_pointer_cast<LinkConnect_>(shared_from_this()),
                   s, name);
            if (link == nullptr)
              {
                ERRORPRINTF (t, E_ERROR | 32, "filter '%s' not found.", name);
                return false;
              }
            if(!dr->push_filter(link))
              {
                ERRORPRINTF (t, E_ERROR | 63, "Linking filter '%s' failed.", name);
                return false;
              }
          }
        if (comma == std::string::npos)
          break;
        pos = comma+1;
      }
  }

  LinkBasePtr s = send;
  while (s != nullptr)
    {
      if (!s->setup())
        {
          ERRORPRINTF (t, E_ERROR | 64, "%s: setup %s: failed", cfg->name, s->cfg->name);
          return false;
        }
      if (s == dr)
        break;
      auto ps = std::dynamic_pointer_cast<Filter>(s);
      if (ps == nullptr)
        {
          ERRORPRINTF (t, E_FATAL | 102, "%s: setup %s: no driver", cfg->name, s->cfg->name);
          return false;
        }
      s = ps->send;
    }
  if (s == nullptr)
    {
      ERRORPRINTF (t, E_FATAL | 103, "%s: setup: no driver", cfg->name);
      return false;
    }
  return true;
}

void
LinkConnect::started()
{
  setState(L_up);
  TRACEPRINTF(t, 5, "Started");
}

void
LinkConnect::send_Next()
{
  send_more = true;
  TRACEPRINTF(t, 6, "sendNext called, send_more set");
  static_cast<Router&>(router).send_Next();
}

void
LinkConnect::send_L_Data (LDataPtr l)
{
  send_more = false;
  assert (state == L_up);
  TRACEPRINTF(t, 6, "sending, send_more clear");
  LinkConnect_::send_L_Data(std::move(l));
}

void
LinkConnect::stopped(bool err)
{
  setState(err ? L_error : L_down);
}

void
LinkConnect::recv_L_Data (LDataPtr l)
{
  static_cast<Router&>(router).recv_L_Data(std::move(l), *this);
}

bool
LinkConnect::checkSysAddress(eibaddr_t addr)
{
  return static_cast<Router&>(router).checkAddress(addr, std::dynamic_pointer_cast<LinkConnect>(shared_from_this()));
}

bool
LinkConnect::checkSysGroupAddress(eibaddr_t addr)
{
  return static_cast<Router&>(router).checkGroupAddress(addr, std::dynamic_pointer_cast<LinkConnect>(shared_from_this()));
}

bool
LinkConnect_::checkSysAddress(eibaddr_t addr)
{
  return static_cast<Router&>(router).checkAddress(addr, nullptr);
}

bool
LinkConnect_::checkSysGroupAddress(eibaddr_t addr)
{
  return static_cast<Router&>(router).checkGroupAddress(addr, nullptr);
}


void
LinkConnect::recv_L_Busmonitor (LBusmonPtr l)
{
  static_cast<Router&>(router).recv_L_Busmonitor(std::move(l));
}

bool
Server::setup()
{
  return true;
}

LinkConnectClient::LinkConnectClient(ServerPtr s, IniSectionPtr& c, TracePtr tr)
  : server(s), LinkConnect(s->router, c, tr)
{
  t->setAuxName("ConnC");
  char n[10];
  sprintf(n,"%d",t->seq);
  linkname = t->name + '_' + n;
}

SubDriver::SubDriver(const LinkConnectClientPtr& c)
  : BusDriver(static_cast<const LinkConnectPtr&>(c), c->cfg)
{
  t->setAuxName("SubDr");
  server = c->server;
}

LineDriver::LineDriver(const LinkConnectClientPtr& c)
  : Driver(c, c->cfg)
{
  t->setAuxName("LineDr");
  server = c->server;
}

void
Driver::send_Next()
{
  auto r = recv.lock();
  if (r != nullptr)
    r->send_Next();
}

void
Driver::recv_L_Data (LDataPtr l)
{
  auto r = recv.lock();
  if (r != nullptr)
    r->recv_L_Data(std::move(l));
}

bool
Driver::checkSysAddress(eibaddr_t addr)
{
  auto r = recv.lock();
  if (r == nullptr)
    return false;
  return r->checkSysAddress(addr);
}

bool
Driver::checkSysGroupAddress(eibaddr_t addr)
{
  auto r = recv.lock();
  if (r == nullptr)
    return false;
  return r->checkSysGroupAddress(addr);
}

void
Filter::send_Next()
{
  auto r = recv.lock();
  if (r != nullptr)
    r->send_Next();
}

void
Filter::recv_L_Data (LDataPtr l)
{
  auto r = recv.lock();
  if (r != nullptr)
    r->recv_L_Data(std::move(l));
}

bool
Filter::checkSysAddress(eibaddr_t addr)
{
  auto r = recv.lock();
  if (r == nullptr)
    return false;
  return r->checkSysAddress(addr);
}

bool
Filter::checkSysGroupAddress(eibaddr_t addr)
{
  auto r = recv.lock();
  if (r == nullptr)
    return false;
  return r->checkSysGroupAddress(addr);
}

void
Driver::recv_L_Busmonitor (LBusmonPtr l)
{
  auto r = recv.lock();
  if (r != nullptr)
    r->recv_L_Busmonitor(std::move(l));
}

void
Filter::recv_L_Busmonitor (LBusmonPtr l)
{
  auto r = recv.lock();
  if (r != nullptr)
    r->recv_L_Busmonitor(std::move(l));
}

void
Driver::started()
{
  auto r = recv.lock();
  if (r != nullptr)
    r->started();
}

void
Filter::started()
{
  auto r = recv.lock();
  if (r != nullptr)
    r->started();
}

void
Driver::stopped(bool err)
{
  auto r = recv.lock();
  if (r != nullptr)
    r->stopped(err);
}

void
Filter::stopped(bool err)
{
  auto r = recv.lock();
  if (r != nullptr)
    r->stopped(err);
}

bool
Driver::push_filter(FilterPtr filter, bool first)
{
  LinkRecvPtr r;
  LinkBasePtr t;

  // r->t ==> r->filter->t

  if (first)
    {
      // r is the LinkConnect base, t is the following LinkBase-ish thing
      LinkConnectPtr_ c = conn.lock();
      if (c == nullptr)
        return false;
      r = c;
      t = c->send;
    }
  else
    {
      // t is this driver, so r is this->recv
      r = recv.lock();
      if (r == nullptr)
        return false;
      t = shared_from_this();
    }

  // link the first part
  if (!r->link(filter))
    return false;
  // link the second part
  if (!filter->link(t))
    {
      // didn't work, so undo the first.
      r->link(t);
      return false;
    }

#if 0 // this is done by LinkConnect::setup() once the stack is complete
  if (!filter->setup())
    {
      filter->unlink();
      return false;
    }
#endif
  return true;
}

Filter::Filter(const LinkConnectPtr_& c, IniSectionPtr& s)
  : LinkRecv(c->router, s, c->t)
{
  conn = c;
  t->setAuxName(c->t->name);
}

