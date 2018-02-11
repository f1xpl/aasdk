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

#include <iomanip>
#include <f1x/aasdk/USB/AccessoryModeSendStringQuery.hpp>
#include <f1x/aasdk/USB/USBEndpoint.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

AccessoryModeSendStringQuery::AccessoryModeSendStringQuery(boost::asio::io_service& ioService, IUSBWrapper& usbWrapper, IUSBEndpoint::Pointer usbEndpoint,
                                                           AccessoryModeSendStringType sendStringType, const std::string& queryValue)
    : AccessoryModeQuery(ioService, std::move(usbEndpoint))
    , sendStringType_(std::move(sendStringType))
{
    data_.resize(8);
    data_.insert(data_.end(), queryValue.begin(), queryValue.end());
    data_.push_back('\0');

    usbWrapper.fillControlSetup(&data_[0], LIBUSB_ENDPOINT_OUT | USB_TYPE_VENDOR, ACC_REQ_SEND_STRING, 0, static_cast<uint16_t>(sendStringType_), data_.size() - 8);
}

void AccessoryModeSendStringQuery::start(Promise::Pointer promise)
{
    strand_.dispatch([this, self = this->shared_from_this(), promise = std::move(promise)]() mutable {
        if(promise_ != nullptr)
        {
            promise->reject(error::Error(error::ErrorCode::OPERATION_IN_PROGRESS));
        }
        else
        {
            promise_ = std::move(promise);

            auto usbEndpointPromise = IUSBEndpoint::Promise::defer(strand_);
            usbEndpointPromise->then([this, self = this->shared_from_this()](size_t bytesTransferred) mutable {
                    promise_->resolve(usbEndpoint_);
                    promise_.reset();
                },
                [this, self = this->shared_from_this()](const error::Error& e) mutable {
                    promise_->reject(e);
                    promise_.reset();
                });

            usbEndpoint_->controlTransfer(common::DataBuffer(data_), cTransferTimeoutMs, std::move(usbEndpointPromise));
        }
    });
}

}
}
}
