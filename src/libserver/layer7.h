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
 * @ingroup KNX_03_03_07
 * Application Layer
 * @{
 */

#ifndef LAYER7_H
#define LAYER7_H

#include "layer4.h"

class APDU;

/** layer 7 broadcast connection */
class Layer7_Broadcast : T_Reader<BroadcastComm>
{
public:
  Layer7_Broadcast (TracePtr tr);
  virtual ~Layer7_Broadcast ();
  bool init (Router * l3);
  void recv (BroadcastComm *c);

  /** send IndividualAddress_Write */
  void A_IndividualAddress_Write (eibaddr_t addr);
  /** sends A_IndividualAddress_Read and collects responses */
  std::vector < eibaddr_t > A_IndividualAddress_Read (TracePtr tr, unsigned timeout = 3);

private:
  TracePtr tr;
  T_BroadcastPtr l4;
};

/** Layer 7 Individual Connection */
class Layer7_Connection : T_Reader<BroadcastComm>
{
public:
  Layer7_Connection (TracePtr tr, eibaddr_t dest);
  virtual ~Layer7_Connection () = default;
  bool init (Router * l3);
  void recv (BroadcastComm *c);

  /** send A_Restart */
  void A_Restart ();
  /** read a property */
  int A_Property_Read (uint8_t obj, uint8_t propertyid, uint16_t start,
                       uint8_t count, CArray & erg);
  /** write a property */
  int A_Property_Write (uint8_t obj, uint8_t propertyid, uint16_t start,
                        uint8_t count, const CArray & data, CArray & result);
  /** descripbe a property */
  int A_Property_Desc (uint8_t obj, uint8_t & property, uint8_t property_index,
                       uint8_t & type, uint16_t & max_nr_elements,
                       uint8_t & access);
  /** read device descript (mask version) */
  int A_Device_Descriptor_Read (uint16_t & maskver, uint8_t type = 0);
  /** read ADC */
  int A_ADC_Read (uint8_t channel, uint8_t readcount, int16_t & value);
  /** read memory */
  int A_Memory_Read (memaddr_t addr, uint8_t len, CArray & data);
  /** write memory */
  int A_Memory_Write (memaddr_t addr, const CArray & data);
  /** try to authorize */
  int A_Authorize (eibkey_type key, uint8_t & level);
  /** try to write a key */
  int A_KeyWrite (eibkey_type key, uint8_t & level);

  /** write a property and verify */
  int X_Property_Write (uint8_t obj, uint8_t propertyid, uint16_t start,
                        uint8_t count, const CArray & data);
  /** write memory and verify */
  int X_Memory_Write (memaddr_t addr, const CArray & data);

  /** write arbitray memory block and verify */
  int X_Memory_Write_Block (memaddr_t addr, const CArray & data);
  /** read arbitray memory block*/
  int X_Memory_Read_Block (memaddr_t addr, unsigned int len, CArray & erg);
  /** write arbitray memory block without verify */
  int A_Memory_Write_Block (memaddr_t addr, const CArray & data);

private:
  TracePtr tr;
  T_ConnectionPtr l4;
  /** destination address */
  eibaddr_t dest;

  /** sends APDU and waits for respone; return NULL, if it fails */
  APDUPtr Request_Response (APDU * r);
};

/** Layer 7 Individual  */
class Layer7_Individual
{
public:
  Layer7_Individual (TracePtr tr, eibaddr_t dest);
  virtual ~Layer7_Individual () = default;
  bool init (Router * l3);

  /** read a property */
  int A_Property_Read (uint8_t obj, uint8_t propertyid, uint16_t start,
                       uint8_t count, CArray & erg);
  /** write a property */
  int A_Property_Write (uint8_t obj, uint8_t propertyid, uint16_t start,
                        uint8_t count, const CArray & data, CArray & result);

private:
  TracePtr tr;
  T_IndividualPtr l4;
  /** destination address */
  eibaddr_t dest;

  /** sends APDU and waits for respone; return NULL, if it fails */
  APDUPtr Request_Response (APDU * r);
};

#endif

/** @} */
