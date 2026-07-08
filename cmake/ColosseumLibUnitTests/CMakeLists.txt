cmake_minimum_required(VERSION 3.5.0)
project(ColosseumLibUnitTests)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake-modules") 
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../cmake-modules/CommonSetup.cmake")
CommonSetup()

IncludeEigen()

SetupConsoleBuild()

include_directories(
  ${COLOSSEUM_ROOT}/ColosseumLibUnitTests
  ${COLOSSEUM_ROOT}/ColosseumLib/include
  ${COLOSSEUM_ROOT}/MavLinkCom/include
)

AddExecutableSource()

CommonTargetLink()
target_link_libraries(${PROJECT_NAME} ColosseumLib)
target_link_libraries(${PROJECT_NAME} MavLinkCom)
target_link_libraries(${PROJECT_NAME} ${RPC_LIB})
