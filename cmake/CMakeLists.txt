cmake_minimum_required(VERSION 3.5.0)
project(Colosseum)

add_subdirectory("rpclib_wrapper")
add_subdirectory("ColosseumLib")
add_subdirectory("MavLinkCom")
add_subdirectory("ColosseumLibUnitTests")
add_subdirectory("HelloDrone")
add_subdirectory("HelloSpawnedDrones")
add_subdirectory("HelloCar")
add_subdirectory("DroneShell")
add_subdirectory("DroneServer")


