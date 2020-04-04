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
 * @addtogroup Driver
 * @{
 */

#ifndef LLTCP
#define LLTCP

#include "iobuf.h"
#include "lowlevel.h"

class LLtcp:public FDdriver
{
public:
  LLtcp (LowLevelIface* parent, IniSectionPtr& s) : FDdriver(parent,s)
  {
    t->setAuxName("TCP");
  }
  virtual ~LLtcp () = default;

  bool setup();
  void start();

private:
  std::string dest;
  uint16_t port;
};

#endif

/** @} */
