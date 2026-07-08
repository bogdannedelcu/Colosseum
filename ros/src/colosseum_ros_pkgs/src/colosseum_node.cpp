#include "ros/ros.h"
#include "colosseum_ros_wrapper.h"
#include <ros/spinner.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "colosseum_node");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");

    std::string host_ip = "localhost";
    nh_private.getParam("host_ip", host_ip);
    ColosseumROSWrapper colosseum_ros_wrapper(nh, nh_private, host_ip);

    if (colosseum_ros_wrapper.is_used_img_timer_cb_queue_) {
        colosseum_ros_wrapper.img_async_spinner_.start();
    }

    if (colosseum_ros_wrapper.is_used_lidar_timer_cb_queue_) {
        colosseum_ros_wrapper.lidar_async_spinner_.start();
    }

    ros::spin();

    return 0;
}