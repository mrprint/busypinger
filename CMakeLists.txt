cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0074 NEW)
project(busypinger)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52.0 COMPONENTS system program_options REQUIRED)
include_directories("include" ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
add_executable(${PROJECT_NAME} 
    "server/service_base.cpp"
    "server/service_base.hpp"
    "server/main.cpp"
    "server/service.hpp"
    "server/payload.cpp"
    "server/payload.hpp"
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -municode -std=c++14")
#set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_OPTIONS "-pthread")
#set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} wsock32 ws2_32 wtsapi32)
