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

/**
 * @file
 * @ingroup KNX_03_03_04
 * Transport Layer
 * @{
 */

#ifndef GROUPCACHE_H
#define GROUPCACHE_H

#include <ctime>
#include <map>
#include <unordered_map>

#include "client.h"
#include "link.h"

class GroupCache;

struct GroupCacheEntry
{
  GroupCacheEntry(eibaddr_t dst)
  {
    this->dst = dst;
  }
  /** Layer 4 data */
  CArray data;
  /** source address */
  eibaddr_t src = 0;
  /** destination address */
  eibaddr_t dst;
  /** receive time */
  time_t recvtime;
  /** seqnum */
  uint32_t seq;
};

typedef void (*GCReadCallback)(const GroupCacheEntry &foo, bool nowait, ClientConnPtr c);
typedef void (*GCLastCallback)(const std::vector<eibaddr_t> &foo, uint32_t end, ClientConnPtr c);

class GroupCacheReader
{
public:
  GroupCacheReader(GroupCache *);
  virtual ~GroupCacheReader();

  bool stopped = false;
  GroupCache *gc;
  virtual void updated(GroupCacheEntry &) = 0;
  virtual void stop(bool err);
};

/** map last-updated sequence numbers to group addresses */
using SeqMap = std::map<uint32_t, eibaddr_t>;

/** map group addresses to cache entries */
using CacheMap = std::unordered_map<eibaddr_t, GroupCacheEntry>;

class GroupCache:public Driver
{
public: // but only for GroupCacheReader
  bool setup();
  void start();
  void stop(bool err);
  virtual bool checkGroupAddress (eibaddr_t) const override
  {
    return true;
  }

  bool checkAddress (eibaddr_t) const
  {
    return false;
  }

  bool hasAddress (eibaddr_t addr) const
  {
    return addr == this->addr;
  }

  void addAddress (eibaddr_t) { }

  /** constructor */
  GroupCache (const LinkConnectPtr& c, IniSectionPtr& s);
  /** destructor */
  virtual ~GroupCache ();
  /** Feed data to the cache */
  void send_L_Data (LDataPtr l);

  /** add a reader which gets triggered on updates */
  void add (GroupCacheReader *r);
  void remove (GroupCacheReader *r);

  /** remove an address from the cache */
  void remove (eibaddr_t ga);

  /** seqnum of last entry */
  uint32_t seq = 0;
  /** map seqnum to group address */
  SeqMap cache_seq;

  /** Turn on caching, calls l3.registerGroupCallBack(ANY) */
  bool Start ();
  /** drop the whole cache */
  void Clear ();
  /** Turn off caching, deregisters */
  void Stop ();

  /** read, and optionally wait for, a cache entry for this address */
  void Read (eibaddr_t addr, unsigned timeout, uint16_t age,
             GCReadCallback cb, ClientConnPtr c);
  /** incrementally monitor group cache updates */
  void LastUpdates (uint16_t start, uint8_t timeout,
                    GCLastCallback cb, ClientConnPtr c);
  void LastUpdates2 (uint32_t start, uint8_t timeout,
                     GCLastCallback cb, ClientConnPtr c);

private:
  std::vector < GroupCacheReader * > reader;
  /** The Cache */
  CacheMap cache;
  /** controlled by .Start/Stop; if false, the whole code does nothing */
  bool enable = false;
  /** max size of cache */
  uint16_t maxsize;
  /** cached copy of main address */
  eibaddr_t addr;

  ev::async remtrigger;
  void remtrigger_cb(ev::async &w, int revents);
  /** signal that this entry has been updated */
  virtual void updated(GroupCacheEntry &);
};

using GroupCachePtr = std::shared_ptr<GroupCache>;

#endif

/** @} */
