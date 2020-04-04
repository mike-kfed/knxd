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

#include "addrtab.h"

const uint8_t EMI2_TLL[] = { 0xA9, 0x00, 0x12, 0x34, 0x56, 0x78, 0x0A };
const uint8_t EMI2_NORM[] = { 0xA9, 0x00, 0x12, 0x34, 0x56, 0x78, 0x8A };
const uint8_t EMI2_LCON[] = { 0x43, 0x00, 0x00, 0x00, 0x00, 0x00 };
const uint8_t EMI2_LDIS[] = { 0x44, 0x00, 0x00, 0x00, 0x00, 0x00 };

const uint8_t EMI1_READ[] = { 0x4C, 0x01, 0x01, 0x16 };

int
readEMI1Mem (LowLevelDriver * iface, memaddr_t addr, uint8_t len,
             CArray & result)
{
  CArray *d1, d;
  d.resize (4);
  d[0] = 0x4C;
  d[1] = len;
  d[2] = (addr >> 8) & 0xff;
  d[3] = (addr) & 0xff;
  iface->send_Data (d);
  d1 = iface->Get_Packet (0);
  if (!d1)
    return 0;
  d = *d1;
  delete d1;
  if (d.size() != 4 + len)
    return 0;
  if (d[0] != 0x4B)
    return 0;
  if (d[1] != len)
    return 0;
  if (d[2] != ((addr >> 8) & 0xff))
    return 0;
  if (d[3] != ((addr) & 0xff))
    return 0;
  result.set (d.data() + 4, len);
  return 1;
}

int
writeEMI1Mem (LowLevelDriver * iface, memaddr_t addr, CArray data)
{
  CArray d;
  iface->SendReset ();
  d.resize (4 + data.size());
  d[0] = 0x46;
  d[1] = data.size() & 0xff;
  d[2] = (addr >> 8) & 0xff;
  d[3] = (addr) & 0xff;
  d.setpart (data, 4);
  iface->send_Data (d);
  if (!readEMI1Mem (iface, addr, data.size(), d))
    return 0;
  return d == data;
}

int
readEMI2Mem (LowLevelDriver * iface, memaddr_t addr, uint8_t len,
             CArray & result)
{
  CArray *d1, d;
  iface->SendReset ();
  iface->send_Data (CArray (EMI2_TLL, sizeof (EMI2_TLL)));
  iface->send_Data (CArray (EMI2_LCON, sizeof (EMI2_LCON)));

  // ignore ACKs
  d1 = iface->Get_Packet (0);
  if (d1)
    {
      d = *d1;
      delete d1;
      if (d.size() != 6)
        return 0;
      if (d[0] != 0x86)
        return 0;
    }
  else
    return 0;

  d.resize (11);
  d[0] = 0x41;
  d[1] = 0x00;
  d[2] = 0x00;
  d[3] = 0x00;
  d[4] = 0x00;
  d[5] = 0x00;
  d[6] = 0x03;
  d[7] = 0x02;
  d[8] = len & 0x0f;
  d[9] = (addr >> 8) & 0xff;
  d[10] = (addr) & 0xff;

  iface->send_Data (d);

  // ignore ACKs
  d1 = iface->Get_Packet (0);
  if (d1)
    {
      d = *d1;
      delete d1;
      if (d.size() != 11)
        return 0;
      if (d[0] != 0x8E)
        return 0;
    }
  else
    return 0;

  d1 = iface->Get_Packet (0);
  if (!d1)
    return 0;
  d = *d1;
  delete d1;
  if (d.size() != 11 + len)
    return 0;
  if (d[0] != 0x89)
    return 0;
  if (d[1] != 0x00)
    return 0;
  if (d[2] != 0x00)
    return 0;
  if (d[3] != 0x00)
    return 0;
  if (d[4] != 0x00)
    return 0;
  if (d[5] != 0x00)
    return 0;
  if (d[6] != 0x03 + len)
    return 0;
  if ((d[7] & 0x03) != 0x02)
    return 0;
  if (d[8] != (0x40 | len))
    return 0;
  if (d[9] != ((addr >> 8) & 0xff))
    return 0;
  if (d[10] != ((addr) & 0xff))
    return 0;
  result.set (d.data() + 11, len);
  iface->send_Data (CArray (EMI2_LDIS, sizeof (EMI2_LDIS)));
  d1 = iface->Get_Packet (0);
  if (!d1)
    return 0;
  else
    {
      d = *d1;
      delete d1;
      if (d.size() != 6)
        return 0;
      if (d[0] != 0x88)
        return 0;
    }
  iface->send_Data (CArray (EMI2_NORM, sizeof (EMI2_NORM)));
  return 1;
}

int
writeEMI2Mem (LowLevelDriver * iface, memaddr_t addr, CArray data)
{
  CArray *d1, d;
  iface->SendReset ();
  iface->send_Data (CArray (EMI2_TLL, sizeof (EMI2_TLL)));

  iface->send_Data (CArray (EMI2_LCON, sizeof (EMI2_LCON)));

  // ignore ACKs
  d1 = iface->Get_Packet (0);
  if (d1)
    {
      d = *d1;
      delete d1;
      if (d.size() != 6)
        return 0;
      if (d[0] != 0x86)
        return 0;
    }
  else
    return 0;

  d.resize (11 + data.size());
  d[0] = 0x41;
  d[1] = 0x00;
  d[2] = 0x00;
  d[3] = 0x00;
  d[4] = 0x00;
  d[5] = 0x00;
  d[6] = 0x03;
  d[7] = 0x02;
  d[8] = (0x80 | (data.size() & 0x0f));
  d[9] = (addr >> 8) & 0xff;
  d[10] = (addr) & 0xff;
  d.setpart (data, 11);

  iface->send_Data (d);
  d1 = iface->Get_Packet (0);
  if (d1)
    {
      d = *d1;
      delete d1;
      if (d.size() != 11 + data.size())
        return 0;
      if (d[0] != 0x8E)
        return 0;
    }
  else
    return 0;

  iface->send_Data (CArray (EMI2_LDIS, sizeof (EMI2_LDIS)));
  d1 = iface->Get_Packet (0);
  if (!d1)
    return 0;
  else
    {
      d = *d1;
      delete d1;
      if (d.size() != 6)
        return 0;
      if (d[0] != 0x88)
        return 0;
    }

  iface->send_Data (CArray (EMI2_NORM, sizeof (EMI2_NORM)));

  if (!readEMI2Mem (iface, addr, data.size(), d))
    return 0;
  return d == data;
}

int
readEMIMem (LowLevelDriver * iface, memaddr_t addr, uint8_t len,
            CArray & result)
{
  switch (iface->getEMIVer ())
    {
    case LowLevelDriver::vEMI1:
      return readEMI1Mem (iface, addr, len, result);
    case LowLevelDriver::vEMI2:
      return readEMI2Mem (iface, addr, len, result);

    default:
      return 0;
    }
}

int
writeEMIMem (LowLevelDriver * iface, memaddr_t addr, CArray data)
{
  switch (iface->getEMIVer ())
    {
    case LowLevelDriver::vEMI1:
      return writeEMI1Mem (iface, addr, data);
    case LowLevelDriver::vEMI2:
      return writeEMI2Mem (iface, addr, data);

    default:
      return 0;
    }
}

int
readAddrTabSize (LowLevelDriver * iface, uint8_t & result)
{
  CArray x;
  if (!readEMIMem (iface, 0x116, 1, x))
    return 0;
  result = x[0];
  return 1;
}

int
writeAddrTabSize (LowLevelDriver * iface, uint8_t size)
{
  CArray x;
  x.resize (1);
  x[0] = size;
  return writeEMIMem (iface, 0x116, x);
}
