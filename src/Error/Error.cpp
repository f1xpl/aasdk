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

#include <f1x/aasdk/Error/Error.hpp>

namespace f1x
{
namespace aasdk
{
namespace error
{

Error::Error()
    : code_(ErrorCode::NONE)
    , nativeCode_(0)
{

}

Error::Error(ErrorCode code, uint32_t nativeCode)
    : code_(code)
    , nativeCode_(nativeCode)
{
    message_ = "AaSdk error code: " + std::to_string(static_cast<uint32_t>(code_))
                + ", native code: " + std::to_string(nativeCode_);
}

ErrorCode Error::getCode() const
{
    return code_;
}

uint32_t Error::getNativeCode() const
{
    return nativeCode_;
}

const char* Error::what() const noexcept
{
    return message_.c_str();
}

bool Error::operator!() const
{
    return code_ == ErrorCode::NONE;
}

bool Error::operator==(const Error& other) const
{
    return code_ == other.code_ && nativeCode_ == other.nativeCode_;
}

bool Error::operator==(const ErrorCode& code) const
{
    return code_ == code;
}

bool Error::operator!=(const ErrorCode& code) const
{
    return !operator==(code);
}

}
}
}
