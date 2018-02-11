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

#include <list>
#include <queue>
#include <boost/asio.hpp>
#include <f1x/aasdk/Transport/ITransport.hpp>
#include <f1x/aasdk/Transport/USBDataSink.hpp>
#include <f1x/aasdk/USB/IAOAPDevice.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{

class USBTransport: public ITransport, public std::enable_shared_from_this<USBTransport>, boost::noncopyable
{
public:
    USBTransport(boost::asio::io_service& ioService, usb::IAOAPDevice::Pointer aoapDevice);

    void receive(size_t size, ReceivePromise::Pointer promise) override;
    void send(common::Data data, SendPromise::Pointer promise) override;
    void stop() override;

private:
    typedef std::list<std::pair<size_t, ReceivePromise::Pointer>> InTransferQueue;
    typedef std::list<std::pair<common::Data, SendPromise::Pointer>> OutTransferQueue;

    using std::enable_shared_from_this<USBTransport>::shared_from_this;
    void receiveHandler(size_t bytesTransferred);
    void distributeReceivedData();
    void rejectReceivePromises(const error::Error& e);

    void doSend(OutTransferQueue::iterator queueElementIter, common::Data::size_type offset);
    void sendHandler(OutTransferQueue::iterator queueElementIter, common::Data::size_type offset, size_t bytesTransferred);

    usb::IAOAPDevice::Pointer aoapDevice_;
    USBDataSink usbReceivedDataSink_;

    boost::asio::io_service::strand receiveStrand_;
    InTransferQueue receiveQueue_;

    boost::asio::io_service::strand sendStrand_;
    OutTransferQueue sendQueue_;

    static constexpr uint32_t cSendTimeoutMs = 10000;
    static constexpr uint32_t cReceiveTimeoutMs = 0;
};

}
}
}
