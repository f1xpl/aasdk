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

#include <boost/asio.hpp>
#include <mutex>

namespace f1x
{
namespace aasdk
{
namespace io
{

class IOContextWrapper
{
public:
    IOContextWrapper();
    explicit IOContextWrapper(boost::asio::io_service& ioService);
    explicit IOContextWrapper(boost::asio::io_service::strand& strand);

    template<typename CompletionHandlerType>
    void post(CompletionHandlerType&& handler)
    {
        if(ioService_ != nullptr)
        {
            ioService_->post(std::move(handler));
        }
        else if(strand_ != nullptr)
        {
            strand_->post(std::move(handler));
        }
    }

    template<typename CompletionHandlerType>
    void dispatch(CompletionHandlerType&& handler)
    {
        if(ioService_ != nullptr)
        {
            ioService_->dispatch(std::move(handler));
        }
        else if(strand_ != nullptr)
        {
            strand_->dispatch(std::move(handler));
        }
    }

    void reset();
    bool isActive() const;

private:
    boost::asio::io_service* ioService_;
    boost::asio::io_service::strand* strand_;
};

}
}
}
