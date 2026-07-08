// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef colosseum_RpcLibAdaptorsBase_hpp
#define colosseum_RpcLibAdaptorsBase_hpp

#include "common/Common.hpp"
#include "common/CommonStructs.hpp"
#include "physics/Kinematics.hpp"
#include "physics/Environment.hpp"
#include "common/ImageCaptureBase.hpp"
#include "safety/SafetyEval.hpp"
#include "api/WorldSimApiBase.hpp"

#include "common/common_utils/WindowsApisCommonPre.hpp"
#include "rpc/msgpack.hpp"
#include "common/common_utils/WindowsApisCommonPost.hpp"

namespace colosseum_rpclib
{

    class RpcLibAdaptorsBase
    {
    public:
        template <typename TSrc, typename TDest>
        static void to(const std::vector<TSrc>& s, std::vector<TDest>& d)
        {
            d.clear();
            for (size_t i = 0; i < s.size(); ++i)
                d.push_back(s.at(i).to());
        }

        template <typename TSrc, typename TDest>
        static void from(const std::vector<TSrc>& s, std::vector<TDest>& d)
        {
            d.clear();
            for (size_t i = 0; i < s.size(); ++i)
                d.push_back(TDest(s.at(i)));
        }

        struct Vector2r
        {
            colosseum::real_T x_val = 0, y_val = 0;
            MSGPACK_DEFINE_ARRAY(x_val, y_val);

            Vector2r()
            {
            }

            Vector2r(const colosseum::Vector2r& s)
            {
                x_val = s.x();
                y_val = s.y();
            }
            colosseum::Vector2r to() const
            {
                return colosseum::Vector2r(x_val, y_val);
            }
        };

        struct Vector3r
        {
            colosseum::real_T x_val = 0, y_val = 0, z_val = 0;
            MSGPACK_DEFINE_ARRAY(x_val, y_val, z_val);

            Vector3r()
            {
            }

            Vector3r(const colosseum::Vector3r& s)
            {
                x_val = s.x();
                y_val = s.y();
                z_val = s.z();
            }
            colosseum::Vector3r to() const
            {
                return colosseum::Vector3r(x_val, y_val, z_val);
            }
        };

        struct CollisionInfo
        {
            bool has_collided = false;
            Vector3r normal;
            Vector3r impact_point;
            Vector3r position;
            colosseum::real_T penetration_depth = 0;
            colosseum::TTimePoint time_stamp = 0;
            std::string object_name;
            int object_id = -1;

            MSGPACK_DEFINE_ARRAY(has_collided, penetration_depth, time_stamp, normal, impact_point, position, object_name, object_id);

            CollisionInfo()
            {
            }

            CollisionInfo(const colosseum::CollisionInfo& s)
            {
                has_collided = s.has_collided;
                normal = s.normal;
                impact_point = s.impact_point;
                position = s.position;
                penetration_depth = s.penetration_depth;
                time_stamp = s.time_stamp;
                object_name = s.object_name;
                object_id = s.object_id;
            }

            colosseum::CollisionInfo to() const
            {
                return colosseum::CollisionInfo(has_collided, normal.to(), impact_point.to(), position.to(), penetration_depth, time_stamp, object_name, object_id);
            }
        };

        struct Quaternionr
        {
            colosseum::real_T w_val = 1, x_val = 0, y_val = 0, z_val = 0;
            MSGPACK_DEFINE_ARRAY(w_val, x_val, y_val, z_val);

            Quaternionr()
            {
            }

            Quaternionr(const colosseum::Quaternionr& s)
            {
                w_val = s.w();
                x_val = s.x();
                y_val = s.y();
                z_val = s.z();
            }
            colosseum::Quaternionr to() const
            {
                return colosseum::Quaternionr(w_val, x_val, y_val, z_val);
            }
        };

        struct Pose
        {
            Vector3r position;
            Quaternionr orientation;
            MSGPACK_DEFINE_ARRAY(position, orientation);

            Pose()
            {
            }
            Pose(const colosseum::Pose& s)
            {
                position = s.position;
                orientation = s.orientation;
            }
            colosseum::Pose to() const
            {
                return colosseum::Pose(position.to(), orientation.to());
            }
        };

        struct GeoPoint
        {
            double latitude = 0, longitude = 0;
            float altitude = 0;
            MSGPACK_DEFINE_ARRAY(latitude, longitude, altitude);

            GeoPoint()
            {
            }

            GeoPoint(const colosseum::GeoPoint& s)
            {
                latitude = s.latitude;
                longitude = s.longitude;
                altitude = s.altitude;
            }
            colosseum::GeoPoint to() const
            {
                return colosseum::GeoPoint(latitude, longitude, altitude);
            }
        };

        struct RCData
        {
            uint64_t timestamp = 0;
            float pitch = 0, roll = 0, throttle = 0, yaw = 0;
            float left_z = 0, right_z = 0;
            uint16_t switches = 0;
            std::string vendor_id = "";
            bool is_initialized = false; //is RC connected?
            bool is_valid = false; //must be true for data to be valid

            MSGPACK_DEFINE_ARRAY(timestamp, pitch, roll, throttle, yaw, left_z, right_z, switches, vendor_id, is_initialized, is_valid);

            RCData()
            {
            }

            RCData(const colosseum::RCData& s)
            {
                timestamp = s.timestamp;
                pitch = s.pitch;
                roll = s.roll;
                throttle = s.throttle;
                yaw = s.yaw;
                left_z = s.left_z;
                right_z = s.right_z;
                switches = s.switches;
                vendor_id = s.vendor_id;
                is_initialized = s.is_initialized;
                is_valid = s.is_valid;
            }
            colosseum::RCData to() const
            {
                colosseum::RCData d;
                d.timestamp = timestamp;
                d.pitch = pitch;
                d.roll = roll;
                d.throttle = throttle;
                d.yaw = yaw;
                d.left_z = left_z;
                d.right_z = right_z;
                d.switches = switches;
                d.vendor_id = vendor_id;
                d.is_initialized = is_initialized;
                d.is_valid = is_valid;

                return d;
            }
        };

        struct ProjectionMatrix
        {
            float matrix[4][4];

            MSGPACK_DEFINE_ARRAY(matrix);

            ProjectionMatrix()
            {
            }

            ProjectionMatrix(const colosseum::ProjectionMatrix& s)
            {
                for (auto i = 0; i < 4; ++i)
                    for (auto j = 0; j < 4; ++j)
                        matrix[i][j] = s.matrix[i][j];
            }

            colosseum::ProjectionMatrix to() const
            {
                colosseum::ProjectionMatrix s;
                for (auto i = 0; i < 4; ++i)
                    for (auto j = 0; j < 4; ++j)
                        s.matrix[i][j] = matrix[i][j];
                return s;
            }
        };

        struct Box2D
        {
            Vector2r min;
            Vector2r max;

            MSGPACK_DEFINE_ARRAY(min, max);

            Box2D()
            {
            }

            Box2D(const colosseum::Box2D& s)
            {
                min = s.min;
                max = s.max;
            }

            colosseum::Box2D to() const
            {
                colosseum::Box2D s;
                s.min = min.to();
                s.max = max.to();

                return s;
            }
        };

        struct Box3D
        {
            Vector3r min;
            Vector3r max;

            MSGPACK_DEFINE_ARRAY(min, max);

            Box3D()
            {
            }

            Box3D(const colosseum::Box3D& s)
            {
                min = s.min;
                max = s.max;
            }

            colosseum::Box3D to() const
            {
                colosseum::Box3D s;
                s.min = min.to();
                s.max = max.to();

                return s;
            }
        };

        struct DetectionInfo
        {
            std::string name;
            GeoPoint geo_point;
            Box2D box2D;
            Box3D box3D;
            Pose relative_pose;

            MSGPACK_DEFINE_ARRAY(name, geo_point, box2D, box3D, relative_pose);

            DetectionInfo()
            {
            }

            DetectionInfo(const colosseum::DetectionInfo& d)
            {
                name = d.name;
                geo_point = d.geo_point;
                box2D = d.box2D;
                box3D = d.box3D;
                relative_pose = d.relative_pose;
            }

            colosseum::DetectionInfo to() const
            {
                colosseum::DetectionInfo d;
                d.name = name;
                d.geo_point = geo_point.to();
                d.box2D = box2D.to();
                d.box3D = box3D.to();
                d.relative_pose = relative_pose.to();

                return d;
            }

            static std::vector<DetectionInfo> from(
                const std::vector<colosseum::DetectionInfo>& request)
            {
                std::vector<DetectionInfo> request_adaptor;
                for (const auto& item : request)
                    request_adaptor.push_back(DetectionInfo(item));

                return request_adaptor;
            }
            static std::vector<colosseum::DetectionInfo> to(
                const std::vector<DetectionInfo>& request_adapter)
            {
                std::vector<colosseum::DetectionInfo> request;
                for (const auto& item : request_adapter)
                    request.push_back(item.to());

                return request;
            }
        };

        struct CameraInfo
        {
            Pose pose;
            float fov;
            ProjectionMatrix proj_mat;

            MSGPACK_DEFINE_ARRAY(pose, fov, proj_mat);

            CameraInfo()
            {
            }

            CameraInfo(const colosseum::CameraInfo& s)
            {
                pose = s.pose;
                fov = s.fov;
                proj_mat = ProjectionMatrix(s.proj_mat);
            }

            colosseum::CameraInfo to() const
            {
                colosseum::CameraInfo s;
                s.pose = pose.to();
                s.fov = fov;
                s.proj_mat = proj_mat.to();

                return s;
            }
        };

        struct KinematicsState
        {
            Vector3r position;
            Quaternionr orientation;

            Vector3r linear_velocity;
            Vector3r angular_velocity;

            Vector3r linear_acceleration;
            Vector3r angular_acceleration;

            MSGPACK_DEFINE_ARRAY(position, orientation, linear_velocity, angular_velocity, linear_acceleration, angular_acceleration);

            KinematicsState()
            {
            }

            KinematicsState(const colosseum::Kinematics::State& s)
            {
                position = s.pose.position;
                orientation = s.pose.orientation;
                linear_velocity = s.twist.linear;
                angular_velocity = s.twist.angular;
                linear_acceleration = s.accelerations.linear;
                angular_acceleration = s.accelerations.angular;
            }

            colosseum::Kinematics::State to() const
            {
                colosseum::Kinematics::State s;
                s.pose.position = position.to();
                s.pose.orientation = orientation.to();
                s.twist.linear = linear_velocity.to();
                s.twist.angular = angular_velocity.to();
                s.accelerations.linear = linear_acceleration.to();
                s.accelerations.angular = angular_acceleration.to();

                return s;
            }
        };

        struct EnvironmentState
        {
            Vector3r position;
            GeoPoint geo_point;

            //these fields are computed
            Vector3r gravity;
            float air_pressure;
            float temperature;
            float air_density;

            MSGPACK_DEFINE_ARRAY(position, geo_point, gravity, air_pressure, temperature, air_density);

            EnvironmentState()
            {
            }

            EnvironmentState(const colosseum::Environment::State& s)
            {
                position = s.position;
                geo_point = s.geo_point;
                gravity = s.gravity;
                air_pressure = s.air_pressure;
                temperature = s.temperature;
                air_density = s.air_density;
            }

            colosseum::Environment::State to() const
            {
                colosseum::Environment::State s;
                s.position = position.to();
                s.geo_point = geo_point.to();
                s.gravity = gravity.to();
                s.air_pressure = air_pressure;
                s.temperature = temperature;
                s.air_density = air_density;

                return s;
            }
        };

        struct ImageRequest
        {
            std::string camera_name;
            colosseum::ImageCaptureBase::ImageType image_type;
            bool pixels_as_float;
            bool compress;

            MSGPACK_DEFINE_ARRAY(camera_name, image_type, pixels_as_float, compress);

            ImageRequest()
            {
            }

            ImageRequest(const colosseum::ImageCaptureBase::ImageRequest& s)
                : camera_name(s.camera_name)
                , image_type(s.image_type)
                , pixels_as_float(s.pixels_as_float)
                , compress(s.compress)
            {
            }

            colosseum::ImageCaptureBase::ImageRequest to() const
            {
                return { camera_name, image_type, pixels_as_float, compress };
            }

            static std::vector<ImageRequest> from(
                const std::vector<colosseum::ImageCaptureBase::ImageRequest>& request)
            {
                std::vector<ImageRequest> request_adaptor;
                for (const auto& item : request)
                    request_adaptor.push_back(ImageRequest(item));

                return request_adaptor;
            }
            static std::vector<colosseum::ImageCaptureBase::ImageRequest> to(
                const std::vector<ImageRequest>& request_adapter)
            {
                std::vector<colosseum::ImageCaptureBase::ImageRequest> request;
                for (const auto& item : request_adapter)
                    request.push_back(item.to());

                return request;
            }
        };

        struct ImageResponse
        {
            std::vector<uint8_t> image_data_uint8;
            std::vector<float> image_data_float;

            std::string camera_name;
            Vector3r camera_position;
            Quaternionr camera_orientation;
            colosseum::TTimePoint time_stamp;
            std::string message;
            bool pixels_as_float;
            bool compress;
            int width, height;
            colosseum::ImageCaptureBase::ImageType image_type;

            MSGPACK_DEFINE_ARRAY(image_data_uint8, image_data_float, camera_position, camera_name,
                               camera_orientation, time_stamp, message, pixels_as_float, compress, width, height, image_type);

            ImageResponse()
            {
            }

            ImageResponse(const colosseum::ImageCaptureBase::ImageResponse& s)
            {
                pixels_as_float = s.pixels_as_float;

                image_data_uint8 = s.image_data_uint8;
                image_data_float = s.image_data_float;

                camera_name = s.camera_name;
                camera_position = Vector3r(s.camera_position);
                camera_orientation = Quaternionr(s.camera_orientation);
                time_stamp = s.time_stamp;
                message = s.message;
                compress = s.compress;
                width = s.width;
                height = s.height;
                image_type = s.image_type;
            }

            colosseum::ImageCaptureBase::ImageResponse to() const
            {
                colosseum::ImageCaptureBase::ImageResponse d;

                d.pixels_as_float = pixels_as_float;

                if (!pixels_as_float)
                    d.image_data_uint8 = image_data_uint8;
                else
                    d.image_data_float = image_data_float;

                d.camera_name = camera_name;
                d.camera_position = camera_position.to();
                d.camera_orientation = camera_orientation.to();
                d.time_stamp = time_stamp;
                d.message = message;
                d.compress = compress;
                d.width = width;
                d.height = height;
                d.image_type = image_type;

                return d;
            }

            static std::vector<colosseum::ImageCaptureBase::ImageResponse> to(
                const std::vector<ImageResponse>& response_adapter)
            {
                std::vector<colosseum::ImageCaptureBase::ImageResponse> response;
                for (const auto& item : response_adapter)
                    response.push_back(item.to());

                return response;
            }
            static std::vector<ImageResponse> from(
                const std::vector<colosseum::ImageCaptureBase::ImageResponse>& response)
            {
                std::vector<ImageResponse> response_adapter;
                for (const auto& item : response)
                    response_adapter.push_back(ImageResponse(item));

                return response_adapter;
            }
        };

        struct LidarData
        {

            colosseum::TTimePoint time_stamp; // timestamp
            std::vector<float> point_cloud; // data
            Pose pose;
            std::vector<int> segmentation;

            MSGPACK_DEFINE_ARRAY(time_stamp, point_cloud, pose, segmentation);

            LidarData()
            {
            }

            LidarData(const colosseum::LidarData& s)
            {
                time_stamp = s.time_stamp;
                point_cloud = s.point_cloud;
                pose = s.pose;
                segmentation = s.segmentation;
            }

            colosseum::LidarData to() const
            {
                colosseum::LidarData d;

                d.time_stamp = time_stamp;
                d.point_cloud = point_cloud;
                d.pose = pose.to();
                d.segmentation = segmentation;

                return d;
            }
        };

        struct ImuData
        {
            colosseum::TTimePoint time_stamp;
            Quaternionr orientation;
            Vector3r angular_velocity;
            Vector3r linear_acceleration;

            MSGPACK_DEFINE_ARRAY(time_stamp, orientation, angular_velocity, linear_acceleration);

            ImuData()
            {
            }

            ImuData(const colosseum::ImuBase::Output& s)
            {
                time_stamp = s.time_stamp;
                orientation = s.orientation;
                angular_velocity = s.angular_velocity;
                linear_acceleration = s.linear_acceleration;
            }

            colosseum::ImuBase::Output to() const
            {
                colosseum::ImuBase::Output d;

                d.time_stamp = time_stamp;
                d.orientation = orientation.to();
                d.angular_velocity = angular_velocity.to();
                d.linear_acceleration = linear_acceleration.to();

                return d;
            }
        };

        struct BarometerData
        {
            colosseum::TTimePoint time_stamp;
            colosseum::real_T altitude;
            colosseum::real_T pressure;
            colosseum::real_T qnh;

            MSGPACK_DEFINE_ARRAY(time_stamp, altitude, pressure, qnh);

            BarometerData()
            {
            }

            BarometerData(const colosseum::BarometerBase::Output& s)
            {
                time_stamp = s.time_stamp;
                altitude = s.altitude;
                pressure = s.pressure;
                qnh = s.qnh;
            }

            colosseum::BarometerBase::Output to() const
            {
                colosseum::BarometerBase::Output d;

                d.time_stamp = time_stamp;
                d.altitude = altitude;
                d.pressure = pressure;
                d.qnh = qnh;

                return d;
            }
        };

        struct MagnetometerData
        {
            colosseum::TTimePoint time_stamp;
            Vector3r magnetic_field_body;
            std::vector<float> magnetic_field_covariance; // not implemented in MagnetometerBase.hpp

            MSGPACK_DEFINE_ARRAY(time_stamp, magnetic_field_body, magnetic_field_covariance);

            MagnetometerData()
            {
            }

            MagnetometerData(const colosseum::MagnetometerBase::Output& s)
            {
                time_stamp = s.time_stamp;
                magnetic_field_body = s.magnetic_field_body;
                magnetic_field_covariance = s.magnetic_field_covariance;
            }

            colosseum::MagnetometerBase::Output to() const
            {
                colosseum::MagnetometerBase::Output d;

                d.time_stamp = time_stamp;
                d.magnetic_field_body = magnetic_field_body.to();
                d.magnetic_field_covariance = magnetic_field_covariance;

                return d;
            }
        };

        struct GnssReport
        {
            GeoPoint geo_point;
            colosseum::real_T eph = 0.0, epv = 0.0;
            Vector3r velocity;
            colosseum::GpsBase::GnssFixType fix_type;
            uint64_t time_utc = 0;

            MSGPACK_DEFINE_ARRAY(geo_point, eph, epv, velocity, fix_type, time_utc);

            GnssReport()
            {
            }

            GnssReport(const colosseum::GpsBase::GnssReport& s)
            {
                geo_point = s.geo_point;
                eph = s.eph;
                epv = s.epv;
                velocity = s.velocity;
                fix_type = s.fix_type;
                time_utc = s.time_utc;
            }

            colosseum::GpsBase::GnssReport to() const
            {
                colosseum::GpsBase::GnssReport d;

                d.geo_point = geo_point.to();
                d.eph = eph;
                d.epv = epv;
                d.velocity = velocity.to();
                d.fix_type = fix_type;
                d.time_utc = time_utc;

                return d;
            }
        };

        struct GpsData
        {
            colosseum::TTimePoint time_stamp;
            GnssReport gnss;
            bool is_valid = false;

            MSGPACK_DEFINE_ARRAY(time_stamp, gnss, is_valid);

            GpsData()
            {
            }

            GpsData(const colosseum::GpsBase::Output& s)
            {
                time_stamp = s.time_stamp;
                gnss = s.gnss;
                is_valid = s.is_valid;
            }

            colosseum::GpsBase::Output to() const
            {
                colosseum::GpsBase::Output d;

                d.time_stamp = time_stamp;
                d.gnss = gnss.to();
                d.is_valid = is_valid;

                return d;
            }
        };

        struct DistanceSensorData
        {
            colosseum::TTimePoint time_stamp;
            colosseum::real_T distance; //meters
            colosseum::real_T min_distance; //m
            colosseum::real_T max_distance; //m
            Pose relative_pose;

            MSGPACK_DEFINE_ARRAY(time_stamp, distance, min_distance, max_distance, relative_pose);

            DistanceSensorData()
            {
            }

            DistanceSensorData(const colosseum::DistanceSensorData& s)
            {
                time_stamp = s.time_stamp;
                distance = s.distance;
                min_distance = s.min_distance;
                max_distance = s.max_distance;
                relative_pose = s.relative_pose;
            }

            colosseum::DistanceSensorData to() const
            {
                colosseum::DistanceSensorData d;

                d.time_stamp = time_stamp;
                d.distance = distance;
                d.min_distance = min_distance;
                d.max_distance = max_distance;
                d.relative_pose = relative_pose.to();

                return d;
            }
        };

        struct MeshPositionVertexBuffersResponse
        {
            Vector3r position;
            Quaternionr orientation;

            std::vector<float> vertices;
            std::vector<uint32_t> indices;
            std::string name;

            MSGPACK_DEFINE_ARRAY(position, orientation, vertices, indices, name);

            MeshPositionVertexBuffersResponse()
            {
            }

            MeshPositionVertexBuffersResponse(const colosseum::MeshPositionVertexBuffersResponse& s)
            {
                position = Vector3r(s.position);
                orientation = Quaternionr(s.orientation);

                vertices = s.vertices;
                indices = s.indices;

                if (vertices.size() == 0)
                    vertices.push_back(0);
                if (indices.size() == 0)
                    indices.push_back(0);

                name = s.name;
            }

            colosseum::MeshPositionVertexBuffersResponse to() const
            {
                colosseum::MeshPositionVertexBuffersResponse d;
                d.position = position.to();
                d.orientation = orientation.to();
                d.vertices = vertices;
                d.indices = indices;
                d.name = name;

                return d;
            }

            static std::vector<colosseum::MeshPositionVertexBuffersResponse> to(
                const std::vector<MeshPositionVertexBuffersResponse>& response_adapter)
            {
                std::vector<colosseum::MeshPositionVertexBuffersResponse> response;
                for (const auto& item : response_adapter)
                    response.push_back(item.to());

                return response;
            }

            static std::vector<MeshPositionVertexBuffersResponse> from(
                const std::vector<colosseum::MeshPositionVertexBuffersResponse>& response)
            {
                std::vector<MeshPositionVertexBuffersResponse> response_adapter;
                for (const auto& item : response)
                    response_adapter.push_back(MeshPositionVertexBuffersResponse(item));

                return response_adapter;
            }
        };
    };
}

MSGPACK_ADD_ENUM(colosseum::SafetyEval::SafetyViolationType_);
MSGPACK_ADD_ENUM(colosseum::SafetyEval::ObsAvoidanceStrategy);
MSGPACK_ADD_ENUM(colosseum::ImageCaptureBase::ImageType);
MSGPACK_ADD_ENUM(colosseum::WorldSimApiBase::WeatherParameter);
MSGPACK_ADD_ENUM(colosseum::GpsBase::GnssFixType);

#endif
