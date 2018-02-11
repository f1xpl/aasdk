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

#include <boost/endian/conversion.hpp>
#include <f1x/aasdk/Messenger/FrameSize.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

FrameSize::FrameSize(size_t frameSize, size_t totalSize)
    : frameSizeType_(FrameSizeType::EXTENDED)
    , frameSize_(frameSize)
    , totalSize_(totalSize)
{

}

FrameSize::FrameSize(size_t frameSize)
    : frameSizeType_(FrameSizeType::SHORT)
    , frameSize_(frameSize)
    , totalSize_(0)
{

}

FrameSize::FrameSize(const common::DataConstBuffer& buffer)
{
    if(buffer.size >= 2)
    {
        frameSizeType_ = FrameSizeType::SHORT;
        frameSize_ = boost::endian::big_to_native(reinterpret_cast<const uint16_t&>(buffer.cdata[0]));
    }

    if(buffer.size >= 6)
    {
        frameSizeType_ = FrameSizeType::EXTENDED;
        totalSize_ = boost::endian::big_to_native(reinterpret_cast<const uint32_t&>(buffer.cdata[2]));
    }
}

common::Data FrameSize::getData() const
{
    common::Data data;

    uint16_t frameSizeBig = boost::endian::native_to_big(static_cast<uint16_t>(frameSize_));
    const common::DataConstBuffer frameSizeBigBuffer(&frameSizeBig, sizeof(frameSizeBig));
    common::copy(data, frameSizeBigBuffer);

    if(frameSizeType_ == FrameSizeType::EXTENDED)
    {
        uint32_t totalSizeBig = boost::endian::native_to_big(static_cast<uint32_t>(totalSize_));
        const common::DataConstBuffer totalSizeBigBuffer(&totalSizeBig, sizeof(totalSizeBig));
        common::copy(data, totalSizeBigBuffer);
    }

    return data;
}

size_t FrameSize::getSize() const
{
    return frameSize_;
}

size_t FrameSize::getSizeOf(FrameSizeType type)
{
    return type == FrameSizeType::EXTENDED ? 6 : 2;
}

}
}
}
