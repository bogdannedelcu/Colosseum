#include "common/common_utils/StrictMode.hpp"
STRICT_MODE_OFF //todo what does this do?
#ifndef RPCLIB_MSGPACK
#define RPCLIB_MSGPACK clmdep_msgpack
#endif // !RPCLIB_MSGPACK
#include "rpc/rpc_error.h"
    STRICT_MODE_ON

#include "colosseum_settings_parser.h"
#include "common/ColosseumSettings.hpp"
#include "common/common_utils/FileSystem.hpp"
#include "sensors/lidar/LidarSimpleParams.hpp"
#include "ros/ros.h"
#include "sensors/imu/ImuBase.hpp"
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "vehicles/car/api/CarRpcLibClient.hpp"
#include "yaml-cpp/yaml.h"
#include <colosseum_ros_pkgs/GimbalAngleEulerCmd.h>
#include <colosseum_ros_pkgs/GimbalAngleQuatCmd.h>
#include <colosseum_ros_pkgs/GPSYaw.h>
#include <colosseum_ros_pkgs/Land.h>
#include <colosseum_ros_pkgs/LandGroup.h>
#include <colosseum_ros_pkgs/Reset.h>
#include <colosseum_ros_pkgs/Takeoff.h>
#include <colosseum_ros_pkgs/TakeoffGroup.h>
#include <colosseum_ros_pkgs/VelCmd.h>
#include <colosseum_ros_pkgs/VelCmdGroup.h>
#include <colosseum_ros_pkgs/CarControls.h>
#include <colosseum_ros_pkgs/CarState.h>
#include <colosseum_ros_pkgs/Environment.h>
#include <chrono>
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>
#include <image_transport/image_transport.h>
#include <iostream>
#include <math.h>
#include <math_common.h>
#include <mavros_msgs/State.h>
#include <nav_msgs/Odometry.h>
#include <opencv2/opencv.hpp>
#include <ros/callback_queue.h>
#include <ros/console.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/distortion_models.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/NavSatFix.h>
#include <colosseum_ros_pkgs/Altimeter.h> //hector_uav_msgs defunct?
#include <sensor_msgs/MagneticField.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Range.h>
#include <rosgraph_msgs/Clock.h>
#include <std_srvs/Empty.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>
#include <tf2/convert.h>
#include <unordered_map>
#include <memory>
    // #include "nodelet/nodelet.h"

    struct SimpleMatrix
{
    int rows;
    int cols;
    double* data;

    SimpleMatrix(int rows, int cols, double* data)
        : rows(rows), cols(cols), data(data)
    {
    }
};

struct VelCmd
{
    double x;
    double y;
    double z;
    colosseum::DrivetrainType drivetrain;
    colosseum::YawMode yaw_mode;
    std::string vehicle_name;

    // VelCmd() :
    //     x(0), y(0), z(0),
    //     vehicle_name("") {drivetrain = colosseum::DrivetrainType::MaxDegreeOfFreedom;
    //             yaw_mode = colosseum::YawMode();};

    // VelCmd(const double& x, const double& y, const double& z,
    //         colosseum::DrivetrainType drivetrain,
    //         const colosseum::YawMode& yaw_mode,
    //         const std::string& vehicle_name) :
    //     x(x), y(y), z(z),
    //     drivetrain(drivetrain),
    //     yaw_mode(yaw_mode),
    //     vehicle_name(vehicle_name) {};
};

struct GimbalCmd
{
    std::string vehicle_name;
    std::string camera_name;
    colosseum::Quaternionr target_quat;

    // GimbalCmd() : vehicle_name(vehicle_name), camera_name(camera_name), target_quat(colosseum::Quaternionr(1,0,0,0)) {}

    // GimbalCmd(const std::string& vehicle_name,
    //         const std::string& camera_name,
    //         const colosseum::Quaternionr& target_quat) :
    //         vehicle_name(vehicle_name), camera_name(camera_name), target_quat(target_quat) {};
};

class ColosseumROSWrapper
{
    using ColosseumSettings = colosseum::ColosseumSettings;
    using SensorBase = colosseum::SensorBase;
    using CameraSetting = colosseum::ColosseumSettings::CameraSetting;
    using CaptureSetting = colosseum::ColosseumSettings::CaptureSetting;
    using LidarSetting = colosseum::ColosseumSettings::LidarSetting;
    using VehicleSetting = colosseum::ColosseumSettings::VehicleSetting;
    using ImageRequest = colosseum::ImageCaptureBase::ImageRequest;
    using ImageResponse = colosseum::ImageCaptureBase::ImageResponse;
    using ImageType = colosseum::ImageCaptureBase::ImageType;

public:
    enum class COLOSSEUM_MODE : unsigned
    {
        DRONE,
        CAR
    };

    ColosseumROSWrapper(const ros::NodeHandle& nh, const ros::NodeHandle& nh_private, const std::string& host_ip);
    ~ColosseumROSWrapper(){};

    void initialize_colosseum();
    void initialize_ros();

    // std::vector<ros::CallbackQueue> callback_queues_;
    ros::AsyncSpinner img_async_spinner_;
    ros::AsyncSpinner lidar_async_spinner_;
    bool is_used_lidar_timer_cb_queue_;
    bool is_used_img_timer_cb_queue_;

private:
    struct SensorPublisher
    {
        SensorBase::SensorType sensor_type;
        std::string sensor_name;
        ros::Publisher publisher;
    };

    // utility struct for a SINGLE robot
    class VehicleROS
    {
    public:
        virtual ~VehicleROS() {}
        std::string vehicle_name;

        /// All things ROS
        ros::Publisher odom_local_pub;
        ros::Publisher global_gps_pub;
        ros::Publisher env_pub;
        colosseum_ros_pkgs::Environment env_msg;
        std::vector<SensorPublisher> sensor_pubs;
        // handle lidar seperately for max performance as data is collected on its own thread/callback
        std::vector<SensorPublisher> lidar_pubs;

        nav_msgs::Odometry curr_odom;
        sensor_msgs::NavSatFix gps_sensor_msg;

        std::vector<geometry_msgs::TransformStamped> static_tf_msg_vec;

        ros::Time stamp;

        std::string odom_frame_id;
        /// Status
        // bool is_armed_;
        // std::string mode_;
    };

    class CarROS : public VehicleROS
    {
    public:
        colosseum::CarApiBase::CarState curr_car_state;

        ros::Subscriber car_cmd_sub;
        ros::Publisher car_state_pub;
        colosseum_ros_pkgs::CarState car_state_msg;

        bool has_car_cmd;
        colosseum::CarApiBase::CarControls car_cmd;
    };

    class MultiRotorROS : public VehicleROS
    {
    public:
        /// State
        colosseum::MultirotorState curr_drone_state;
        // bool in_air_; // todo change to "status" and keep track of this

        ros::Subscriber vel_cmd_body_frame_sub;
        ros::Subscriber vel_cmd_world_frame_sub;

        ros::ServiceServer takeoff_srvr;
        ros::ServiceServer land_srvr;

        bool has_vel_cmd;
        VelCmd vel_cmd;

        /// Status
        // bool in_air_; // todo change to "status" and keep track of this
    };

    /// ROS timer callbacks
    void img_response_timer_cb(const ros::TimerEvent& event); // update images from colosseum_client_ every nth sec
    void drone_state_timer_cb(const ros::TimerEvent& event); // update drone state from colosseum_client_ every nth sec
    void lidar_timer_cb(const ros::TimerEvent& event);

    /// ROS subscriber callbacks
    void vel_cmd_world_frame_cb(const colosseum_ros_pkgs::VelCmd::ConstPtr& msg, const std::string& vehicle_name);
    void vel_cmd_body_frame_cb(const colosseum_ros_pkgs::VelCmd::ConstPtr& msg, const std::string& vehicle_name);

    void vel_cmd_group_body_frame_cb(const colosseum_ros_pkgs::VelCmdGroup& msg);
    void vel_cmd_group_world_frame_cb(const colosseum_ros_pkgs::VelCmdGroup& msg);

    void vel_cmd_all_world_frame_cb(const colosseum_ros_pkgs::VelCmd& msg);
    void vel_cmd_all_body_frame_cb(const colosseum_ros_pkgs::VelCmd& msg);

    // void vel_cmd_body_frame_cb(const colosseum_ros_pkgs::VelCmd& msg, const std::string& vehicle_name);
    void gimbal_angle_quat_cmd_cb(const colosseum_ros_pkgs::GimbalAngleQuatCmd& gimbal_angle_quat_cmd_msg);
    void gimbal_angle_euler_cmd_cb(const colosseum_ros_pkgs::GimbalAngleEulerCmd& gimbal_angle_euler_cmd_msg);

    // commands
    void car_cmd_cb(const colosseum_ros_pkgs::CarControls::ConstPtr& msg, const std::string& vehicle_name);
    void update_commands();

    // state, returns the simulation timestamp best guess based on drone state timestamp, colosseum needs to return timestap for environment
    ros::Time update_state();
    void update_and_publish_static_transforms(VehicleROS* vehicle_ros);
    void publish_vehicle_state();

    /// ROS service callbacks
    bool takeoff_srv_cb(colosseum_ros_pkgs::Takeoff::Request& request, colosseum_ros_pkgs::Takeoff::Response& response, const std::string& vehicle_name);
    bool takeoff_group_srv_cb(colosseum_ros_pkgs::TakeoffGroup::Request& request, colosseum_ros_pkgs::TakeoffGroup::Response& response);
    bool takeoff_all_srv_cb(colosseum_ros_pkgs::Takeoff::Request& request, colosseum_ros_pkgs::Takeoff::Response& response);
    bool land_srv_cb(colosseum_ros_pkgs::Land::Request& request, colosseum_ros_pkgs::Land::Response& response, const std::string& vehicle_name);
    bool land_group_srv_cb(colosseum_ros_pkgs::LandGroup::Request& request, colosseum_ros_pkgs::LandGroup::Response& response);
    bool land_all_srv_cb(colosseum_ros_pkgs::Land::Request& request, colosseum_ros_pkgs::Land::Response& response);
    bool reset_srv_cb(colosseum_ros_pkgs::Reset::Request& request, colosseum_ros_pkgs::Reset::Response& response);

    /// ROS tf broadcasters
    void publish_camera_tf(const ImageResponse& img_response, const ros::Time& ros_time, const std::string& frame_id, const std::string& child_frame_id);
    void publish_odom_tf(const nav_msgs::Odometry& odom_msg);

    /// camera helper methods
    sensor_msgs::CameraInfo generate_cam_info(const std::string& camera_name, const CameraSetting& camera_setting, const CaptureSetting& capture_setting) const;
    cv::Mat manual_decode_depth(const ImageResponse& img_response) const;

    sensor_msgs::ImagePtr get_img_msg_from_response(const ImageResponse& img_response, const ros::Time curr_ros_time, const std::string frame_id);
    sensor_msgs::ImagePtr get_depth_img_msg_from_response(const ImageResponse& img_response, const ros::Time curr_ros_time, const std::string frame_id);

    void process_and_publish_img_response(const std::vector<ImageResponse>& img_response_vec, const int img_response_idx, const std::string& vehicle_name);

    // methods which parse setting json ang generate ros pubsubsrv
    void create_ros_pubs_from_settings_json();
    void append_static_camera_tf(VehicleROS* vehicle_ros, const std::string& camera_name, const CameraSetting& camera_setting);
    void append_static_lidar_tf(VehicleROS* vehicle_ros, const std::string& lidar_name, const colosseum::LidarSimpleParams& lidar_setting);
    void append_static_vehicle_tf(VehicleROS* vehicle_ros, const VehicleSetting& vehicle_setting);
    void set_nans_to_zeros_in_pose(VehicleSetting& vehicle_setting) const;
    void set_nans_to_zeros_in_pose(const VehicleSetting& vehicle_setting, CameraSetting& camera_setting) const;
    void set_nans_to_zeros_in_pose(const VehicleSetting& vehicle_setting, LidarSetting& lidar_setting) const;

    /// utils. todo parse into an ColosseumLib<->ROS conversion class
    tf2::Quaternion get_tf2_quat(const colosseum::Quaternionr& colosseum_quat) const;
    colosseum::Quaternionr get_colosseum_quat(const geometry_msgs::Quaternion& geometry_msgs_quat) const;
    colosseum::Quaternionr get_colosseum_quat(const tf2::Quaternion& tf2_quat) const;
    nav_msgs::Odometry get_odom_msg_from_multirotor_state(const colosseum::MultirotorState& drone_state) const;
    nav_msgs::Odometry get_odom_msg_from_car_state(const colosseum::CarApiBase::CarState& car_state) const;
    colosseum_ros_pkgs::CarState get_roscarstate_msg_from_car_state(const colosseum::CarApiBase::CarState& car_state) const;
    colosseum::Pose get_colosseum_pose(const float& x, const float& y, const float& z, const colosseum::Quaternionr& colosseum_quat) const;
    colosseum_ros_pkgs::GPSYaw get_gps_msg_from_colosseum_geo_point(const colosseum::GeoPoint& geo_point) const;
    sensor_msgs::NavSatFix get_gps_sensor_msg_from_colosseum_geo_point(const colosseum::GeoPoint& geo_point) const;
    sensor_msgs::Imu get_imu_msg_from_colosseum(const colosseum::ImuBase::Output& imu_data) const;
    colosseum_ros_pkgs::Altimeter get_altimeter_msg_from_colosseum(const colosseum::BarometerBase::Output& alt_data) const;
    sensor_msgs::Range get_range_from_colosseum(const colosseum::DistanceSensorData& dist_data) const;
    sensor_msgs::PointCloud2 get_lidar_msg_from_colosseum(const colosseum::LidarData& lidar_data, const std::string& vehicle_name, const std::string& sensor_name) const;
    sensor_msgs::NavSatFix get_gps_msg_from_colosseum(const colosseum::GpsBase::Output& gps_data) const;
    sensor_msgs::MagneticField get_mag_msg_from_colosseum(const colosseum::MagnetometerBase::Output& mag_data) const;
    colosseum_ros_pkgs::Environment get_environment_msg_from_colosseum(const colosseum::Environment::State& env_data) const;

    // not used anymore, but can be useful in future with an unreal camera calibration environment
    void read_params_from_yaml_and_fill_cam_info_msg(const std::string& file_name, sensor_msgs::CameraInfo& cam_info) const;
    void convert_yaml_to_simple_mat(const YAML::Node& node, SimpleMatrix& m) const; // todo ugly

    // simulation time utility
    ros::Time colosseum_timestamp_to_ros(const colosseum::TTimePoint& stamp) const;
    ros::Time chrono_timestamp_to_ros(const std::chrono::system_clock::time_point& stamp) const;

    // Utility methods to convert colosseum_client_
    colosseum::MultirotorRpcLibClient* get_multirotor_client();
    colosseum::CarRpcLibClient* get_car_client();

private:
    ros::NodeHandle nh_;
    ros::NodeHandle nh_private_;

    std::string host_ip_;

    // subscriber / services for ALL robots
    ros::Subscriber vel_cmd_all_body_frame_sub_;
    ros::Subscriber vel_cmd_all_world_frame_sub_;
    ros::ServiceServer takeoff_all_srvr_;
    ros::ServiceServer land_all_srvr_;

    // todo - subscriber / services for a GROUP of robots, which is defined by a list of `vehicle_name`s passed in the ros msg / srv request
    ros::Subscriber vel_cmd_group_body_frame_sub_;
    ros::Subscriber vel_cmd_group_world_frame_sub_;
    ros::ServiceServer takeoff_group_srvr_;
    ros::ServiceServer land_group_srvr_;

    COLOSSEUM_MODE colosseum_mode_ = COLOSSEUM_MODE::DRONE;

    ros::ServiceServer reset_srvr_;
    ros::Publisher origin_geo_point_pub_; // home geo coord of drones
    colosseum::GeoPoint origin_geo_point_; // gps coord of unreal origin
    colosseum_ros_pkgs::GPSYaw origin_geo_point_msg_; // todo duplicate

    ColosseumSettingsParser colosseum_settings_parser_;
    std::unordered_map<std::string, std::unique_ptr<VehicleROS>> vehicle_name_ptr_map_;
    static const std::unordered_map<int, std::string> image_type_int_to_string_map_;

    bool is_vulkan_; // rosparam obtained from launch file. If vulkan is being used, we BGR encoding instead of RGB

    std::unique_ptr<colosseum::RpcLibClientBase> colosseum_client_ = nullptr;
    // seperate busy connections to colosseum, update in their own thread
    colosseum::RpcLibClientBase colosseum_client_images_;
    colosseum::RpcLibClientBase colosseum_client_lidar_;

    // todo not sure if async spinners shuold be inside this class, or should be instantiated in colosseum_node.cpp, and cb queues should be public
    // todo for multiple drones with multiple sensors, this won't scale. make it a part of VehicleROS?
    ros::CallbackQueue img_timer_cb_queue_;
    ros::CallbackQueue lidar_timer_cb_queue_;

    std::mutex drone_control_mutex_;

    // gimbal control
    bool has_gimbal_cmd_;
    GimbalCmd gimbal_cmd_;

    /// ROS tf
    const std::string COLOSSEUM_FRAME_ID = "world_ned";
    std::string world_frame_id_ = COLOSSEUM_FRAME_ID;
    const std::string COLOSSEUM_ODOM_FRAME_ID = "odom_local_ned";
    const std::string ENU_ODOM_FRAME_ID = "odom_local_enu";
    std::string odom_frame_id_ = COLOSSEUM_ODOM_FRAME_ID;
    tf2_ros::TransformBroadcaster tf_broadcaster_;
    tf2_ros::StaticTransformBroadcaster static_tf_pub_;

    bool isENU_ = false;
    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;

    /// ROS params
    double vel_cmd_duration_;

    /// ROS Timers.
    ros::Timer colosseum_img_response_timer_;
    ros::Timer colosseum_control_update_timer_;
    ros::Timer colosseum_lidar_update_timer_;

    typedef std::pair<std::vector<ImageRequest>, std::string> colosseum_img_request_vehicle_name_pair;
    std::vector<colosseum_img_request_vehicle_name_pair> colosseum_img_request_vehicle_name_pair_vec_;
    std::vector<image_transport::Publisher> image_pub_vec_;
    std::vector<ros::Publisher> cam_info_pub_vec_;

    std::vector<sensor_msgs::CameraInfo> camera_info_msg_vec_;

    /// ROS other publishers
    ros::Publisher clock_pub_;
    rosgraph_msgs::Clock ros_clock_;
    bool publish_clock_ = false;

    ros::Subscriber gimbal_angle_quat_cmd_sub_;
    ros::Subscriber gimbal_angle_euler_cmd_sub_;

    static constexpr char CAM_YML_NAME[] = "camera_name";
    static constexpr char WIDTH_YML_NAME[] = "image_width";
    static constexpr char HEIGHT_YML_NAME[] = "image_height";
    static constexpr char K_YML_NAME[] = "camera_matrix";
    static constexpr char D_YML_NAME[] = "distortion_coefficients";
    static constexpr char R_YML_NAME[] = "rectification_matrix";
    static constexpr char P_YML_NAME[] = "projection_matrix";
    static constexpr char DMODEL_YML_NAME[] = "distortion_model";
};
