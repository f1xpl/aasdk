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

#include <stdint.h>

namespace f1x
{
namespace aasdk
{
namespace error
{

enum class ErrorCode
{
    NONE,
    USB_CLAIM_INTERFACE,
    USB_INVALID_CONFIG_DESCRIPTOR,
    USB_OBTAIN_INTERFACE_DESCRIPTOR,
    USB_EMPTY_INTERFACES,
    USB_INVALID_DEVICE_ENDPOINTS,
    USB_INVALID_TRANSFER_METHOD,
    USB_TRANSFER_ALLOCATION,
    USB_LIST_DEVICES,
    USB_OBTAIN_CONFIG_DESCRIPTOR,
    USB_TRANSFER,
    USB_SINK_COMMIT_OVERFLOW,
    USB_SINK_CONSUME_UNDERFLOW,
    USB_AOAP_PROTOCOL_VERSION,
    USB_EMPTY_DEVICE_LIST,
    USB_AOAP_DEVICE_NOT_FOUND,
    SSL_READ_CERTIFICATE,
    SSL_READ_PRIVATE_KEY,
    SSL_METHOD,
    SSL_CONTEXT_CREATION,
    SSL_USE_CERTIFICATE,
    SSL_USE_PRIVATE_KEY,
    SSL_HANDLER_CREATION,
    SSL_READ_BIO_CREATION,
    SSL_WRITE_BIO_CREATION,
    SSL_HANDSHAKE,
    SSL_WRITE,
    SSL_READ,
    SSL_BIO_READ,
    SSL_BIO_WRITE,
    MESSENGER_INTERTWINED_CHANNELS,
    OPERATION_ABORTED,
    OPERATION_IN_PROGRESS,
    PARSE_PAYLOAD
};

}
}
}
