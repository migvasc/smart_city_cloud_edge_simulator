cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(SmartCityEdgeSimulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(SimGrid 3.23 REQUIRED)

include_directories(${SimGrid_INCLUDE_DIR})

add_executable(simulationmain main.cpp CommunicationInterface.hpp MailBoxSingleton.hpp MailBoxSingleton.cpp DAGOfTasks.hpp DAGOfTasks.cpp DAGDispatcher.hpp DAGDispatcher.cpp DAGManager.hpp DAGManager.cpp SegmentTask.hpp SegmentTask.cpp )

target_link_libraries(simulationmain ${SimGrid_LIBRARY})

################
# Installation #
################
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/simulationmain
    DESTINATION bin)
