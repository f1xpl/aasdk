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
#include <f1x/aasdk/USB/UT/AccessoryModeQueryChainFactory.mock.hpp>
#include <f1x/aasdk/USB/UT/AccessoryModeQueryChain.mock.hpp>
#include <f1x/aasdk/USB/UT/ConnectedAccessoriesEnumeratorPromiseHandler.mock.hpp>
#include <f1x/aasdk/USB/ConnectedAccessoriesEnumerator.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{
namespace ut
{

using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::Return;
using ::testing::SaveArg;

class ConnectedAccessoriesEnumeratorUnitTest
{
protected:
    ConnectedAccessoriesEnumeratorUnitTest()
        : queryChain_(&queryChainMock_, [](auto*) {})
        , deviceListHandle_(&deviceList_, [](auto*) {})
        , device_(reinterpret_cast<libusb_device*>(-1))
        , deviceHandle_(reinterpret_cast<libusb_device_handle*>(&dummyDeviceHandle_), [](auto*) {})
        , promise_(IConnectedAccessoriesEnumerator::Promise::defer(ioService_))
    {
        promise_->then(std::bind(&ConnectedAccessoriesEnumeratorPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                       std::bind(&ConnectedAccessoriesEnumeratorPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));
    }

    boost::asio::io_service ioService_;
    USBWrapperMock usbWrapperMock_;
    AccessoryModeQueryChainFactoryMock queryChainFactoryMock_;
    AccessoryModeQueryChainMock queryChainMock_;
    IAccessoryModeQueryChain::Pointer queryChain_;
    DeviceList deviceList_;
    DeviceListHandle deviceListHandle_;
    libusb_device* device_;
    USBWrapperMock::DummyDeviceHandle dummyDeviceHandle_;
    DeviceHandle deviceHandle_;
    ConnectedAccessoriesEnumeratorPromiseHandlerMock promiseHandlerMock_;
    IConnectedAccessoriesEnumerator::Promise::Pointer promise_;
};

BOOST_FIXTURE_TEST_CASE(ConnectedAccessoriesEnumerator_FirstDeviceIsAOAPCapables, ConnectedAccessoriesEnumeratorUnitTest)
{
    deviceList_.push_back(reinterpret_cast<libusb_device*>(1));
    EXPECT_CALL(queryChainFactoryMock_, create()).WillOnce(Return(queryChain_));
    auto connectedAccessoriesEnumerator(std::make_shared<ConnectedAccessoriesEnumerator>(usbWrapperMock_, ioService_, queryChainFactoryMock_));

    EXPECT_CALL(usbWrapperMock_, getDeviceList(_)).WillOnce(testing::DoAll(SetArgReferee<0>(deviceListHandle_), Return(0)));
    connectedAccessoriesEnumerator->enumerate(std::move(promise_));

    EXPECT_CALL(usbWrapperMock_, open(*deviceList_.begin(), _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle_), Return(0)));

    IAccessoryModeQueryChain::Promise::Pointer queryChainPromise;
    EXPECT_CALL(queryChainMock_, start(deviceHandle_, _)).WillOnce(SaveArg<1>(&queryChainPromise));
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(promiseHandlerMock_, onResolve(true));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    queryChainPromise->resolve(deviceHandle_);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(ConnectedAccessoriesEnumerator_SecondDeviceIsAOAPCapable, ConnectedAccessoriesEnumeratorUnitTest)
{
    deviceList_.push_back(reinterpret_cast<libusb_device*>(1));
    deviceList_.push_back(reinterpret_cast<libusb_device*>(2));
    auto connectedAccessoriesEnumerator(std::make_shared<ConnectedAccessoriesEnumerator>(usbWrapperMock_, ioService_, queryChainFactoryMock_));

    EXPECT_CALL(queryChainFactoryMock_, create()).Times(deviceList_.size()).WillRepeatedly(Return(queryChain_));
    EXPECT_CALL(usbWrapperMock_, getDeviceList(_)).WillOnce(testing::DoAll(SetArgReferee<0>(deviceListHandle_), Return(0)));
    connectedAccessoriesEnumerator->enumerate(std::move(promise_));

    EXPECT_CALL(usbWrapperMock_, open(*deviceList_.begin(), _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle_), Return(0)));
    IAccessoryModeQueryChain::Promise::Pointer queryChainPromise;
    EXPECT_CALL(queryChainMock_, start(deviceHandle_, _)).WillRepeatedly(SaveArg<1>(&queryChainPromise));
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);

    // open second device
    USBWrapperMock::DummyDeviceHandle dummyDeviceHandle2;
    DeviceHandle deviceHandle2(reinterpret_cast<libusb_device_handle*>(&dummyDeviceHandle2), [](auto*) {});
    EXPECT_CALL(usbWrapperMock_, open(*(++deviceList_.begin()), _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle2), Return(0)));

    IAccessoryModeQueryChain::Promise::Pointer queryChainPromise2;
    EXPECT_CALL(queryChainMock_, start(deviceHandle2, _)).WillRepeatedly(SaveArg<1>(&queryChainPromise2));

    queryChainPromise->reject(error::Error(error::ErrorCode::USB_AOAP_PROTOCOL_VERSION));
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(promiseHandlerMock_, onResolve(true));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    queryChainPromise2->resolve(deviceHandle2);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(ConnectedAccessoriesEnumerator_NoAOAPCapableDevice, ConnectedAccessoriesEnumeratorUnitTest)
{
    for(size_t i = 1; i < 1000; ++i)
    {
        deviceList_.push_back(reinterpret_cast<libusb_device*>(i));
    }

    EXPECT_CALL(usbWrapperMock_, getDeviceList(_)).WillOnce(testing::DoAll(SetArgReferee<0>(deviceListHandle_), Return(0)));
    EXPECT_CALL(queryChainFactoryMock_, create()).Times(deviceList_.size()).WillRepeatedly(Return(queryChain_));

    auto connectedAccessoriesEnumerator(std::make_shared<ConnectedAccessoriesEnumerator>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    connectedAccessoriesEnumerator->enumerate(std::move(promise_));

    EXPECT_CALL(promiseHandlerMock_, onResolve(false));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);

    for(const auto& device : deviceList_)
    {
        USBWrapperMock::DummyDeviceHandle dummyDeviceHandle;
        DeviceHandle deviceHandle(reinterpret_cast<libusb_device_handle*>(&dummyDeviceHandle), [](auto*) {});
        EXPECT_CALL(usbWrapperMock_, open(device, _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle), Return(0)));

        IAccessoryModeQueryChain::Promise::Pointer queryChainPromise;
        EXPECT_CALL(queryChainMock_, start(deviceHandle, _)).WillRepeatedly(SaveArg<1>(&queryChainPromise));

        ioService_.run();
        ioService_.reset();

        queryChainPromise->reject(error::Error(error::ErrorCode::USB_AOAP_PROTOCOL_VERSION));
    }

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(ConnectedAccessoriesEnumerator_GetDeviceListFailed, ConnectedAccessoriesEnumeratorUnitTest)
{
    EXPECT_CALL(usbWrapperMock_, getDeviceList(_)).WillOnce(testing::DoAll(SetArgReferee<0>(deviceListHandle_), Return(-1)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::USB_LIST_DEVICES)));

    auto connectedAccessoriesEnumerator(std::make_shared<ConnectedAccessoriesEnumerator>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    connectedAccessoriesEnumerator->enumerate(std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(ConnectedAccessoriesEnumerator_EmptyDevicesList, ConnectedAccessoriesEnumeratorUnitTest)
{
    EXPECT_CALL(usbWrapperMock_, getDeviceList(_)).WillOnce(testing::DoAll(SetArgReferee<0>(deviceListHandle_), Return(0)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(false));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);

    auto connectedAccessoriesEnumerator(std::make_shared<ConnectedAccessoriesEnumerator>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    connectedAccessoriesEnumerator->enumerate(std::move(promise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(ConnectedAccessoriesEnumerator_OpenDeviceFailed, ConnectedAccessoriesEnumeratorUnitTest)
{
    for(size_t i = 1; i < 1000; ++i)
    {
        deviceList_.push_back(reinterpret_cast<libusb_device*>(i));
    }

    EXPECT_CALL(usbWrapperMock_, getDeviceList(_)).WillOnce(testing::DoAll(SetArgReferee<0>(deviceListHandle_), Return(0)));

    auto connectedAccessoriesEnumerator(std::make_shared<ConnectedAccessoriesEnumerator>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    connectedAccessoriesEnumerator->enumerate(std::move(promise_));

    EXPECT_CALL(promiseHandlerMock_, onResolve(false));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);

    for(const auto& device : deviceList_)
    {
        EXPECT_CALL(usbWrapperMock_, open(device, _)).WillOnce(testing::DoAll(SetArgReferee<1>(nullptr), Return(0xFFF)));
    }

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(ConnectedAccessoriesEnumerator_CancelEnumeration, ConnectedAccessoriesEnumeratorUnitTest)
{
    deviceList_.push_back(reinterpret_cast<libusb_device*>(1));
    EXPECT_CALL(queryChainFactoryMock_, create()).WillOnce(Return(queryChain_));
    auto connectedAccessoriesEnumerator(std::make_shared<ConnectedAccessoriesEnumerator>(usbWrapperMock_, ioService_, queryChainFactoryMock_));

    EXPECT_CALL(usbWrapperMock_, getDeviceList(_)).WillOnce(testing::DoAll(SetArgReferee<0>(deviceListHandle_), Return(0)));
    connectedAccessoriesEnumerator->enumerate(std::move(promise_));

    EXPECT_CALL(usbWrapperMock_, open(*deviceList_.begin(), _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle_), Return(0)));

    IAccessoryModeQueryChain::Promise::Pointer queryChainPromise;
    EXPECT_CALL(queryChainMock_, start(deviceHandle_, _)).WillOnce(SaveArg<1>(&queryChainPromise));
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(queryChainMock_, cancel());
    connectedAccessoriesEnumerator->cancel();

    error::Error e(error::ErrorCode::OPERATION_ABORTED);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    queryChainPromise->reject(e);
    ioService_.run();
}

}
}
}
}
