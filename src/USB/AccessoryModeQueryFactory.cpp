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

#include <f1x/aasdk/USB/AccessoryModeQueryFactory.hpp>
#include <f1x/aasdk/USB/AccessoryModeSendStringQuery.hpp>
#include <f1x/aasdk/USB/AccessoryModeStartQuery.hpp>
#include <f1x/aasdk/USB/AccessoryModeProtocolVersionQuery.hpp>
#include <f1x/aasdk/USB/AccessoryModeSendStringType.hpp>


namespace f1x
{
namespace aasdk
{
namespace usb
{

AccessoryModeQueryFactory::AccessoryModeQueryFactory(usb::IUSBWrapper& usbWrapper, boost::asio::io_service& ioService)
    : usbWrapper_(usbWrapper)
    , ioService_(ioService)
{

}

IAccessoryModeQuery::Pointer AccessoryModeQueryFactory::createQuery(AccessoryModeQueryType queryType, IUSBEndpoint::Pointer usbEndpoint)
{
    switch(queryType)
    {
    case AccessoryModeQueryType::PROTOCOL_VERSION:
        return std::make_shared<AccessoryModeProtocolVersionQuery>(ioService_, usbWrapper_, std::move(usbEndpoint));

    case AccessoryModeQueryType::SEND_DESCRIPTION:
        return std::make_shared<AccessoryModeSendStringQuery>(ioService_, usbWrapper_, std::move(usbEndpoint),
                                                              AccessoryModeSendStringType::DESCRIPTION, "Android Auto");

    case AccessoryModeQueryType::SEND_MANUFACTURER:
        return std::make_shared<AccessoryModeSendStringQuery>(ioService_, usbWrapper_, std::move(usbEndpoint),
                                                              AccessoryModeSendStringType::MANUFACTURER, "Android");

    case AccessoryModeQueryType::SEND_MODEL:
        return std::make_shared<AccessoryModeSendStringQuery>(ioService_, usbWrapper_, std::move(usbEndpoint),
                                                              AccessoryModeSendStringType::MODEL, "Android Auto");

    case AccessoryModeQueryType::SEND_SERIAL:
        return std::make_shared<AccessoryModeSendStringQuery>(ioService_, usbWrapper_, std::move(usbEndpoint),
                                                              AccessoryModeSendStringType::SERIAL, "HU-AAAAAA001");

    case AccessoryModeQueryType::SEND_URI:
        return std::make_shared<AccessoryModeSendStringQuery>(ioService_, usbWrapper_, std::move(usbEndpoint),
                                                              AccessoryModeSendStringType::URI, "https://f1xstudio.com");

    case AccessoryModeQueryType::SEND_VERSION:
        return std::make_shared<AccessoryModeSendStringQuery>(ioService_, usbWrapper_, std::move(usbEndpoint),
                                                              AccessoryModeSendStringType::VERSION, "2.0.1");

    case AccessoryModeQueryType::START:
        return std::make_shared<AccessoryModeStartQuery>(ioService_, usbWrapper_, std::move(usbEndpoint));

    default:
        return nullptr;
    }
}

}
}
}
