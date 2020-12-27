//
// Created by Siddharth Thakur on 12/26/20.
//

#ifndef VISIBLE_POLYGON_ESTIMATOR_RS_HELPERS_H
#define VISIBLE_POLYGON_ESTIMATOR_RS_HELPERS_H

#include <iostream>
#include <thread>
#include <chrono>
#include <librealsense2/rs.hpp>

namespace vpe {
    const std::string ros_bag_file = "/Users/sthakur/workspace/toy-problem-data/20201226_113911.bag";

    void InitializeConfig(rs2::config *config);

    void HandleNewFrames(rs2::pipeline *pipeline);

} // namespace vpe

#endif //VISIBLE_POLYGON_ESTIMATOR_RS_HELPERS_H
