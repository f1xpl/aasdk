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

#include <f1x/aasdk/USB/AccessoryModeQuery.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

class AccessoryModeProtocolVersionQuery: public AccessoryModeQuery, public std::enable_shared_from_this<AccessoryModeProtocolVersionQuery>
{
public:
    AccessoryModeProtocolVersionQuery(boost::asio::io_service& ioService, IUSBWrapper& usbWrapper, IUSBEndpoint::Pointer usbEndpoint);
    void start(Promise::Pointer promise) override;

private:
    using std::enable_shared_from_this<AccessoryModeProtocolVersionQuery>::shared_from_this;
    void protocolVersionHandler(size_t bytesTransferred);

    typedef uint16_t ProtocolVersion;
    static constexpr uint32_t ACC_REQ_GET_PROTOCOL = 51;
};

}
}
}
