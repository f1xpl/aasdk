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

#include <f1x/aasdk/USB/USBWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

USBWrapper::USBWrapper(libusb_context* usbContext)
    : usbContext_(usbContext)
{

}

int USBWrapper::releaseInterface(const DeviceHandle& dev_handle, int interface_number)
{
    return libusb_release_interface(dev_handle.get(), interface_number);
}

libusb_device* USBWrapper::getDevice(const DeviceHandle& dev_handle)
{
    return libusb_get_device(dev_handle.get());
}

int USBWrapper::claimInterface(const DeviceHandle& dev_handle, int interface_number)
{
    return libusb_claim_interface(dev_handle.get(), interface_number);
}

DeviceHandle USBWrapper::openDeviceWithVidPid(uint16_t vendor_id, uint16_t product_id)
{
    auto raw_handle = libusb_open_device_with_vid_pid(usbContext_, vendor_id, product_id);
    return raw_handle != nullptr ? DeviceHandle(raw_handle, &libusb_close) : DeviceHandle();
}

int USBWrapper::getConfigDescriptor(libusb_device *dev, uint8_t config_index, ConfigDescriptorHandle& config_descriptor_handle)
{
    libusb_config_descriptor* raw_handle = nullptr;
    auto result = libusb_get_config_descriptor(dev, config_index, &raw_handle);

    config_descriptor_handle = (result == 0 && raw_handle != nullptr) ? ConfigDescriptorHandle(raw_handle, &libusb_free_config_descriptor) : ConfigDescriptorHandle();
    return result;
}

void USBWrapper::fillBulkTransfer(libusb_transfer *transfer,
    const DeviceHandle& dev_handle, unsigned char endpoint,
    unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
    void *user_data, unsigned int timeout)
{
    libusb_fill_bulk_transfer(transfer, dev_handle.get(), endpoint, buffer, length, callback, user_data, timeout);
}

void USBWrapper::fillInterruptTransfer(libusb_transfer *transfer,
    const DeviceHandle& dev_handle, unsigned char endpoint,
    unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
    void *user_data, unsigned int timeout)
{
    libusb_fill_interrupt_transfer(transfer, dev_handle.get(), endpoint, buffer, length, callback, user_data, timeout);
}

void USBWrapper::fillControlTransfer(
    libusb_transfer *transfer, const DeviceHandle& dev_handle,
    unsigned char *buffer, libusb_transfer_cb_fn callback, void *user_data,
    unsigned int timeout)
{
    libusb_fill_control_transfer(transfer, dev_handle.get(), buffer, callback, user_data, timeout);
}

int USBWrapper::submitTransfer(libusb_transfer *transfer)
{
    return libusb_submit_transfer(transfer);
}

int USBWrapper::cancelTransfer(libusb_transfer *transfer)
{
    return libusb_cancel_transfer(transfer);
}

void USBWrapper::freeTransfer(libusb_transfer *transfer)
{
    libusb_free_transfer(transfer);
}

ssize_t USBWrapper::getDeviceList(DeviceListHandle& handle)
{
    libusb_device** raw_handle;
    auto result = libusb_get_device_list(usbContext_, &raw_handle);

    if(result >= 0)
    {
        handle = DeviceListHandle(new DeviceList(raw_handle, raw_handle + result),
                                  [raw_handle](auto in_device_list)
        {
            if(!in_device_list->empty())
            {
                libusb_free_device_list(raw_handle, 1);
            }

            in_device_list->clear();
            delete in_device_list;
        });
    }
    else
    {
        handle = DeviceListHandle();
    }

    return result;
}

int USBWrapper::open(libusb_device *dev, DeviceHandle& dev_handle)
{
    libusb_device_handle* raw_handle;
    auto result = libusb_open(dev, &raw_handle);

    dev_handle = (result == 0 && raw_handle != nullptr) ? DeviceHandle(raw_handle, &libusb_close) : DeviceHandle();
    return result;
}

void USBWrapper::fillControlSetup(unsigned char *buffer,
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
    uint16_t wLength)
{
    libusb_fill_control_setup(buffer, bmRequestType, bRequest, wValue, wIndex, wLength);
}

int USBWrapper::getDeviceDescriptor(libusb_device *dev, libusb_device_descriptor &desc)
{
    return libusb_get_device_descriptor(dev, &desc);
}

void USBWrapper::handleEvents()
{
    libusb_handle_events(usbContext_);
}

HotplugCallbackHandle USBWrapper::hotplugRegisterCallback(libusb_hotplug_event events, libusb_hotplug_flag flags, int vendor_id, int product_id, int dev_class,
                                                          libusb_hotplug_callback_fn cb_fn, void *user_data)
{
    libusb_hotplug_callback_handle raw_handle;
    libusb_hotplug_register_callback (usbContext_, events, flags, vendor_id, product_id, dev_class, cb_fn, user_data, &raw_handle);

    HotplugCallbackHandle handle(&raw_handle, [this](auto raw_handle) { libusb_hotplug_deregister_callback(usbContext_, *raw_handle); });
    return handle;
}

libusb_transfer* USBWrapper::allocTransfer(int iso_packets)
{
    return libusb_alloc_transfer(iso_packets);
}

}
}
}
