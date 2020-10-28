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
#include <f1x/aasdk/TCP/UT/TCPWrapper.mock.hpp>
#include <f1x/aasdk/TCP/UT/TCPEndpointPromiseHandler.mock.hpp>
#include <f1x/aasdk/TCP/TCPEndpoint.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{
namespace ut
{

using ::testing::_;
using ::testing::SaveArg;

class TCPEndpointUnitTest
{
protected:
    TCPEndpointUnitTest()
        : socket_(std::make_shared<boost::asio::ip::tcp::socket>(ioService_))
        , promise_(ITCPEndpoint::Promise::defer(ioService_))
    {
        promise_->then(std::bind(&TCPEndpointPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                       std::bind(&TCPEndpointPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));
    }

    TCPWrapperMock tcpWrapperMock_;
    TCPEndpointPromiseHandlerMock promiseHandlerMock_;
    boost::asio::io_service ioService_;
    ITCPEndpoint::SocketPointer socket_;
    ITCPEndpoint::Promise::Pointer promise_;
};

BOOST_FIXTURE_TEST_CASE(TCPEndpoint_Receive, TCPEndpointUnitTest)
{
    auto tcpEndpoint = std::make_shared<TCPEndpoint>(tcpWrapperMock_, std::move(socket_));

    common::DataBuffer buffer;
    ITCPWrapper::Handler handler;
    EXPECT_CALL(tcpWrapperMock_, asyncRead(_, _, _)).WillOnce(testing::DoAll(SaveArg<1>(&buffer), SaveArg<2>(&handler)));

    common::Data actualData(100, 0);
    tcpEndpoint->receive(common::DataBuffer(actualData), std::move(promise_));

    const common::Data expectedData(actualData.size(), 0x5F);
    std::copy(expectedData.begin(), expectedData.end(), buffer.data);

    EXPECT_CALL(promiseHandlerMock_, onResolve(expectedData.size()));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    handler(boost::system::error_code(), expectedData.size());

    ioService_.run();

    BOOST_CHECK_EQUAL_COLLECTIONS(actualData.begin(), actualData.end(), expectedData.begin(), expectedData.end());
}

BOOST_FIXTURE_TEST_CASE(TCPEndpoint_ReceiveError, TCPEndpointUnitTest)
{
    auto tcpEndpoint = std::make_shared<TCPEndpoint>(tcpWrapperMock_, std::move(socket_));

    common::DataBuffer buffer;
    ITCPWrapper::Handler handler;
    EXPECT_CALL(tcpWrapperMock_, asyncRead(_, _, _)).WillOnce(testing::DoAll(SaveArg<1>(&buffer), SaveArg<2>(&handler)));

    common::Data actualData(100, 0);
    tcpEndpoint->receive(common::DataBuffer(actualData), std::move(promise_));

    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::TCP_TRANSFER, boost::asio::error::bad_descriptor)));
    handler(boost::asio::error::bad_descriptor, 0);

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(TCPEndpoint_Send, TCPEndpointUnitTest)
{
    auto tcpEndpoint = std::make_shared<TCPEndpoint>(tcpWrapperMock_, std::move(socket_));

    common::Data actualData(100, 0);
    common::DataConstBuffer buffer(actualData);
    ITCPWrapper::Handler handler;
    EXPECT_CALL(tcpWrapperMock_, asyncWrite(_, buffer, _)).WillOnce(SaveArg<2>(&handler));
    tcpEndpoint->send(common::DataConstBuffer(actualData), std::move(promise_));

    EXPECT_CALL(promiseHandlerMock_, onResolve(actualData.size()));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    handler(boost::system::error_code(), actualData.size());

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(TCPEndpoint_SendError, TCPEndpointUnitTest)
{
    auto tcpEndpoint = std::make_shared<TCPEndpoint>(tcpWrapperMock_, std::move(socket_));

    common::Data actualData(100, 0);
    common::DataConstBuffer buffer(actualData);
    ITCPWrapper::Handler handler;
    EXPECT_CALL(tcpWrapperMock_, asyncWrite(_, buffer, _)).WillOnce(SaveArg<2>(&handler));
    tcpEndpoint->send(common::DataConstBuffer(actualData), std::move(promise_));

    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::OPERATION_ABORTED)));
    handler(boost::asio::error::operation_aborted, 0);

    ioService_.run();
}

}
}
}
}
