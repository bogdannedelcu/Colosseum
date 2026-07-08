cmake_minimum_required(VERSION 3.10.0)
project(colosseum_ros_pkgs)

# set this to path to Colosseum root folder if you want your catkin workspace in a custom directory
set(COLOSSEUM_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

add_subdirectory("${COLOSSEUM_ROOT}/cmake/rpclib_wrapper" rpclib_wrapper)
add_subdirectory("${COLOSSEUM_ROOT}/cmake/ColosseumLib" ColosseumLib)
add_subdirectory("${COLOSSEUM_ROOT}/cmake/MavLinkCom" MavLinkCom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wstrict-null-sentinel -Wno-unused")
set(CXX_EXP_LIB "-nostdinc++ -I/usr/include/c++/8 -I/usr/include/x86_64-linux-gnu/c++/8 -nodefaultlibs 
-l/usr/lib/x86_64-linux-gnu/libc++.so -l/usr/lib/x86_64-linux-gnu/libc++abi.so
-lm -lc -lgcc_s -lgcc  
-lstdc++fs -fmax-errors=10")

set(RPC_LIB_INCLUDES " ${COLOSSEUM_ROOT}/external/rpclib/rpclib-2.3.0/include")
set(RPC_LIB rpc) # name of .a file with lib prefix
message(STATUS "found RPC_LIB_INCLUDES=${RPC_LIB_INCLUDES}")

# todo eigen3 in ColosseumLib already
# find_package(Eigen3 REQUIRED) 
find_package(Boost REQUIRED)
find_package(OpenCV REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  mavros_msgs
  message_generation
  nav_msgs
  # nodelet
  roscpp
  rospy
  sensor_msgs
  std_msgs
  geographic_msgs
  geometry_msgs
  std_srvs
  tf2
  tf2_ros
  tf2_sensor_msgs
  tf2_geometry_msgs
)

add_message_files(
  FILES
  GimbalAngleEulerCmd.msg
  GimbalAngleQuatCmd.msg
  GPSYaw.msg
  VelCmd.msg
  VelCmdGroup.msg
  CarControls.msg
  CarState.msg
  Altimeter.msg
  Environment.msg
)

add_service_files(
  FILES
  SetGPSPosition.srv
  Takeoff.srv
  TakeoffGroup.srv
  Land.srv
  LandGroup.srv
  Reset.srv
  SetLocalPosition.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
  geographic_msgs
)

catkin_package(
  INCLUDE_DIRS include
  # LIBRARIES colosseum_ros
  CATKIN_DEPENDS message_runtime roscpp std_msgs # nodelet 
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${COLOSSEUM_ROOT}/ColosseumLib/deps/eigen3
  ${COLOSSEUM_ROOT}/ColosseumLib/include
  ${RPC_LIB_INCLUDES}
  ${COLOSSEUM_ROOT}/MavLinkCom/include
  ${COLOSSEUM_ROOT}/MavLinkCom/common_utils
  ${OpenCV_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

add_library(colosseum_settings_parser src/colosseum_settings_parser.cpp)
target_link_libraries(colosseum_settings_parser ${catkin_LIBRARIES} ColosseumLib)

add_library(pd_position_controller_simple src/pd_position_controller_simple.cpp)
add_dependencies(pd_position_controller_simple ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(pd_position_controller_simple ${catkin_EXPORTED_TARGETS})

add_library(colosseum_ros src/colosseum_ros_wrapper.cpp)
add_dependencies(colosseum_ros ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(colosseum_ros ${catkin_LIBRARIES} ${OpenCV_LIBS} yaml-cpp ColosseumLib colosseum_settings_parser)

add_executable(colosseum_node src/colosseum_node.cpp)
target_link_libraries(colosseum_node colosseum_ros ${catkin_LIBRARIES} ColosseumLib)

add_executable(pd_position_controller_simple_node src/pd_position_controller_simple_node.cpp)
target_link_libraries(pd_position_controller_simple_node pd_position_controller_simple colosseum_ros ${catkin_LIBRARIES} ColosseumLib)

install(TARGETS
    #list of nodes
    colosseum_node
    pd_position_controller_simple_node
    DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS 
    #list of shared libraries
    colosseum_ros
    pd_position_controller_simple
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

install(FILES 
  #list of necessary files (xml...)
  README.md
  # settings.json
  # nodelet_plugins.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

# for shipping binaries. run `$ catin_make install`
install(DIRECTORY
  launch
  rviz
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  # PATTERN ".git" EXCLUDE
)
