cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0074 NEW)
project(busypinger LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CLIENT_EXEC_NAME "bpingclient")
set(SERVER_EXEC_NAME "bpingservice")

find_package(Boost 1.70.0 COMPONENTS system program_options REQUIRED)
find_package(Qt5 COMPONENTS Core Quick REQUIRED)
include_directories(
    "submodules/crow/include"
    "submodules/WinReg/WinReg"
    ${Boost_INCLUDE_DIR}
)
link_directories(${Boost_LIBRARY_DIR})

add_executable(${SERVER_EXEC_NAME}
    "server/service_base.cpp"
    "server/service_base.hpp"
    "server/main.cpp"
    "server/service.hpp"
    "server/payload.cpp"
    "server/payload.hpp"
)

add_executable(${CLIENT_EXEC_NAME}
    "client/QmlEnvVar.cpp"
    "client/QmlEnvVar.h"
    "client/main.cpp"
    "client/qml.qrc"
)

if(THREADS_HAVE_PTHREAD_ARG)
    set_property(TARGET ${SERVER_EXEC_NAME} APPEND PROPERTY COMPILE_OPTIONS "-pthread")
    set_property(TARGET ${SERVER_EXEC_NAME} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
set_target_properties(${SERVER_EXEC_NAME} PROPERTIES LINK_FLAGS "-municode" COMPILE_FLAGS "-DUNICODE")
target_link_libraries(${SERVER_EXEC_NAME} ${Boost_LIBRARIES} z wsock32 ws2_32 iphlpapi wtsapi32)

set_target_properties(${CLIENT_EXEC_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    INCLUDE_CURRENT_DIR ON
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_property(TARGET ${CLIENT_EXEC_NAME} PROPERTY WIN32_EXECUTABLE TRUE)
endif()
target_compile_definitions(${CLIENT_EXEC_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${CLIENT_EXEC_NAME} PRIVATE Qt5::Core Qt5::Quick)

