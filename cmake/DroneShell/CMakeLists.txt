cmake_minimum_required(VERSION 3.5.0)
project(DroneShell)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake-modules") 
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../cmake-modules/CommonSetup.cmake")
CommonSetup()

IncludeEigen()

SetupConsoleBuild()

include_directories(
  ${COLOSSEUM_ROOT}/DroneShell
  ${RPC_LIB_INCLUDES}
  ${COLOSSEUM_ROOT}/DroneShell/include;
  ${COLOSSEUM_ROOT}/ColosseumLib/include
  ${COLOSSEUM_ROOT}/MavLinkCom/include
)

AddExecutableSource()
			
CommonTargetLink()
target_link_libraries(${PROJECT_NAME} ColosseumLib)
target_link_libraries(${PROJECT_NAME} ${RPC_LIB})
