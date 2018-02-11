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

#include <gmock/gmock.h>
#include <f1x/aasdk/USB/IAccessoryModeQuery.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{
namespace ut
{

class AccessoryModeQueryMock: public IAccessoryModeQuery
{
public:
    MOCK_METHOD1(start, void(Promise::Pointer promise));
    MOCK_METHOD0(cancel, void());
};

}
}
}
}
