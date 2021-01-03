cmake_minimum_required(VERSION 3.13)
project(visible-polygon-extractor)

set(CMAKE_CXX_STANDARD 14)

find_package(realsense2 REQUIRED)

find_package(OpenCV REQUIRED)

find_package(Boost COMPONENTS filesystem program_options REQUIRED)

find_package(nlohmann_json REQUIRED)

include_directories(visible-polygon-extractor
        PUBLIC
        ${realsense2_INCLUDE_DIR}
        ${OpenCV_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        )

add_library(vpe
        src/ground-polygon-calculator.h
        src/ground-polygon-calculator.cpp
        src/visible-polygon-extractor.h
        src/visible-polygon-extractor.cpp
        src/debug-output-writer.cpp src/debug-output-writer.h src/basic-definitions.h)

add_executable(visible-polygon-extractor src/main.cpp)

target_link_libraries(visible-polygon-extractor
        ${realsense2_LIBRARY}
        ${OpenCV_LIBS}
        ${Boost_FILESYSTEM_LIBRARIES}
        ${Boost_PROGRAM_OPTIONS_LIBRARIES}
        nlohmann_json::nlohmann_json
        vpe
        )
