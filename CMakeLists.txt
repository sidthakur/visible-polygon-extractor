cmake_minimum_required(VERSION 3.13)
project(visible-polygon-estimator)

set(CMAKE_CXX_STANDARD 14)

find_package(realsense2 REQUIRED)

include_directories(visible-polygon-estimator PUBLIC ${realsense2_INCLUDE_DIR})

add_library(rs-helpers src/rs-helpers.h src/rs-helpers.cpp)

add_executable(visible-polygon-estimator src/main.cpp)

target_link_libraries(visible-polygon-estimator ${realsense2_LIBRARY} rs-helpers)