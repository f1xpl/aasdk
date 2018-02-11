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

#include <stdexcept>
#include <f1x/aasdk/USB/USBEndpoint.hpp>
#include <f1x/aasdk/USB/AOAPDevice.hpp>
#include <f1x/aasdk/Error/Error.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

AOAPDevice::AOAPDevice(IUSBWrapper& usbWrapper, boost::asio::io_service& ioService, DeviceHandle handle, const libusb_interface_descriptor* interfaceDescriptor)
    : usbWrapper_(usbWrapper)
    , handle_(std::move(handle))
    , interfaceDescriptor_(interfaceDescriptor)
{
    if((interfaceDescriptor->endpoint[0].bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN)
    {
        inEndpoint_ = std::make_shared<USBEndpoint>(usbWrapper_, ioService, handle_, interfaceDescriptor_->endpoint[0].bEndpointAddress);
        outEndpoint_ = std::make_shared<USBEndpoint>(usbWrapper_, ioService, handle_, interfaceDescriptor_->endpoint[1].bEndpointAddress);
    }
    else
    {
        inEndpoint_ = std::make_shared<USBEndpoint>(usbWrapper_, ioService, handle_, interfaceDescriptor_->endpoint[1].bEndpointAddress);
        outEndpoint_ = std::make_shared<USBEndpoint>(usbWrapper_, ioService, handle_, interfaceDescriptor_->endpoint[0].bEndpointAddress);
    }
}

AOAPDevice::~AOAPDevice()
{
    inEndpoint_->cancelTransfers();
    outEndpoint_->cancelTransfers();
    usbWrapper_.releaseInterface(handle_, interfaceDescriptor_->bInterfaceNumber);
}

IUSBEndpoint& AOAPDevice::getInEndpoint()
{
    return *inEndpoint_;
}

IUSBEndpoint& AOAPDevice::getOutEndpoint()
{
    return *outEndpoint_;
}

IAOAPDevice::Pointer AOAPDevice::create(IUSBWrapper& usbWrapper, boost::asio::io_service& ioService, DeviceHandle handle)
{
    auto configDescriptorHandle = AOAPDevice::getConfigDescriptor(usbWrapper, handle);
    auto interface = AOAPDevice::getInterface(configDescriptorHandle);
    auto interfaceDescriptor = AOAPDevice::getInterfaceDescriptor(interface);

    if(interfaceDescriptor->bNumEndpoints < 2)
    {
        throw error::Error(error::ErrorCode::USB_INVALID_DEVICE_ENDPOINTS);
    }

    auto result = usbWrapper.claimInterface(handle, interfaceDescriptor->bInterfaceNumber);

    if(result != 0)
    {
        throw error::Error(error::ErrorCode::USB_CLAIM_INTERFACE, result);
    }

    return std::make_unique<AOAPDevice>(usbWrapper, ioService, std::move(handle), interfaceDescriptor);
}

ConfigDescriptorHandle AOAPDevice::getConfigDescriptor(IUSBWrapper& usbWrapper, DeviceHandle handle)
{
    ConfigDescriptorHandle configDescriptorHandle;
    libusb_device* device = usbWrapper.getDevice(handle);

    auto result = usbWrapper.getConfigDescriptor(device, 0, configDescriptorHandle);
    if(result != 0)
    {
        throw error::Error(error::ErrorCode::USB_OBTAIN_CONFIG_DESCRIPTOR, result);
    }

    if(configDescriptorHandle == nullptr)
    {
        throw error::Error(error::ErrorCode::USB_INVALID_CONFIG_DESCRIPTOR, result);
    }

    return configDescriptorHandle;
}

const libusb_interface* AOAPDevice::getInterface(const ConfigDescriptorHandle& configDescriptorHandle)
{
    if(configDescriptorHandle->bNumInterfaces == 0)
    {
        throw error::Error(error::ErrorCode::USB_EMPTY_INTERFACES);
    }

    return &(configDescriptorHandle->interface[0]);
}

const libusb_interface_descriptor* AOAPDevice::getInterfaceDescriptor(const libusb_interface* interface)
{
    if(interface->num_altsetting == 0)
    {
        throw error::Error(error::ErrorCode::USB_OBTAIN_INTERFACE_DESCRIPTOR);
    }

    return &interface->altsetting[0];
}

}
}
}
