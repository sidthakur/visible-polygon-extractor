//
// Created by Siddharth Thakur on 12/26/20.
//

#ifndef VISIBLE_POLYGON_EXTRACTOR_VISIBLE_POLYGON_EXTRACTOR_H
#define VISIBLE_POLYGON_EXTRACTOR_VISIBLE_POLYGON_EXTRACTOR_H

#include "ground-polygon-calculator.h"
#include "debug-output-writer.h"

#include <iostream>
#include <boost/program_options.hpp>
#include <librealsense2/rs.hpp>


namespace vpe {

    void InitializeConfig(rs2::config *config, const std::string &ros_bag_file);

    void HandleNewFrames(rs2::pipeline *pipeline,
                         const std::string &path_for_images,
                         DebugOutputWriter &debug_output_writer);

} // namespace vpe

#endif //VISIBLE_POLYGON_EXTRACTOR_VISIBLE_POLYGON_EXTRACTOR_H
