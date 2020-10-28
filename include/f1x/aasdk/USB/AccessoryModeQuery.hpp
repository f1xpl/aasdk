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
#include <libusb.h>
#include <list>
#include <f1x/aasdk/USB/IUSBEndpoint.hpp>
#include <f1x/aasdk/USB/IUSBWrapper.hpp>
#include <f1x/aasdk/USB/IAccessoryModeQuery.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

class AccessoryModeQuery: public IAccessoryModeQuery
{
public:
    AccessoryModeQuery(boost::asio::io_service& ioService, IUSBEndpoint::Pointer usbEndpoint);
    void cancel() override;

private:
    AccessoryModeQuery(const AccessoryModeQuery&) = delete;

protected:
    boost::asio::io_service::strand strand_;
    IUSBEndpoint::Pointer usbEndpoint_;
    common::Data data_;
    Promise::Pointer promise_;

    static constexpr uint32_t cTransferTimeoutMs = 1000;
    static constexpr uint32_t USB_TYPE_VENDOR = 0x40;
};

}
}
}
