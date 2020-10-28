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
#include <f1x/aasdk/USB/IAccessoryModeQueryFactory.hpp>
#include <f1x/aasdk/USB/IAccessoryModeQueryChain.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{
    
class IAccessoryModeQueryFactory;

class AccessoryModeQueryChain: public IAccessoryModeQueryChain, public std::enable_shared_from_this<AccessoryModeQueryChain>
{
public:
    AccessoryModeQueryChain(IUSBWrapper& usbWrapper,
                            boost::asio::io_service& ioService,
                            IAccessoryModeQueryFactory& queryFactory);

    void start(DeviceHandle handle, Promise::Pointer promise) override;
    void cancel() override;
    
private:
    using std::enable_shared_from_this<AccessoryModeQueryChain>::shared_from_this;

    void startQuery(AccessoryModeQueryType queryType, IUSBEndpoint::Pointer usbEndpoint, IAccessoryModeQuery::Promise::Pointer queryPromise);

    void protocolVersionQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    void manufacturerQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    void modelQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    void descriptionQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    void versionQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    void uriQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    void serialQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    void startQueryHandler(IUSBEndpoint::Pointer usbEndpoint);
    
    IUSBWrapper& usbWrapper_;
    boost::asio::io_service::strand strand_;
    IAccessoryModeQueryFactory& queryFactory_;
    DeviceHandle handle_;    
    Promise::Pointer promise_;
    IAccessoryModeQuery::Pointer activeQuery_;

    AccessoryModeQueryChain(const AccessoryModeQueryChain&) = delete;
};

}
}
}
