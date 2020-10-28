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
#include <f1x/aasdk/USB/UT/USBEndpointPromiseHandler.mock.hpp>
#include <f1x/aasdk/Error/Error.hpp>
#include <f1x/aasdk/USB/USBEndpoint.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{
namespace ut
{

using ::testing::_;
using ::testing::Return;
using ::testing::SaveArg;

class USBEndpointUnitTest
{
protected:
    USBEndpointUnitTest()
      : deviceHandle_(reinterpret_cast<libusb_device_handle*>(&dummyDeviceHandle_), [](auto*) {})
      , promise_(IUSBEndpoint::Promise::defer(ioService_))
    {
        promise_->then(std::bind(&USBEndpointPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                      std::bind(&USBEndpointPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));
    }

    USBWrapperMock usbWrapperMock_;
    boost::asio::io_service ioService_;
    USBWrapperMock::DummyDeviceHandle dummyDeviceHandle_;
    DeviceHandle deviceHandle_;
    USBEndpointPromiseHandlerMock promiseHandlerMock_;
    IUSBEndpoint::Promise::Pointer promise_;
};

BOOST_FIXTURE_TEST_CASE(USBEndpoint_ControlTransferForNonControlEndpoint, USBEndpointUnitTest)
{
    common::Data data(10, 0);
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_, 0x01));

    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_INVALID_TRANSFER_METHOD)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    usbEndpoint->controlTransfer(common::DataBuffer(data), 0, std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_BulkTransferForControlEndpoint, USBEndpointUnitTest)
{
    common::Data data(10, 0);
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_));

    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_INVALID_TRANSFER_METHOD)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    usbEndpoint->bulkTransfer(common::DataBuffer(data), 0, std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_InterruptTransferForControlEndpoint, USBEndpointUnitTest)
{
    common::Data data(10, 0);
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_));

    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_INVALID_TRANSFER_METHOD)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    usbEndpoint->interruptTransfer(common::DataBuffer(data), 0, std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_ControlTransferAllocationFailed, USBEndpointUnitTest)
{
    common::Data data(10, 0);
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_));

    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillOnce(Return(nullptr));
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_TRANSFER_ALLOCATION)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    usbEndpoint->controlTransfer(common::DataBuffer(data), 0, std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_BulkTransferAllocationFailed, USBEndpointUnitTest)
{
    common::Data data(10, 0);
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_, 0x01));

    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillOnce(Return(nullptr));
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_TRANSFER_ALLOCATION)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    usbEndpoint->bulkTransfer(common::DataBuffer(data), 0, std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_InterruptTransferAllocationFailed, USBEndpointUnitTest)
{
    common::Data data(10, 0);
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_, 0x01));

    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillOnce(Return(nullptr));
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_TRANSFER_ALLOCATION)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    usbEndpoint->interruptTransfer(common::DataBuffer(data), 0, std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_BulkTransfer, USBEndpointUnitTest)
{
    const uint8_t endpointAddress = 0x55;
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_, endpointAddress));

    libusb_transfer transfer;
    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillOnce(Return(&transfer));

    libusb_transfer_cb_fn transferCallback;
    common::Data data(1000, 0);
    common::DataBuffer buffer(data);
    EXPECT_CALL(usbWrapperMock_, fillBulkTransfer(&transfer, _, endpointAddress, buffer.data, buffer.size, _, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&transferCallback), SaveArg<6>(&transfer.user_data)));
    EXPECT_CALL(usbWrapperMock_, submitTransfer(&transfer));

    usbEndpoint->bulkTransfer(common::DataBuffer(data), 0, std::move(promise_));
    ioService_.run();
    ioService_.reset();

    transfer.actual_length = buffer.size;
    transfer.status = LIBUSB_TRANSFER_COMPLETED;
    transferCallback(&transfer);

    EXPECT_CALL(usbWrapperMock_, freeTransfer(&transfer));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onResolve(buffer.size));
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_MultipleBulkTransfers, USBEndpointUnitTest)
{
    const uint8_t endpointAddress = 0x55;
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_, endpointAddress));

    libusb_transfer transfer;
    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillRepeatedly(Return(&transfer));

    const size_t attemptsCount = 1000;

    libusb_transfer_cb_fn transferCallback;

    EXPECT_CALL(usbWrapperMock_, submitTransfer(&transfer)).Times(attemptsCount);
    EXPECT_CALL(usbWrapperMock_, freeTransfer(&transfer)).Times(attemptsCount);
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);

    for(size_t i = 0; i < attemptsCount; ++i)
    {
        common::Data data(10000 + attemptsCount, 0);
        common::DataBuffer buffer(data);
        EXPECT_CALL(usbWrapperMock_, fillBulkTransfer(&transfer, _, endpointAddress, buffer.data, buffer.size, _, _, _))
                .WillOnce(testing::DoAll(SaveArg<5>(&transferCallback), SaveArg<6>(&transfer.user_data)));
        EXPECT_CALL(promiseHandlerMock_, onResolve(buffer.size)).Times(1);

        transfer.actual_length = 0;
        transfer.status = LIBUSB_TRANSFER_ERROR;

        auto promise = IUSBEndpoint::Promise::defer(ioService_);
        promise->then(std::bind(&USBEndpointPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                      std::bind(&USBEndpointPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));

        usbEndpoint->bulkTransfer(common::DataBuffer(data), 0, std::move(promise));
        ioService_.run();
        ioService_.reset();

        transfer.actual_length = buffer.size;
        transfer.status = LIBUSB_TRANSFER_COMPLETED;
        transferCallback(&transfer);
        ioService_.run();
        ioService_.reset();
    }
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_ControlTransfer, USBEndpointUnitTest)
{
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_));

    libusb_transfer transfer;
    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillOnce(Return(&transfer));

    libusb_transfer_cb_fn transferCallback;
    common::Data data(100, 0);
    common::DataBuffer buffer(data);
    EXPECT_CALL(usbWrapperMock_, fillControlTransfer(&transfer, _, buffer.data, _, _, _))
            .WillOnce(testing::DoAll(SaveArg<3>(&transferCallback), SaveArg<4>(&transfer.user_data)));
    EXPECT_CALL(usbWrapperMock_, submitTransfer(&transfer));

    usbEndpoint->controlTransfer(common::DataBuffer(data), 0, std::move(promise_));
    ioService_.run();
    ioService_.reset();

    transfer.actual_length = buffer.size;
    transfer.status = LIBUSB_TRANSFER_COMPLETED;
    transferCallback(&transfer);

    EXPECT_CALL(usbWrapperMock_, freeTransfer(&transfer));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onResolve(buffer.size));
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_InterruptTransfer, USBEndpointUnitTest)
{
    const uint8_t endpointAddress = 0x35;
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_, endpointAddress));

    libusb_transfer transfer;
    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillOnce(Return(&transfer));

    libusb_transfer_cb_fn transferCallback;
    common::Data data(150, 0);
    common::DataBuffer buffer(data);
    EXPECT_CALL(usbWrapperMock_, fillInterruptTransfer(&transfer, _, endpointAddress, buffer.data, buffer.size, _, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&transferCallback), SaveArg<6>(&transfer.user_data)));
    EXPECT_CALL(usbWrapperMock_, submitTransfer(&transfer));

    usbEndpoint->interruptTransfer(common::DataBuffer(data), 0, std::move(promise_));
    ioService_.run();
    ioService_.reset();

    transfer.actual_length = buffer.size;
    transfer.status = LIBUSB_TRANSFER_COMPLETED;
    transferCallback(&transfer);

    EXPECT_CALL(usbWrapperMock_, freeTransfer(&transfer));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onResolve(buffer.size));
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBEndpoint_BulkTransferFailed, USBEndpointUnitTest)
{
    const uint8_t endpointAddress = 0x55;
    USBEndpoint::Pointer usbEndpoint(std::make_shared<USBEndpoint>(usbWrapperMock_, ioService_, deviceHandle_, endpointAddress));

    libusb_transfer transfer;
    EXPECT_CALL(usbWrapperMock_, allocTransfer(0)).WillOnce(Return(&transfer));

    libusb_transfer_cb_fn transferCallback;
    common::Data data(10, 0);
    common::DataBuffer buffer(data);
    EXPECT_CALL(usbWrapperMock_, fillBulkTransfer(&transfer, _, endpointAddress, buffer.data, buffer.size, _, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&transferCallback), SaveArg<6>(&transfer.user_data)));
    EXPECT_CALL(usbWrapperMock_, submitTransfer(&transfer));

    usbEndpoint->bulkTransfer(common::DataBuffer(data), 0, std::move(promise_));
    ioService_.run();
    ioService_.reset();

    transfer.actual_length = buffer.size;
    transfer.status = LIBUSB_TRANSFER_CANCELLED;
    transferCallback(&transfer);

    EXPECT_CALL(usbWrapperMock_, freeTransfer(&transfer));
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::OPERATION_ABORTED))).Times(1);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    ioService_.run();
}

}
}
}
}
