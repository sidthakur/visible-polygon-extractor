//
// Created by Siddharth Thakur on 12/26/20.
//
// Basic functions to initialize the pipeline and handle new frames

#include "rs-helpers.h"

void vpe::InitializeConfig(rs2::config *config) {
    config->enable_device_from_file(vpe::ros_bag_file);
    config->enable_stream(RS2_STREAM_COLOR,
                          -1,
                          1280,
                          800,
                          rs2_format::RS2_FORMAT_RGB8,
                          30);
    config->enable_stream(RS2_STREAM_DEPTH,
                          -1,
                          848,
                          480,
                          rs2_format::RS2_FORMAT_ANY,
                          30);
}

void vpe::HandleNewFrames(rs2::pipeline *pipeline) {
    static unsigned long long frame_number = 0;
    static rs2::frameset frameset;
    pipeline->poll_for_frames(&frameset);
    if (frameset.size() == 0)
        return;
    rs2::video_frame color_frame = frameset.get_color_frame();
    if (frame_number == color_frame.get_frame_number())
        return;
    frame_number = color_frame.get_frame_number();
    std::cout << "Frame No: " << frame_number << " New frame." << std::endl;
}