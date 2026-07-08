cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(GazeboDrone)

set(COLOSSEUM_ROOT ..)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

find_package(gazebo REQUIRED)

include_directories(
    ${GAZEBO_INCLUDE_DIRS}
    ${COLOSSEUM_ROOT}/ColosseumLib/deps/eigen3
    ${COLOSSEUM_ROOT}/ColosseumLib/deps/rpclib/include
    ${COLOSSEUM_ROOT}/ColosseumLib/include
)

link_directories(
    ${GAZEBO_LIBRARY_DIRS}
    ../../build_release/output/lib
)

list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(
    ${PROJECT_NAME} 
    ${GAZEBO_LIBRARIES}
    pthread
    ColosseumLib
    rpc
)

