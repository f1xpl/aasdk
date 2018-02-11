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

#include <memory>
#include <functional>
#include <f1x/aasdk/Error/Error.hpp>
#include <f1x/aasdk/IO/Promise.hpp>

namespace f1x
{
namespace aasdk
{
namespace io
{

template<typename SourceResolveArgumentType = void, typename DestinationResolveArgumentType = void>
class PromiseLink: public std::enable_shared_from_this<PromiseLink<SourceResolveArgumentType, DestinationResolveArgumentType>>
{
public:
    typedef std::shared_ptr<PromiseLink<SourceResolveArgumentType, DestinationResolveArgumentType>> Pointer;
    typedef std::function<DestinationResolveArgumentType(SourceResolveArgumentType)> TransformFunctor;

    PromiseLink(typename Promise<DestinationResolveArgumentType>::Pointer promise, TransformFunctor transformFunctor)
        : promise_(std::move(promise))
        , transformFunctor_(std::make_shared<TransformFunctor>(std::move(transformFunctor)))
    {

    }

    static void forward(Promise<SourceResolveArgumentType>& source, typename Promise<DestinationResolveArgumentType>::Pointer destination,
                        TransformFunctor transformFunctor = [](SourceResolveArgumentType&& argument) { return std::move(argument); })
    {
        auto link = std::make_shared<PromiseLink<SourceResolveArgumentType, DestinationResolveArgumentType>>(std::forward<typename Promise<DestinationResolveArgumentType>::Pointer>(destination),
                                                                                                             std::forward<TransformFunctor>(transformFunctor));
        source.then(link->getResolveHandler(), link->getRejectHandler());
    }

private:
    using std::enable_shared_from_this<PromiseLink<SourceResolveArgumentType, DestinationResolveArgumentType>>::shared_from_this;

    void resolve(SourceResolveArgumentType argument)
    {
        if(transformFunctor_ != nullptr)
        {
            promise_->resolve((*transformFunctor_)(std::move(argument)));
            transformFunctor_.reset();
        }
    }

    void reject(const error::Error& e)
    {
        promise_->reject(e);
    }

    typename Promise<SourceResolveArgumentType>::ResolveHandler getResolveHandler()
    {
        return std::bind(&PromiseLink::resolve, this->shared_from_this(), std::placeholders::_1);
    }

    typename Promise<SourceResolveArgumentType>::RejectHandler getRejectHandler()
    {
        return std::bind(&PromiseLink::reject, this->shared_from_this(), std::placeholders::_1);
    }

    typename Promise<DestinationResolveArgumentType>::Pointer promise_;
    std::shared_ptr<TransformFunctor> transformFunctor_;
};

template<>
class PromiseLink<void, void>: public std::enable_shared_from_this<PromiseLink<void, void>>
{
public:
    typedef std::shared_ptr<PromiseLink<void, void>> Pointer;

    PromiseLink(typename Promise<void>::Pointer promise)
        : promise_(std::move(promise))
    {

    }

    static void forward(Promise<void>& source, typename Promise<void>::Pointer destination)
    {
        auto link = std::make_shared<PromiseLink<void, void>>(std::forward<typename Promise<void>::Pointer>(destination));
        source.then(link->getResolveHandler(), link->getRejectHandler());
    }

private:
    using std::enable_shared_from_this<PromiseLink<void, void>>::shared_from_this;

    void resolve()
    {
        promise_->resolve();
    }

    void reject(const error::Error& e)
    {
        promise_->reject(e);
    }

    Promise<void>::ResolveHandler getResolveHandler()
    {
        return std::bind(&PromiseLink::resolve, this->shared_from_this());
    }

    Promise<void>::RejectHandler getRejectHandler()
    {
        return std::bind(&PromiseLink::reject, this->shared_from_this(), std::placeholders::_1);
    }

    typename Promise<void>::Pointer promise_;
};

}
}
}
