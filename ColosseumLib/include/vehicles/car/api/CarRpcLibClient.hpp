// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef colosseum_CarRpcLibClient_hpp
#define colosseum_CarRpcLibClient_hpp

#include "common/Common.hpp"
#include <functional>
#include "common/CommonStructs.hpp"
#include "vehicles/car/api/CarApiBase.hpp"
#include "api/RpcLibClientBase.hpp"
#include "common/ImageCaptureBase.hpp"

namespace colosseum
{

    class CarRpcLibClient : public RpcLibClientBase
    {
    public:
        CarRpcLibClient(const string& ip_address = "localhost", uint16_t port = RpcLibPort, float timeout_sec = 60);

        void setCarControls(const CarApiBase::CarControls& controls, const std::string& vehicle_name = "");
        CarApiBase::CarState getCarState(const std::string& vehicle_name = "");
        CarApiBase::CarControls getCarControls(const std::string& vehicle_name = "");
        virtual ~CarRpcLibClient(); //required for pimpl
    };
}
#endif
