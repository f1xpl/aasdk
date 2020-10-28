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

#include <boost/asio.hpp>
#include <list>
#include <f1x/aasdk/USB/IUSBHub.hpp>
#include <f1x/aasdk/USB/IAccessoryModeQueryChainFactory.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

class IUSBWrapper;

class USBHub: public IUSBHub, public std::enable_shared_from_this<USBHub>
{
public:
    USBHub(IUSBWrapper& usbWrapper, boost::asio::io_service& ioService, IAccessoryModeQueryChainFactory& queryChainFactory);

    void start(Promise::Pointer promise) override;
    void cancel() override;
    
private:
    typedef std::list<IAccessoryModeQueryChain::Pointer> QueryChainQueue;
    using std::enable_shared_from_this<USBHub>::shared_from_this;
    void handleDevice(libusb_device* device);
    bool isAOAPDevice(const libusb_device_descriptor& deviceDescriptor) const;
    static int hotplugEventsHandler(libusb_context* usbContext, libusb_device* device, libusb_hotplug_event event, void* uerData);

    IUSBWrapper& usbWrapper_;
    boost::asio::io_service::strand strand_;
    IAccessoryModeQueryChainFactory& queryChainFactory_;
    Promise::Pointer hotplugPromise_;
    Pointer self_;
    HotplugCallbackHandle hotplugHandle_;
    QueryChainQueue queryChainQueue_;

    static constexpr uint16_t cGoogleVendorId = 0x18D1;
    static constexpr uint16_t cAOAPId = 0x2D00;
    static constexpr uint16_t cAOAPWithAdbId = 0x2D01;

    USBHub(const USBHub&) = delete;
};

}
}
}
