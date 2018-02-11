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

#include <queue>
#include <unordered_map>
#include <f1x/aasdk/Messenger/IMessenger.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class ChannelReceivePromiseQueue
{
public:
    void push(ChannelId channelId, ReceivePromise::Pointer promise);
    ReceivePromise::Pointer pop(ChannelId channelId);
    bool isPending(ChannelId channelId) const;
    size_t size() const;
    bool empty() const;
    void clear();
    ReceivePromise::Pointer pop();

private:
    std::unordered_map<ChannelId, std::queue<ReceivePromise::Pointer>> queue_;
};

}
}
}
