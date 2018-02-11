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

#include <gmock/gmock.h>
#include <f1x/aasdk/USB/IUSBWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{
namespace ut
{

class USBWrapperMock: public IUSBWrapper
{
public:
    struct DummyDeviceHandle {};

    MOCK_METHOD2(releaseInterface, int(const DeviceHandle& dev_handle, int interface_number));
    MOCK_METHOD1(getDevice, libusb_device*(const DeviceHandle& dev_handle));
    MOCK_METHOD2(claimInterface, int(const DeviceHandle& dev_handle, int interface_number));
    MOCK_METHOD2(openDeviceWithVidPid, DeviceHandle(uint16_t vendor_id, uint16_t product_id));
    MOCK_METHOD3(getConfigDescriptor, int(libusb_device *dev, uint8_t config_index, ConfigDescriptorHandle& config_descriptor_handle));

    MOCK_METHOD8(fillBulkTransfer, void(libusb_transfer *transfer,
        const DeviceHandle& dev_handle, unsigned char endpoint,
        unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
        void *user_data, unsigned int timeout));

    MOCK_METHOD8(fillInterruptTransfer, void(libusb_transfer *transfer,
        const DeviceHandle& dev_handle, unsigned char endpoint,
        unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
        void *user_data, unsigned int timeout));

    MOCK_METHOD6(fillControlTransfer, void(
        libusb_transfer *transfer, const DeviceHandle& dev_handle,
        unsigned char *buffer, libusb_transfer_cb_fn callback, void *user_data,
        unsigned int timeout));

    MOCK_METHOD1(submitTransfer, int(libusb_transfer *transfer));
    MOCK_METHOD1(cancelTransfer, int(libusb_transfer *transfer));
    MOCK_METHOD1(freeTransfer, void(libusb_transfer *transfer));

    MOCK_METHOD1(getDeviceList, ssize_t(DeviceListHandle& handle));
    MOCK_METHOD2(open, int(libusb_device *dev, DeviceHandle& dev_handle));
    MOCK_METHOD6(fillControlSetup, void(unsigned char *buffer,
        uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
        uint16_t wLength));
    MOCK_METHOD2(getDeviceDescriptor, int(libusb_device *dev, libusb_device_descriptor &desc));
    MOCK_METHOD0(handleEvents, void());
    MOCK_METHOD7(hotplugRegisterCallback, HotplugCallbackHandle(libusb_hotplug_event events, libusb_hotplug_flag flags, int vendor_id, int product_id, int dev_class,
                                                                libusb_hotplug_callback_fn cb_fn, void *user_data));
    MOCK_METHOD1(allocTransfer, libusb_transfer*(int iso_packets));
};

}
}
}
}
