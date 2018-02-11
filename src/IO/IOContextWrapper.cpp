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

#include <f1x/aasdk/IO/IOContextWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace io
{

IOContextWrapper::IOContextWrapper()
    : ioService_(nullptr)
    , strand_(nullptr)
{

}

IOContextWrapper::IOContextWrapper(boost::asio::io_service& ioService)
    : ioService_(&ioService)
    , strand_(nullptr)
{

}

IOContextWrapper::IOContextWrapper(boost::asio::io_service::strand& strand)
    : ioService_(nullptr)
    , strand_(&strand)
{

}

void IOContextWrapper::reset()
{
    ioService_ = nullptr;
    strand_ = nullptr;
}

bool IOContextWrapper::isActive() const
{
    return ioService_ != nullptr || strand_ != nullptr;
}

}
}
}
