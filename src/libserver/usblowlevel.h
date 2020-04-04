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

#ifndef EIB_USB_H
#define EIB_USB_H

#include <libusb.h>

#include "lowlevel.h"
#include "usb.h"

struct USBEndpoint
{
  int bus;
  int device;
  int config;
  int altsetting;
  int interface;
};

struct USBDevice
{
  libusb_device *dev;
  int config;
  int altsetting;
  int interface;
  int sendep;
  int recvep;
};

USBEndpoint parseUSBEndpoint (const char *addr);
USBDevice detectUSBEndpoint (USBEndpoint e);

enum UState
{
  sNone = 0,
  sStarted,
  sReleasing,
  sClaimed,
  sRunning,
  sConnected,
};

class USBLowLevelDriver : public LowLevelDriver
{
public:
  USBLowLevelDriver (LowLevelIface* p, IniSectionPtr& s);
  virtual ~USBLowLevelDriver ();

  bool setup();
  void start();
  void stop(bool err);
  void send_Data (CArray& l);
  void abort_send();

  // for use by callbacks only
  void CompleteReceive(struct libusb_transfer *recvh);
  void CompleteSend(struct libusb_transfer *recvh);

private:
  libusb_device_handle *dev;
  /* libusb event loop */
  USBLoop *loop;
  USBDevice d;
  USBEndpoint e;

  /** transmit buffer */
  CArray out;
  /** transmit retry counter */
  int send_retry = 0;
  int send_timeout = 1000;

  UState state = sNone;
  bool stopping = false;
  bool stopped_err = false;
  uint8_t sendbuf[64];
  uint8_t recvbuf[64];
  bool startUsbRecvTransferFailed = false;

  struct libusb_transfer *sendh = 0;
  struct libusb_transfer *recvh = 0;

  void StartUsbRecvTransfer();
  void HandleReceiveUsb();
  virtual void reset();
  void do_send();
  void do_send_Next();
  void stop_(bool force);

  // need to do the trigger callbacks outside of libusb
  ev::async read_trigger;
  void read_trigger_cb(ev::async &w, int revents);
  ev::async write_trigger;
  void write_trigger_cb(ev::async &w, int revents);
};

#endif

/** @} */
