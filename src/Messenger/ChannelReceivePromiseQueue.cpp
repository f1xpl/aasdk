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

#include <f1x/aasdk/Messenger/ChannelReceivePromiseQueue.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

void ChannelReceivePromiseQueue::push(ChannelId channelId, ReceivePromise::Pointer promise)
{
    queue_[channelId].push(std::move(promise));
}

ReceivePromise::Pointer ChannelReceivePromiseQueue::pop(ChannelId channelId)
{
    auto& channelQueue = queue_.at(channelId);
    auto promise = std::move(channelQueue.front());
    channelQueue.pop();

    if(channelQueue.empty())
    {
        queue_.erase(channelId);
    }

    return promise;
}

bool ChannelReceivePromiseQueue::isPending(ChannelId channelId) const
{
    return queue_.count(channelId) > 0 && !queue_.at(channelId).empty();
}

size_t ChannelReceivePromiseQueue::size() const
{
    return queue_.size();
}

bool ChannelReceivePromiseQueue::empty() const
{
    return this->size() == 0;
}

void ChannelReceivePromiseQueue::clear()
{
    queue_.clear();
}

ReceivePromise::Pointer ChannelReceivePromiseQueue::pop()
{
    return this->pop(queue_.begin()->first);
}

}
}
}
