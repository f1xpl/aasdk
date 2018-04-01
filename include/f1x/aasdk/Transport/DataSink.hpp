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

#include <limits>
#include <boost/circular_buffer.hpp>
#include <f1x/aasdk/Common/Data.hpp>


namespace f1x
{
namespace aasdk
{
namespace transport
{

class DataSink
{
public:
    DataSink();

    common::DataBuffer fill();
    void commit(common::Data::size_type size);

    common::Data::size_type getAvailableSize();
    common::Data consume(common::Data::size_type size);

private:
    boost::circular_buffer<common::Data::value_type> data_;
    static constexpr common::Data::size_type cChunkSize = 16384;
};

}
}
}
