/*
*  This file is part of aasdk library project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  aasdk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  aasdk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with aasdk. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <memory>
#include <list>
#include <boost/asio.hpp>
#include <libusb.h>

namespace f1x
{
namespace aasdk
{
namespace usb
{

typedef std::shared_ptr<libusb_device_handle> DeviceHandle;
typedef std::list<libusb_device*> DeviceList;
typedef std::shared_ptr<DeviceList> DeviceListHandle;
typedef std::shared_ptr<libusb_config_descriptor> ConfigDescriptorHandle;
typedef std::shared_ptr<libusb_hotplug_callback_handle> HotplugCallbackHandle;

class IUSBWrapper
{
public:
    virtual ~IUSBWrapper() = default;

    virtual int releaseInterface(const DeviceHandle& dev_handle, int interface_number) = 0;
    virtual libusb_device* getDevice(const DeviceHandle& dev_handle) = 0;
    virtual int claimInterface(const DeviceHandle& dev_handle, int interface_number) = 0;
    virtual DeviceHandle openDeviceWithVidPid(uint16_t vendor_id, uint16_t product_id) = 0;
    virtual int getConfigDescriptor(libusb_device *dev, uint8_t config_index, ConfigDescriptorHandle& config_descriptor_handle) = 0;

    virtual void fillBulkTransfer(libusb_transfer *transfer,
        const DeviceHandle& dev_handle, unsigned char endpoint,
        unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
        void *user_data, unsigned int timeout) = 0;

    virtual void fillInterruptTransfer(libusb_transfer *transfer,
        const DeviceHandle& dev_handle, unsigned char endpoint,
        unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
        void *user_data, unsigned int timeout) = 0;

    virtual void fillControlTransfer(
        libusb_transfer *transfer, const DeviceHandle& dev_handle,
        unsigned char *buffer, libusb_transfer_cb_fn callback, void *user_data,
        unsigned int timeout) = 0;

    virtual int submitTransfer(libusb_transfer *transfer) = 0;
    virtual int cancelTransfer(libusb_transfer *transfer) = 0;
    virtual void freeTransfer(libusb_transfer *transfer) = 0;

    virtual ssize_t getDeviceList(DeviceListHandle& handle) = 0;
    virtual int open(libusb_device *dev, DeviceHandle& dev_handle) = 0;
    virtual void fillControlSetup(unsigned char *buffer,
        uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
        uint16_t wLength) = 0;
    virtual int getDeviceDescriptor(libusb_device *dev, libusb_device_descriptor &desc) = 0;
    virtual void handleEvents() = 0;
    virtual HotplugCallbackHandle hotplugRegisterCallback(libusb_hotplug_event events, libusb_hotplug_flag flags, int vendor_id, int product_id, int dev_class,
                                                          libusb_hotplug_callback_fn cb_fn, void *user_data) = 0;
    virtual libusb_transfer* allocTransfer(int iso_packets) = 0;
};

}
}
}
