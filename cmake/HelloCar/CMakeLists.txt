cmake_minimum_required(VERSION 3.5.0)
project(HelloCar)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake-modules") 
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../cmake-modules/CommonSetup.cmake")
CommonSetup()

IncludeEigen()

SetupConsoleBuild()

## Specify additional locations of header files
include_directories(
  ${COLOSSEUM_ROOT}/HelloCar
  ${COLOSSEUM_ROOT}/ColosseumLib/include
  ${RPC_LIB_INCLUDES}
  ${COLOSSEUM_ROOT}/MavLinkCom/include
  ${COLOSSEUM_ROOT}/MavLinkCom/common_utils
)

AddExecutableSource()
			
CommonTargetLink()
target_link_libraries(${PROJECT_NAME} ColosseumLib)
target_link_libraries(${PROJECT_NAME} ${RPC_LIB})
