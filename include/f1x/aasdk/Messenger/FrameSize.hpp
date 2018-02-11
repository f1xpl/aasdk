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

#include <f1x/aasdk/Common/Data.hpp>
#include <f1x/aasdk/Messenger/FrameSizeType.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class FrameSize
{
public:
    FrameSize(size_t frameSize, size_t totalSize);
    FrameSize(size_t frameSize);
    FrameSize(const common::DataConstBuffer& buffer);

    common::Data getData() const;
    size_t getSize() const;

    static size_t getSizeOf(FrameSizeType type);

private:
    FrameSizeType frameSizeType_;
    size_t frameSize_;
    size_t totalSize_;
};

}
}
}
