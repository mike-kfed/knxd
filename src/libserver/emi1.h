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
 * @ingroup KNX_03_06_03_03
 * External Message Interface 1.x
 * @{
 */

#ifndef EIB_EMI1_H
#define EIB_EMI1_H

#include "emi_common.h"

/** EMI1 backend */
class EMI1Driver:public EMI_Common
{
public:
  EMI1Driver (LowLevelIface* c, IniSectionPtr& s, LowLevelDriver *i = nullptr);
  virtual ~EMI1Driver () = default;

private:
  virtual void cmdEnterMonitor() override;
  virtual void cmdLeaveMonitor() override;
  virtual void cmdOpen() override;
  virtual void cmdClose() override;
  virtual const uint8_t * getIndTypes() const override;
  virtual EMIVer getVersion() const override
  {
    return vEMI1;
  }
  void sendLocal_done_cb(bool success);
  enum { N_bad, N_up, N_want_close, N_down, N_open } sendLocal_done_next = N_bad;
  void do_send_Next();
};

#endif

/** @} */
