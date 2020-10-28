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

#include <functional>
#include <boost/asio.hpp>
#include <f1x/aasdk/Error/Error.hpp>
#include <f1x/aasdk/IO/IOContextWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace io
{

template<typename ResolveArgumentType, typename ErrorArgumentType = error::Error>
class Promise
{
public:
    typedef ResolveArgumentType ValueType;
    typedef ErrorArgumentType ErrorType;
    typedef std::function<void(ResolveArgumentType)> ResolveHandler;
    typedef std::function<void(ErrorArgumentType)> RejectHandler;
    typedef std::shared_ptr<Promise> Pointer;

    static Pointer defer(boost::asio::io_service& ioService)
    {
        return std::make_shared<Promise>(ioService);
    }

    static Pointer defer(boost::asio::io_service::strand& strand)
    {
        return std::make_shared<Promise>(strand);
    }

    Promise(boost::asio::io_service& ioService)
        : ioContextWrapper_(ioService)
    {

    }

    Promise(boost::asio::io_service::strand& strand)
        : ioContextWrapper_(strand)
    {

    }

    void then(ResolveHandler resolveHandler, RejectHandler rejectHandler = RejectHandler())
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        resolveHandler_ = std::move(resolveHandler);
        rejectHandler_ = std::move(rejectHandler);
    }

    void resolve(ResolveArgumentType argument)
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(resolveHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([argument = std::move(argument), resolveHandler = std::move(resolveHandler_)]() mutable {
                resolveHandler(std::move(argument));
            });
        }

        ioContextWrapper_.reset();
        rejectHandler_ = RejectHandler();
    }

    void reject(ErrorArgumentType error)
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(rejectHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([error = std::move(error), rejectHandler = std::move(rejectHandler_)]() mutable {
                rejectHandler(std::move(error));
            });
        }

        ioContextWrapper_.reset();
        resolveHandler_ = ResolveHandler();
    }

private:
    bool isPending() const
    {
        return ioContextWrapper_.isActive();
    }

    ResolveHandler resolveHandler_;
    RejectHandler rejectHandler_;
    IOContextWrapper ioContextWrapper_;
    std::mutex mutex_;

    Promise(const Promise&) = delete;
};

template<typename ErrorArgumentType>
class Promise<void, ErrorArgumentType>
{
public:
    typedef ErrorArgumentType ErrorType;
    typedef std::function<void()> ResolveHandler;
    typedef std::function<void(ErrorArgumentType)> RejectHandler;
    typedef std::shared_ptr<Promise> Pointer;

    static Pointer defer(boost::asio::io_service& ioService)
    {
        return std::make_shared<Promise>(ioService);
    }

    static Pointer defer(boost::asio::io_service::strand& strand)
    {
        return std::make_shared<Promise>(strand);
    }

    Promise(boost::asio::io_service& ioService)
        : ioContextWrapper_(ioService)
    {

    }

    Promise(boost::asio::io_service::strand& strand)
        : ioContextWrapper_(strand)
    {

    }

    void then(ResolveHandler resolveHandler, RejectHandler rejectHandler = RejectHandler())
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        resolveHandler_ = std::move(resolveHandler);
        rejectHandler_ = std::move(rejectHandler);
    }

    void resolve()
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(resolveHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([resolveHandler = std::move(resolveHandler_)]() mutable {
                resolveHandler();
            });
        }

        ioContextWrapper_.reset();
        rejectHandler_ = RejectHandler();
    }

    void reject(ErrorArgumentType error)
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(rejectHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([error = std::move(error), rejectHandler = std::move(rejectHandler_)]() mutable {
                rejectHandler(std::move(error));
            });
        }

        ioContextWrapper_.reset();
        resolveHandler_ = ResolveHandler();
    }

private:
    bool isPending() const
    {
        return ioContextWrapper_.isActive();
    }

    ResolveHandler resolveHandler_;
    RejectHandler rejectHandler_;
    IOContextWrapper ioContextWrapper_;
    std::mutex mutex_;

    Promise(const Promise&) = delete;
};

template<>
class Promise<void, void>
{
public:
    typedef std::function<void()> ResolveHandler;
    typedef std::function<void()> RejectHandler;
    typedef std::shared_ptr<Promise> Pointer;

    static Pointer defer(boost::asio::io_service& ioService)
    {
        return std::make_shared<Promise>(ioService);
    }

    static Pointer defer(boost::asio::io_service::strand& strand)
    {
        return std::make_shared<Promise>(strand);
    }

    Promise(boost::asio::io_service& ioService)
        : ioContextWrapper_(ioService)
    {

    }

    Promise(boost::asio::io_service::strand& strand)
        : ioContextWrapper_(strand)
    {

    }

    void then(ResolveHandler resolveHandler, RejectHandler rejectHandler = RejectHandler())
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        resolveHandler_ = std::move(resolveHandler);
        rejectHandler_ = std::move(rejectHandler);
    }

    void resolve()
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(resolveHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([resolveHandler = std::move(resolveHandler_)]() mutable {
                resolveHandler();
            });
        }

        ioContextWrapper_.reset();
        rejectHandler_ = RejectHandler();
    }

    void reject()
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(rejectHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([rejectHandler = std::move(rejectHandler_)]() mutable {
                rejectHandler();
            });
        }

        ioContextWrapper_.reset();
        resolveHandler_ = ResolveHandler();
    }

private:
    bool isPending() const
    {
        return ioContextWrapper_.isActive();
    }

    ResolveHandler resolveHandler_;
    RejectHandler rejectHandler_;
    IOContextWrapper ioContextWrapper_;
    std::mutex mutex_;

    Promise(const Promise&) = delete;
};

template<typename ResolveArgumentType>
class Promise<ResolveArgumentType, void>
{
public:
    typedef ResolveArgumentType ValueType;
    typedef std::function<void(ResolveArgumentType)> ResolveHandler;
    typedef std::function<void(void)> RejectHandler;
    typedef std::shared_ptr<Promise> Pointer;

    static Pointer defer(boost::asio::io_service& ioService)
    {
        return std::make_shared<Promise>(ioService);
    }

    static Pointer defer(boost::asio::io_service::strand& strand)
    {
        return std::make_shared<Promise>(strand);
    }

    Promise(boost::asio::io_service& ioService)
        : ioContextWrapper_(ioService)
    {

    }

    Promise(boost::asio::io_service::strand& strand)
        : ioContextWrapper_(strand)
    {

    }

    void then(ResolveHandler resolveHandler, RejectHandler rejectHandler = RejectHandler())
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        resolveHandler_ = std::move(resolveHandler);
        rejectHandler_ = std::move(rejectHandler);
    }

    void resolve(ResolveArgumentType argument)
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(resolveHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([argument = std::move(argument), resolveHandler = std::move(resolveHandler_)]() mutable {
                resolveHandler(std::move(argument));
            });
        }

        ioContextWrapper_.reset();
        rejectHandler_ = RejectHandler();
    }

    void reject()
    {
        std::lock_guard<decltype(mutex_)> lock(mutex_);

        if(rejectHandler_ != nullptr && this->isPending())
        {
            ioContextWrapper_.post([rejectHandler = std::move(rejectHandler_)]() mutable {
                rejectHandler();
            });
        }

        ioContextWrapper_.reset();
        resolveHandler_ = ResolveHandler();
    }

private:
    bool isPending() const
    {
        return ioContextWrapper_.isActive();
    }

    ResolveHandler resolveHandler_;
    RejectHandler rejectHandler_;
    IOContextWrapper ioContextWrapper_;
    std::mutex mutex_;

    Promise(const Promise&) = delete;
};


}
}
}
