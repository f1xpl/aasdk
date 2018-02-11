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

#include <f1x/aasdk/USB/IUSBWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

class USBWrapper: public IUSBWrapper
{
public:
    USBWrapper(libusb_context* usbContext);

    int releaseInterface(const DeviceHandle& dev_handle, int interface_number) override;
    libusb_device* getDevice(const DeviceHandle& dev_handle) override;
    int claimInterface(const DeviceHandle& dev_handle, int interface_number) override;
    DeviceHandle openDeviceWithVidPid(uint16_t vendor_id, uint16_t product_id) override;
    int getConfigDescriptor(libusb_device *dev, uint8_t config_index, ConfigDescriptorHandle& config_descriptor_handle) override;

    void fillBulkTransfer(libusb_transfer *transfer,
        const DeviceHandle& dev_handle, unsigned char endpoint,
        unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
        void *user_data, unsigned int timeout) override;

    void fillInterruptTransfer(libusb_transfer *transfer,
        const DeviceHandle& dev_handle, unsigned char endpoint,
        unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
        void *user_data, unsigned int timeout) override;

    void fillControlTransfer(
        libusb_transfer *transfer, const DeviceHandle& dev_handle,
        unsigned char *buffer, libusb_transfer_cb_fn callback, void *user_data,
        unsigned int timeout) override;

    int submitTransfer(libusb_transfer *transfer) override;
    int cancelTransfer(libusb_transfer *transfer) override;
    void freeTransfer(libusb_transfer *transfer) override;

    ssize_t getDeviceList(DeviceListHandle& handle) override;
    int open(libusb_device *dev, DeviceHandle& dev_handle) override;
    void fillControlSetup(unsigned char *buffer,
        uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
        uint16_t wLength) override;
    int getDeviceDescriptor(libusb_device *dev, libusb_device_descriptor &desc) override;
    void handleEvents() override;
    HotplugCallbackHandle hotplugRegisterCallback(libusb_hotplug_event events, libusb_hotplug_flag flags, int vendor_id, int product_id, int dev_class,
                                                  libusb_hotplug_callback_fn cb_fn, void *user_data) override;
    libusb_transfer* allocTransfer(int iso_packets) override;

private:
    libusb_context* usbContext_;
};

}
}
}
