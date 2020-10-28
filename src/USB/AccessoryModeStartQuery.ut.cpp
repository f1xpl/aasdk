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

#include <boost/test/unit_test.hpp>
#include <f1x/aasdk/USB/UT/USBWrapper.mock.hpp>
#include <f1x/aasdk/USB/UT/USBEndpoint.mock.hpp>
#include <f1x/aasdk/USB/UT/AccessoryModeQueryPromiseHandler.mock.hpp>
#include <f1x/aasdk/USB/AccessoryModeStartQuery.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{
namespace ut
{

using ::testing::_;
using ::testing::SaveArg;
using ::testing::NotNull;

class AccessoryModeStartQueryUnitTest
{
protected:
    AccessoryModeStartQueryUnitTest()
      : usbEndpointMock_(std::make_shared<USBEndpointMock>())
      , usbEndpoint_(usbEndpointMock_.get(), [](auto*) {})
      , promise_(IAccessoryModeQuery::Promise::defer(ioService_))
    {
        promise_->then(std::bind(&AccessoryModeQueryPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                      std::bind(&AccessoryModeQueryPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));
    }

    boost::asio::io_service ioService_;
    USBWrapperMock usbWrapperMock_;
    std::shared_ptr<USBEndpointMock> usbEndpointMock_;
    IUSBEndpoint::Pointer usbEndpoint_;
    AccessoryModeQueryPromiseHandlerMock promiseHandlerMock_;
    IAccessoryModeQuery::Promise::Pointer promise_;

    static constexpr uint32_t USB_TYPE_VENDOR = 0x40;
    static constexpr uint32_t ACC_REQ_START = 53;
};

BOOST_FIXTURE_TEST_CASE(AccessoryModeStartQuery_Start, AccessoryModeStartQueryUnitTest)
{
    common::DataBuffer buffer;
    IUSBEndpoint::Promise::Pointer usbEndpointPromise;
    EXPECT_CALL(*usbEndpointMock_, controlTransfer(_, _, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<2>(&usbEndpointPromise)));
    EXPECT_CALL(usbWrapperMock_, fillControlSetup(NotNull(), LIBUSB_ENDPOINT_OUT | USB_TYPE_VENDOR, ACC_REQ_START, 0, 0, 0));

    AccessoryModeStartQuery::Pointer query(std::make_shared<AccessoryModeStartQuery>(ioService_, usbWrapperMock_, usbEndpointMock_));
    query->start(std::move(promise_));
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onResolve(usbEndpoint_));

    usbEndpointPromise->resolve(buffer.size);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(AccessoryModeStartQuery_TransferError, AccessoryModeStartQueryUnitTest)
{
    common::DataBuffer buffer;
    IUSBEndpoint::Promise::Pointer usbEndpointPromise;
    EXPECT_CALL(*usbEndpointMock_, controlTransfer(_, _, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<2>(&usbEndpointPromise)));
    EXPECT_CALL(usbWrapperMock_, fillControlSetup(NotNull(), LIBUSB_ENDPOINT_OUT | USB_TYPE_VENDOR, ACC_REQ_START, 0, 0, 0));

    AccessoryModeStartQuery::Pointer query(std::make_shared<AccessoryModeStartQuery>(ioService_, usbWrapperMock_, usbEndpointMock_));
    query->start(std::move(promise_));
    ioService_.run();
    ioService_.reset();

    const error::Error transferError(error::ErrorCode::USB_TRANSFER, LIBUSB_TRANSFER_ERROR);
    EXPECT_CALL(promiseHandlerMock_, onReject(transferError));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);

    usbEndpointPromise->reject(transferError);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(AccessoryModeStartQuery_RejectWhenInProgress, AccessoryModeStartQueryUnitTest)
{
    common::DataBuffer buffer;
    IUSBEndpoint::Promise::Pointer usbEndpointPromise;
    EXPECT_CALL(*usbEndpointMock_, controlTransfer(_, _, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<2>(&usbEndpointPromise)));
    EXPECT_CALL(usbWrapperMock_, fillControlSetup(NotNull(), LIBUSB_ENDPOINT_OUT | USB_TYPE_VENDOR, ACC_REQ_START, 0, 0, 0));

    AccessoryModeStartQuery::Pointer query(std::make_shared<AccessoryModeStartQuery>(ioService_, usbWrapperMock_, usbEndpointMock_));
    query->start(std::move(promise_));
    ioService_.run();
    ioService_.reset();

    AccessoryModeQueryPromiseHandlerMock secondPromiseHandlerMock;
    auto secondPromise = IAccessoryModeQuery::Promise::defer(ioService_);
    secondPromise->then(std::bind(&AccessoryModeQueryPromiseHandlerMock::onResolve, &secondPromiseHandlerMock, std::placeholders::_1),
                       std::bind(&AccessoryModeQueryPromiseHandlerMock::onReject, &secondPromiseHandlerMock, std::placeholders::_1));

    EXPECT_CALL(secondPromiseHandlerMock, onReject(error::Error(error::ErrorCode::OPERATION_IN_PROGRESS)));
    EXPECT_CALL(secondPromiseHandlerMock, onResolve(_)).Times(0);
    query->start(std::move(secondPromise));
    ioService_.run();
}

}
}
}
}
