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
    NONE = 0,
    USB_CLAIM_INTERFACE = 1,
    USB_INVALID_CONFIG_DESCRIPTOR = 2,
    USB_OBTAIN_INTERFACE_DESCRIPTOR = 3,
    USB_EMPTY_INTERFACES = 4,
    USB_INVALID_DEVICE_ENDPOINTS = 5,
    USB_INVALID_TRANSFER_METHOD = 6,
    USB_TRANSFER_ALLOCATION = 7,
    USB_LIST_DEVICES = 8,
    USB_OBTAIN_CONFIG_DESCRIPTOR = 9,
    USB_TRANSFER = 10,
    DATA_SINK_COMMIT_OVERFLOW = 11,
    DATA_SINK_CONSUME_UNDERFLOW = 12,
    USB_AOAP_PROTOCOL_VERSION = 13,
    USB_EMPTY_DEVICE_LIST = 14,
    USB_AOAP_DEVICE_NOT_FOUND = 15,
    SSL_READ_CERTIFICATE = 16,
    SSL_READ_PRIVATE_KEY = 17,
    SSL_METHOD = 18,
    SSL_CONTEXT_CREATION = 19,
    SSL_USE_CERTIFICATE = 20,
    SSL_USE_PRIVATE_KEY = 21,
    SSL_HANDLER_CREATION = 22,
    SSL_READ_BIO_CREATION = 23,
    SSL_WRITE_BIO_CREATION = 24,
    SSL_HANDSHAKE = 25,
    SSL_WRITE = 26,
    SSL_READ = 27,
    SSL_BIO_READ = 28,
    SSL_BIO_WRITE = 29,
    MESSENGER_INTERTWINED_CHANNELS = 30,
    OPERATION_ABORTED = 31,
    OPERATION_IN_PROGRESS = 32,
    PARSE_PAYLOAD = 33,
    TCP_TRANSFER = 34
};

}
}
}
