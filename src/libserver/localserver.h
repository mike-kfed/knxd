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
 * @addtogroup Server
 * @{
 */

#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include "server.h"

/** implements a server listening on a unix domain socket */
SERVER_(LocalServer,NetServer,knxd_unix)
{
public:
  LocalServer (BaseRouter& r, IniSectionPtr& s);
  virtual ~LocalServer ();

  bool setup();
  void start();
  void stop(bool err);

private:
  std::string path;
};

#endif

/** @} */
