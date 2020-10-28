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
#include <f1x/aasdk/USB/AccessoryModeProtocolVersionQuery.hpp>

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

class AccessoryModeProtocolVersionQueryUnitTest
{
protected:
    AccessoryModeProtocolVersionQueryUnitTest()
      : usbEndpointMock_(std::make_shared<USBEndpointMock>())
      , usbEndpoint_(usbEndpointMock_.get(), [](auto*) {})
      , promise_(IAccessoryModeQuery::Promise::defer(ioService_))
    {
        promise_->then(std::bind(&AccessoryModeQueryPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                      std::bind(&AccessoryModeQueryPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));
    }

    void scenario_ValidProtocolVersion(uint16_t protocolVersion)
    {
        common::DataBuffer buffer;
        IUSBEndpoint::Promise::Pointer usbEndpointPromise;
        EXPECT_CALL(*usbEndpointMock_, controlTransfer(_, _, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<2>(&usbEndpointPromise)));
        EXPECT_CALL(usbWrapperMock_, fillControlSetup(NotNull(), LIBUSB_ENDPOINT_IN | USB_TYPE_VENDOR, ACC_REQ_GET_PROTOCOL, 0, 0, sizeof(protocolVersion)));

        AccessoryModeProtocolVersionQuery::Pointer query(std::make_shared<AccessoryModeProtocolVersionQuery>(ioService_, usbWrapperMock_, usbEndpointMock_));
        query->start(std::move(promise_));
        ioService_.run();
        ioService_.reset();

        reinterpret_cast<uint16_t&>(buffer.data[8]) = protocolVersion;
        usbEndpointPromise->resolve(buffer.size);

        EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
        EXPECT_CALL(promiseHandlerMock_, onResolve(usbEndpoint_));
        ioService_.run();
    }

    boost::asio::io_service ioService_;
    USBWrapperMock usbWrapperMock_;
    std::shared_ptr<USBEndpointMock> usbEndpointMock_;
    IUSBEndpoint::Pointer usbEndpoint_;
    AccessoryModeQueryPromiseHandlerMock promiseHandlerMock_;
    IAccessoryModeQuery::Promise::Pointer promise_;

    static constexpr uint32_t USB_TYPE_VENDOR = 0x40;
    static constexpr uint32_t ACC_REQ_GET_PROTOCOL = 51;
};

BOOST_FIXTURE_TEST_CASE(AccessoryModeProtocolVersionQuery_ProtcolVersion1, AccessoryModeProtocolVersionQueryUnitTest)
{
    this->scenario_ValidProtocolVersion(1);
}

BOOST_FIXTURE_TEST_CASE(AccessoryModeProtocolVersionQuery_ProtcolVersion2, AccessoryModeProtocolVersionQueryUnitTest)
{
    this->scenario_ValidProtocolVersion(2);
}

BOOST_FIXTURE_TEST_CASE(AccessoryModeProtocolVersionQuery_InvalidProtocolVersion, AccessoryModeProtocolVersionQueryUnitTest)
{
    const uint16_t protocolVersion = 3;

    common::DataBuffer buffer;
    IUSBEndpoint::Promise::Pointer usbEndpointPromise;
    EXPECT_CALL(*usbEndpointMock_, controlTransfer(_, _, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<2>(&usbEndpointPromise)));
    EXPECT_CALL(usbWrapperMock_, fillControlSetup(NotNull(), LIBUSB_ENDPOINT_IN | USB_TYPE_VENDOR, ACC_REQ_GET_PROTOCOL, 0, 0, sizeof(protocolVersion)));

    AccessoryModeProtocolVersionQuery::Pointer query(std::make_shared<AccessoryModeProtocolVersionQuery>(ioService_, usbWrapperMock_, usbEndpointMock_));
    query->start(std::move(promise_));
    ioService_.run();
    ioService_.reset();

    reinterpret_cast<uint16_t&>(buffer.data[8]) = protocolVersion;
    usbEndpointPromise->resolve(buffer.size);

    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_AOAP_PROTOCOL_VERSION)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(AccessoryModeProtocolVersionQuery_TransferError, AccessoryModeProtocolVersionQueryUnitTest)
{
    const uint16_t protocolVersion = 1;

    common::DataBuffer buffer;
    IUSBEndpoint::Promise::Pointer usbEndpointPromise;
    EXPECT_CALL(*usbEndpointMock_, controlTransfer(_, _, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<2>(&usbEndpointPromise)));
    EXPECT_CALL(usbWrapperMock_, fillControlSetup(NotNull(), LIBUSB_ENDPOINT_IN | USB_TYPE_VENDOR, ACC_REQ_GET_PROTOCOL, 0, 0, sizeof(protocolVersion)));

    AccessoryModeProtocolVersionQuery::Pointer query(std::make_shared<AccessoryModeProtocolVersionQuery>(ioService_, usbWrapperMock_, usbEndpointMock_));
    query->start(std::move(promise_));
    ioService_.run();
    ioService_.reset();

    reinterpret_cast<uint16_t&>(buffer.data[8]) = protocolVersion;
    const error::Error transferError(error::ErrorCode::USB_TRANSFER, LIBUSB_TRANSFER_ERROR);
    usbEndpointPromise->reject(transferError);

    EXPECT_CALL(promiseHandlerMock_, onReject(transferError));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(AccessoryModeProtocolVersionQuery_RejectWhenInProgress, AccessoryModeProtocolVersionQueryUnitTest)
{
    const uint16_t protocolVersion = 1;

    common::DataBuffer buffer;
    IUSBEndpoint::Promise::Pointer usbEndpointPromise;
    EXPECT_CALL(*usbEndpointMock_, controlTransfer(_, _, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<2>(&usbEndpointPromise)));
    EXPECT_CALL(usbWrapperMock_, fillControlSetup(NotNull(), LIBUSB_ENDPOINT_IN | USB_TYPE_VENDOR, ACC_REQ_GET_PROTOCOL, 0, 0, sizeof(protocolVersion)));

    AccessoryModeProtocolVersionQuery::Pointer query(std::make_shared<AccessoryModeProtocolVersionQuery>(ioService_, usbWrapperMock_, usbEndpointMock_));
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
