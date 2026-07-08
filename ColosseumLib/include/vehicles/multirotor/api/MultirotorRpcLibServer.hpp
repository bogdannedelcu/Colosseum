// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef colosseum_MultirotorRpcLibServer_hpp
#define colosseum_MultirotorRpcLibServer_hpp

#include "common/Common.hpp"
#include <functional>
#include "api/RpcLibServerBase.hpp"
#include "vehicles/multirotor/api/MultirotorApiBase.hpp"

namespace colosseum
{

    class MultirotorRpcLibServer : public RpcLibServerBase
    {
    public:
        MultirotorRpcLibServer(ApiProvider* api_provider, string server_address, uint16_t port = RpcLibPort);
        virtual ~MultirotorRpcLibServer();

    protected:
        virtual MultirotorApiBase* getVehicleApi(const std::string& vehicle_name) override
        {
            return static_cast<MultirotorApiBase*>(RpcLibServerBase::getVehicleApi(vehicle_name));
        }
    };
}
#endif
