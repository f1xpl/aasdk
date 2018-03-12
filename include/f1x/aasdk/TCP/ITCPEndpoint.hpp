#pragma once

#include <memory>
#include <f1x/aasdk/Common/Data.hpp>
#include <f1x/aasdk/IO/Promise.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{

class ITCPEndpoint
{
public:
    typedef std::shared_ptr<ITCPEndpoint> Pointer;
    typedef io::Promise<void> Promise;

    virtual ~ITCPEndpoint() = default;

    virtual void send(common::DataConstBuffer buffer, Promise::Pointer promise) = 0;
    virtual void receive(common::DataBuffer buffer, Promise::Pointer promise) = 0;
    virtual void stop() = 0;
};

}
}
}
