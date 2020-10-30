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

#include <stdexcept>
#include <string>
#include <f1x/aasdk/Error/ErrorCode.hpp>

namespace f1x
{
namespace aasdk
{
namespace error
{

class Error: public std::exception
{
public:
    Error();
    explicit Error(ErrorCode code, uint32_t nativeCode = 0);

    ErrorCode getCode() const;
    uint32_t getNativeCode() const;
    const char* what() const noexcept override;

    bool operator!() const;
    bool operator==(const Error& other) const;
    bool operator==(const ErrorCode& code) const;
    bool operator!=(const ErrorCode& code) const;

private:
    ErrorCode code_;
    uint32_t nativeCode_;
    std::string message_;
};

}
}
}
