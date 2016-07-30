cmake_minimum_required(VERSION 2.8.11)

project(TTP_GRASP)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pthread -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -pipe -g")

set(PAAL ${CMAKE_CURRENT_SOURCE_DIR}/paal/)
file(GLOB BOOST_SUBS "${PAAL}/boost/*")
foreach(dir ${BOOST_SUBS})
    include_directories("${dir}/include/")
endforeach()
include_directories("${BOOST_SUBS}/numeric_bindings/")
include_directories (${PAAL}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ttp/")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/grasp/")

add_executable(ttpgrasp main.cpp)
target_link_libraries(ttpgrasp libttp libgrasp)
