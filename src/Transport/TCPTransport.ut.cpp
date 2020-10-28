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
#include <f1x/aasdk/TCP/UT/TCPEndpoint.mock.hpp>
#include <f1x/aasdk/Transport/UT/TransportReceivePromiseHandler.mock.hpp>
#include <f1x/aasdk/Transport/UT/TransportSendPromiseHandler.mock.hpp>
#include <f1x/aasdk/Transport/TCPTransport.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{
namespace ut
{

using ::testing::ReturnRef;
using ::testing::SaveArg;
using ::testing::_;
using ::testing::AtLeast;

class TCPTransportUnitTest
{
protected:
    TCPTransportUnitTest()
        : receivePromise_(ITransport::ReceivePromise::defer(ioService_))
        , sendPromise_(ITransport::SendPromise::defer(ioService_))
        , tcpEndpoint_(&tcpEndpointMock_, [](auto*) {})
    {
        receivePromise_->then(std::bind(&TransportReceivePromiseHandlerMock::onResolve, &receivePromiseHandlerMock_, std::placeholders::_1),
                              std::bind(&TransportReceivePromiseHandlerMock::onReject, &receivePromiseHandlerMock_, std::placeholders::_1));

        sendPromise_->then(std::bind(&TransportSendPromiseHandlerMock::onResolve, &sendPromiseHandlerMock_),
                           std::bind(&TransportSendPromiseHandlerMock::onReject, &sendPromiseHandlerMock_, std::placeholders::_1));
    }

    boost::asio::io_service ioService_;
    tcp::ut::TCPEndpointMock tcpEndpointMock_;
    TransportReceivePromiseHandlerMock receivePromiseHandlerMock_;
    ITransport::ReceivePromise::Pointer receivePromise_;
    TransportSendPromiseHandlerMock sendPromiseHandlerMock_;
    ITransport::SendPromise::Pointer sendPromise_;
    tcp::ITCPEndpoint::Pointer tcpEndpoint_;
};

BOOST_FIXTURE_TEST_CASE(TCPTransport_ReceiveAtOnce, TCPTransportUnitTest)
{
    const size_t receiveSize = 100;

    tcp::ITCPEndpoint::Promise::Pointer tcpEndpointPromise;
    common::DataBuffer dataBuffer;
    EXPECT_CALL(tcpEndpointMock_, receive(_, _)).WillOnce(testing::DoAll(SaveArg<0>(&dataBuffer), SaveArg<1>(&tcpEndpointPromise)));

    auto transport(std::make_shared<TCPTransport>(ioService_, tcpEndpoint_));
    transport->receive(receiveSize, std::move(receivePromise_));
    ioService_.run();
    ioService_.reset();

    BOOST_TEST(dataBuffer.size >= receiveSize);
    common::Data expectedData(receiveSize, 0x5E);
    std::copy(expectedData.begin(), expectedData.end(), dataBuffer.data);

    EXPECT_CALL(receivePromiseHandlerMock_, onResolve(expectedData)).Times(1);
    EXPECT_CALL(receivePromiseHandlerMock_, onReject(_)).Times(0);
    tcpEndpointPromise->resolve(receiveSize);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(TCPTransport_ReceiveInPieces, TCPTransportUnitTest)
{
    const size_t stepsCount = 100;
    const size_t receiveSize = 1000 * stepsCount;
    const size_t stepSize = receiveSize / stepsCount;

    auto transport(std::make_shared<TCPTransport>(ioService_, tcpEndpoint_));
    transport->receive(receiveSize, std::move(receivePromise_));

    tcp::ITCPEndpoint::Promise::Pointer tcpEndpointPromise;
    common::DataBuffer dataBuffer;
    EXPECT_CALL(tcpEndpointMock_, receive(_, _)).Times(AtLeast(stepsCount))
            .WillRepeatedly(testing::DoAll(SaveArg<0>(&dataBuffer), SaveArg<1>(&tcpEndpointPromise)));

    common::Data expectedData(receiveSize, 0x5E);
    EXPECT_CALL(receivePromiseHandlerMock_, onResolve(expectedData)).Times(1);
    EXPECT_CALL(receivePromiseHandlerMock_, onReject(_)).Times(0);

    for(size_t i = 0; i < stepsCount; ++i)
    {
        ioService_.run();
        ioService_.reset();

        BOOST_TEST(dataBuffer.size >= stepSize);

        std::fill(dataBuffer.data, dataBuffer.data + stepSize, 0x5E);
        tcpEndpointPromise->resolve(stepSize);
        ioService_.run();
    }
}

BOOST_FIXTURE_TEST_CASE(TCPTransport_OnlyOneReceiveAtATime, TCPTransportUnitTest)
{
    const size_t receiveSize = 200;
    const size_t stepSize = receiveSize / 2;

    tcp::ITCPEndpoint::Promise::Pointer tcpEndpointPromise;
    common::DataBuffer dataBuffer;
    EXPECT_CALL(tcpEndpointMock_, receive(_, _)).WillOnce(testing::DoAll(SaveArg<0>(&dataBuffer), SaveArg<1>(&tcpEndpointPromise)));

    auto transport(std::make_shared<TCPTransport>(ioService_, tcpEndpoint_));
    transport->receive(stepSize, std::move(receivePromise_));
    ioService_.run();
    ioService_.reset();

    BOOST_TEST(dataBuffer.size >= receiveSize);
    std::fill(dataBuffer.data, dataBuffer.data + stepSize, 0x5E);
    std::fill(dataBuffer.data + stepSize, dataBuffer.data + receiveSize, 0x5F);

    auto secondPromise = ITransport::ReceivePromise::defer(ioService_);
    TransportReceivePromiseHandlerMock secondPromiseHandlerMock;
    secondPromise->then(std::bind(&TransportReceivePromiseHandlerMock::onResolve, &secondPromiseHandlerMock, std::placeholders::_1),
                       std::bind(&TransportReceivePromiseHandlerMock::onReject, &secondPromiseHandlerMock, std::placeholders::_1));

    transport->receive(stepSize, std::move(secondPromise));
    ioService_.run();
    ioService_.reset();

    common::Data expectedData(stepSize, 0x5E);
    EXPECT_CALL(receivePromiseHandlerMock_, onResolve(expectedData)).Times(1);
    EXPECT_CALL(receivePromiseHandlerMock_, onReject(_)).Times(0);

    common::Data secondExpectedData(stepSize, 0x5F);
    EXPECT_CALL(secondPromiseHandlerMock, onResolve(secondExpectedData)).Times(1);
    EXPECT_CALL(secondPromiseHandlerMock, onReject(_)).Times(0);

    tcpEndpointPromise->resolve(receiveSize);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(TCPTransport_ReceiveError, TCPTransportUnitTest)
{
    tcp::ITCPEndpoint::Promise::Pointer tcpEndpointPromise;
    EXPECT_CALL(tcpEndpointMock_, receive(_, _)).WillOnce(SaveArg<1>(&tcpEndpointPromise));

    auto transport(std::make_shared<TCPTransport>(ioService_, tcpEndpoint_));
    transport->receive(1000, std::move(receivePromise_));

    auto secondPromise = ITransport::ReceivePromise::defer(ioService_);
    secondPromise->then(std::bind(&TransportReceivePromiseHandlerMock::onResolve, &receivePromiseHandlerMock_, std::placeholders::_1),
                       std::bind(&TransportReceivePromiseHandlerMock::onReject, &receivePromiseHandlerMock_, std::placeholders::_1));

    transport->receive(1000, std::move(secondPromise));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::TCP_TRANSFER, 11);
    EXPECT_CALL(receivePromiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(receivePromiseHandlerMock_, onReject(e)).Times(2);

    tcpEndpointPromise->reject(e);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(TCPTransport_Send, TCPTransportUnitTest)
{
    tcp::ITCPEndpoint::Promise::Pointer tcpEndpointPromise;
    common::DataConstBuffer buffer;
    EXPECT_CALL(tcpEndpointMock_, send(_, _)).WillOnce(testing::DoAll(SaveArg<0>(&buffer), SaveArg<1>(&tcpEndpointPromise)));

    auto transport(std::make_shared<TCPTransport>(ioService_, tcpEndpoint_));
    const common::Data expectedData(1000, 0x5E);
    transport->send(expectedData, std::move(sendPromise_));
    ioService_.run();
    ioService_.reset();

    common::Data actualData(buffer.cdata, buffer.cdata + buffer.size);
    BOOST_CHECK_EQUAL_COLLECTIONS(actualData.begin(), actualData.end(), expectedData.begin(), expectedData.end());

    EXPECT_CALL(sendPromiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve());
    tcpEndpointPromise->resolve(expectedData.size());
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(TCPTransport_OnlyOneSendAtATime, TCPTransportUnitTest)
{
    tcp::ITCPEndpoint::Promise::Pointer tcpEndpointPromise;
    common::DataConstBuffer buffer;
    EXPECT_CALL(tcpEndpointMock_, send(_, _)).Times(2).WillRepeatedly(testing::DoAll(SaveArg<0>(&buffer), SaveArg<1>(&tcpEndpointPromise)));

    auto transport(std::make_shared<TCPTransport>(ioService_, tcpEndpoint_));
    const common::Data expectedData1(1000, 0x5E);
    transport->send(expectedData1, std::move(sendPromise_));
    ioService_.run();
    ioService_.reset();

    const common::Data expectedData2(3000, 0x5F);

    auto secondSendPromise = ITransport::SendPromise::defer(ioService_);
    TransportSendPromiseHandlerMock secondSendPromiseHandlerMock;
    secondSendPromise->then(std::bind(&TransportSendPromiseHandlerMock::onResolve, &secondSendPromiseHandlerMock),
                           std::bind(&TransportSendPromiseHandlerMock::onReject, &secondSendPromiseHandlerMock, std::placeholders::_1));

    transport->send(expectedData2, std::move(secondSendPromise));
    ioService_.run();
    ioService_.reset();

    common::Data actualData1(buffer.cdata, buffer.cdata + buffer.size);
    BOOST_CHECK_EQUAL_COLLECTIONS(actualData1.begin(), actualData1.end(), expectedData1.begin(), expectedData1.end());

    EXPECT_CALL(sendPromiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve());
    tcpEndpointPromise->resolve(expectedData1.size());
    ioService_.run();
    ioService_.reset();

    common::Data actualData2(buffer.cdata, buffer.cdata + buffer.size);
    BOOST_CHECK_EQUAL_COLLECTIONS(actualData2.begin(), actualData2.end(), expectedData2.begin(), expectedData2.end());

    EXPECT_CALL(secondSendPromiseHandlerMock, onReject(_)).Times(0);
    EXPECT_CALL(secondSendPromiseHandlerMock, onResolve());
    tcpEndpointPromise->resolve(expectedData2.size());
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(TCPTransport_SendError, TCPTransportUnitTest)
{
    tcp::ITCPEndpoint::Promise::Pointer tcpEndpointPromise;
    EXPECT_CALL(tcpEndpointMock_, send(_, _)).Times(2).WillRepeatedly(SaveArg<1>(&tcpEndpointPromise));

    auto transport(std::make_shared<TCPTransport>(ioService_, tcpEndpoint_));
    const common::Data expectedData1(1000, 0x5E);
    transport->send(expectedData1, std::move(sendPromise_));
    ioService_.run();
    ioService_.reset();

    auto secondSendPromise = ITransport::SendPromise::defer(ioService_);
    TransportSendPromiseHandlerMock secondSendPromiseHandlerMock;
    secondSendPromise->then(std::bind(&TransportSendPromiseHandlerMock::onResolve, &secondSendPromiseHandlerMock),
                           std::bind(&TransportSendPromiseHandlerMock::onReject, &secondSendPromiseHandlerMock, std::placeholders::_1));

    const common::Data expectedData2(3000, 0x5F);
    transport->send(expectedData2, std::move(secondSendPromise));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER, 15);
    EXPECT_CALL(sendPromiseHandlerMock_, onReject(e));
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve()).Times(0);
    tcpEndpointPromise->reject(e);
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(secondSendPromiseHandlerMock, onReject(_)).Times(0);
    EXPECT_CALL(secondSendPromiseHandlerMock, onResolve());
    tcpEndpointPromise->resolve(expectedData2.size());
    ioService_.run();
}

}
}
}
}
