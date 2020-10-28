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
#include <f1x/aasdk/USB/UT/USBHubPromiseHandler.mock.hpp>
#include <f1x/aasdk/USB/USBHub.hpp>

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
using ::testing::SetArgReferee;

class USBHubUnitTest
{
protected:
    USBHubUnitTest()
        : queryChain_(&queryChainMock_, [](auto*) {})
        , device_(reinterpret_cast<libusb_device*>(-1))
        , deviceHandle_(reinterpret_cast<libusb_device_handle*>(&dummyDeviceHandle_), [](auto*) {})
        , promise_(IUSBHub::Promise::defer(ioService_))
        , rawHotplugCallbacHandle_(-1)
        , hotplugCallbackHandle_(&rawHotplugCallbacHandle_, [](auto*) {})
    {
        promise_->then(std::bind(&USBHubPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                      std::bind(&USBHubPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));
    }

    boost::asio::io_service ioService_;
    USBWrapperMock usbWrapperMock_;
    AccessoryModeQueryChainFactoryMock queryChainFactoryMock_;
    AccessoryModeQueryChainMock queryChainMock_;
    IAccessoryModeQueryChain::Pointer queryChain_;
    libusb_device* device_;
    USBWrapperMock::DummyDeviceHandle dummyDeviceHandle_;
    DeviceHandle deviceHandle_;
    USBHubPromiseHandlerMock promiseHandlerMock_;
    IUSBHub::Promise::Pointer promise_;
    libusb_hotplug_callback_handle rawHotplugCallbacHandle_;
    HotplugCallbackHandle hotplugCallbackHandle_;
    libusb_hotplug_callback_fn hotplugCallback_;

    static constexpr uint16_t cGoogleVendorId = 0x18D1;
    static constexpr uint16_t cAOAPId = 0x2D00;
    static constexpr uint16_t cAOAPWithAdbId = 0x2D01;
};

BOOST_FIXTURE_TEST_CASE(USBHub_QueryDevice, USBHubUnitTest)
{
    void* userData = nullptr;
    EXPECT_CALL(usbWrapperMock_, hotplugRegisterCallback(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_NO_FLAGS,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&hotplugCallback_), SaveArg<6>(&userData), Return(hotplugCallbackHandle_)));

    USBHub::Pointer usbHub(std::make_shared<USBHub>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    usbHub->start(std::move(promise_));

    ioService_.run();
    ioService_.reset();

    libusb_device_descriptor connectedDeviceDescriptor = {0};
    connectedDeviceDescriptor.idVendor = 123;
    connectedDeviceDescriptor.idProduct = 456;

    EXPECT_CALL(usbWrapperMock_, getDeviceDescriptor(device_, _)).WillOnce(testing::DoAll(SetArgReferee<1>(connectedDeviceDescriptor), Return(0)));
    EXPECT_CALL(usbWrapperMock_, open(device_, _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle_), Return(0)));
    EXPECT_CALL(queryChainFactoryMock_, create()).WillOnce(Return(queryChain_));

    IAccessoryModeQueryChain::Promise::Pointer queryChainPromise;
    EXPECT_CALL(queryChainMock_, start(deviceHandle_, _)).WillOnce(SaveArg<1>(&queryChainPromise));

    hotplugCallback_(nullptr, device_, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, userData);
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::OPERATION_ABORTED)));
    queryChainPromise->resolve(deviceHandle_);
    usbHub->cancel();
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBHub_AOAPDeviceConnected, USBHubUnitTest)
{
    void* userData = nullptr;
    EXPECT_CALL(usbWrapperMock_, hotplugRegisterCallback(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_NO_FLAGS,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&hotplugCallback_), SaveArg<6>(&userData), Return(hotplugCallbackHandle_)));

    USBHub::Pointer usbHub(std::make_shared<USBHub>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    usbHub->start(std::move(promise_));

    ioService_.run();
    ioService_.reset();

    libusb_device_descriptor connectedDeviceDescriptor = {0};
    connectedDeviceDescriptor.idVendor = cGoogleVendorId;
    connectedDeviceDescriptor.idProduct = cAOAPWithAdbId;

    EXPECT_CALL(usbWrapperMock_, getDeviceDescriptor(device_, _)).WillOnce(testing::DoAll(SetArgReferee<1>(connectedDeviceDescriptor), Return(0)));
    EXPECT_CALL(usbWrapperMock_, open(device_, _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle_), Return(0)));
    EXPECT_CALL(promiseHandlerMock_, onResolve(deviceHandle_));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);

    hotplugCallback_(nullptr, device_, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, userData);
    ioService_.run();
    ioService_.reset();

    usbHub->cancel();
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBHub_GetDeviceDescriptorFailed, USBHubUnitTest)
{
    void* userData = nullptr;
    EXPECT_CALL(usbWrapperMock_, hotplugRegisterCallback(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_NO_FLAGS,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&hotplugCallback_), SaveArg<6>(&userData), Return(hotplugCallbackHandle_)));

    USBHub::Pointer usbHub(std::make_shared<USBHub>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    usbHub->start(std::move(promise_));

    ioService_.run();
    ioService_.reset();

    libusb_device_descriptor connectedDeviceDescriptor = {0};
    EXPECT_CALL(usbWrapperMock_, getDeviceDescriptor(device_, _)).WillOnce(testing::DoAll(SetArgReferee<1>(connectedDeviceDescriptor), Return(1)));

    hotplugCallback_(nullptr, device_, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, userData);
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::OPERATION_ABORTED)));
    usbHub->cancel();
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBHub_OpenDeviceFailed, USBHubUnitTest)
{
    void* userData = nullptr;
    EXPECT_CALL(usbWrapperMock_, hotplugRegisterCallback(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_NO_FLAGS,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&hotplugCallback_), SaveArg<6>(&userData), Return(hotplugCallbackHandle_)));

    USBHub::Pointer usbHub(std::make_shared<USBHub>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    usbHub->start(std::move(promise_));

    ioService_.run();
    ioService_.reset();

    libusb_device_descriptor connectedDeviceDescriptor = {0};
    EXPECT_CALL(usbWrapperMock_, getDeviceDescriptor(device_, _)).WillOnce(testing::DoAll(SetArgReferee<1>(connectedDeviceDescriptor), Return(0)));
    EXPECT_CALL(usbWrapperMock_, open(device_, _)).WillOnce(testing::DoAll(SetArgReferee<1>(deviceHandle_), Return(1)));

    hotplugCallback_(nullptr, device_, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, userData);
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::OPERATION_ABORTED)));
    usbHub->cancel();
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(USBHub_CancelAllQueryChains, USBHubUnitTest)
{
    void* userData = nullptr;
    EXPECT_CALL(usbWrapperMock_, hotplugRegisterCallback(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_NO_FLAGS,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                                         LIBUSB_HOTPLUG_MATCH_ANY, _, _))
            .WillOnce(testing::DoAll(SaveArg<5>(&hotplugCallback_), SaveArg<6>(&userData), Return(hotplugCallbackHandle_)));

    USBHub::Pointer usbHub(std::make_shared<USBHub>(usbWrapperMock_, ioService_, queryChainFactoryMock_));
    usbHub->start(std::move(promise_));

    ioService_.run();
    ioService_.reset();

    libusb_device_descriptor connectedDeviceDescriptor = {0};
    connectedDeviceDescriptor.idVendor = 123;
    connectedDeviceDescriptor.idProduct = 456;

    EXPECT_CALL(usbWrapperMock_, getDeviceDescriptor(device_, _)).Times(2).WillRepeatedly(testing::DoAll(SetArgReferee<1>(connectedDeviceDescriptor), Return(0)));
    EXPECT_CALL(usbWrapperMock_, open(device_, _)).Times(2).WillRepeatedly(testing::DoAll(SetArgReferee<1>(deviceHandle_), Return(0)));
    EXPECT_CALL(queryChainFactoryMock_, create()).Times(2).WillRepeatedly(Return(queryChain_));

    IAccessoryModeQueryChain::Promise::Pointer queryChainPromise[2];
    EXPECT_CALL(queryChainMock_, start(deviceHandle_, _)).WillOnce(SaveArg<1>(&queryChainPromise[0])).WillOnce(SaveArg<1>(&queryChainPromise[1]));

    hotplugCallback_(nullptr, device_, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, userData);
    ioService_.run();
    ioService_.reset();

    hotplugCallback_(nullptr, device_, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, userData);
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(queryChainMock_, cancel()).Times(2);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::OPERATION_ABORTED)));
    usbHub->cancel();
    ioService_.run();
    ioService_.reset();

    queryChainPromise[0]->reject(error::Error(error::ErrorCode::OPERATION_ABORTED));
    queryChainPromise[1]->reject(error::Error(error::ErrorCode::OPERATION_ABORTED));
    ioService_.run();
}

}
}
}
}
