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

#include <f1x/aasdk/USB/AccessoryModeQueryChain.hpp>
#include <f1x/aasdk/Error/Error.hpp>
#include <f1x/aasdk/USB/USBEndpoint.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

AccessoryModeQueryChain::AccessoryModeQueryChain(IUSBWrapper& usbWrapper,
                                                 boost::asio::io_service& ioService,
                                                 IAccessoryModeQueryFactory& queryFactory)
    : usbWrapper_(usbWrapper)
    , strand_(ioService)
    , queryFactory_(queryFactory)
{

}

void AccessoryModeQueryChain::start(DeviceHandle handle, Promise::Pointer promise)
{   
    strand_.dispatch([this, self = this->shared_from_this(), handle = std::move(handle), promise = std::move(promise)]() mutable {
        if(promise_ != nullptr)
        {
            promise->reject(error::Error(error::ErrorCode::OPERATION_IN_PROGRESS));
        }
        else
        {
            promise_ = std::move(promise);

            auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
            queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
                    this->protocolVersionQueryHandler(std::move(usbEndpoint));
                },
                [this, self = this->shared_from_this()](const error::Error& e) mutable {
                    promise_->reject(e);
                    promise_.reset();
                });

            this->startQuery(AccessoryModeQueryType::PROTOCOL_VERSION,
                             std::make_shared<USBEndpoint>(usbWrapper_, strand_.context(), std::move(handle)),
                             std::move(queryPromise));
        }
    });
}

void AccessoryModeQueryChain::cancel()
{
    strand_.dispatch([this, self = this->shared_from_this()]() {
        if(activeQuery_ != nullptr)
        {
            activeQuery_->cancel();
            activeQuery_.reset();
        }
    });
}

void AccessoryModeQueryChain::startQuery(AccessoryModeQueryType queryType, IUSBEndpoint::Pointer usbEndpoint, IAccessoryModeQuery::Promise::Pointer queryPromise)
{
    activeQuery_ = queryFactory_.createQuery(queryType, std::move(usbEndpoint));
    activeQuery_->start(std::move(queryPromise));
}

void AccessoryModeQueryChain::protocolVersionQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
    queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
            this->manufacturerQueryHandler(std::move(usbEndpoint));
        },
        [this, self = this->shared_from_this()](const error::Error& e) mutable {
            promise_->reject(e);
            promise_.reset();
        });

    this->startQuery(AccessoryModeQueryType::SEND_MANUFACTURER,
                     std::move(usbEndpoint),
                     std::move(queryPromise));
}

void AccessoryModeQueryChain::manufacturerQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
    queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
            this->modelQueryHandler(std::move(usbEndpoint));
        },
        [this, self = this->shared_from_this()](const error::Error& e) mutable {
            promise_->reject(e);
            promise_.reset();
        });

    this->startQuery(AccessoryModeQueryType::SEND_MODEL,
                     std::move(usbEndpoint),
                     std::move(queryPromise));
}

void AccessoryModeQueryChain::modelQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
    queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
            this->descriptionQueryHandler(std::move(usbEndpoint));
        },
        [this, self = this->shared_from_this()](const error::Error& e) mutable {
            promise_->reject(e);
            promise_.reset();
        });

    this->startQuery(AccessoryModeQueryType::SEND_DESCRIPTION,
                     std::move(usbEndpoint),
                     std::move(queryPromise));
}

void AccessoryModeQueryChain::descriptionQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
    queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
            this->versionQueryHandler(std::move(usbEndpoint));
        },
        [this, self = this->shared_from_this()](const error::Error& e) mutable {
            promise_->reject(e);
            promise_.reset();
        });

    this->startQuery(AccessoryModeQueryType::SEND_VERSION,
                     std::move(usbEndpoint),
                     std::move(queryPromise));
}

void AccessoryModeQueryChain::versionQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
    queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
            this->uriQueryHandler(std::move(usbEndpoint));
        },
        [this, self = this->shared_from_this()](const error::Error& e) mutable {
            promise_->reject(e);
            promise_.reset();
        });

    this->startQuery(AccessoryModeQueryType::SEND_URI,
                     std::move(usbEndpoint),
                     std::move(queryPromise));
}

void AccessoryModeQueryChain::uriQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
    queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
            this->serialQueryHandler(std::move(usbEndpoint));
        },
        [this, self = this->shared_from_this()](const error::Error& e) mutable {
            promise_->reject(e);
            promise_.reset();
        });

    this->startQuery(AccessoryModeQueryType::SEND_SERIAL,
                     std::move(usbEndpoint),
                     std::move(queryPromise));
}

void AccessoryModeQueryChain::serialQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    auto queryPromise = IAccessoryModeQuery::Promise::defer(strand_);
    queryPromise->then([this, self = this->shared_from_this()](IUSBEndpoint::Pointer usbEndpoint) mutable {
            this->startQueryHandler(std::move(usbEndpoint));
        },
        [this, self = this->shared_from_this()](const error::Error& e) mutable {
            promise_->reject(e);
            promise_.reset();
        });

    this->startQuery(AccessoryModeQueryType::START,
                     std::move(usbEndpoint),
                     std::move(queryPromise));
}

void AccessoryModeQueryChain::startQueryHandler(IUSBEndpoint::Pointer usbEndpoint)
{
    activeQuery_.reset();
    promise_->resolve(usbEndpoint->getDeviceHandle());
    promise_.reset();
}

}
}
}
