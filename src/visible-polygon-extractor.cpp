//
// Created by Siddharth Thakur on 12/26/20.
//
// Basic functions to initialize the pipeline and handle new frames

#include "visible-polygon-extractor.h"

void vpe::InitializeConfig(rs2::config *config, const std::string &ros_bag_file) {
    config->enable_device_from_file(ros_bag_file);
    config->enable_stream(RS2_STREAM_COLOR,
                          -1,
                          vpe::rgb_width,
                          vpe::rgb_height,
                          rs2_format::RS2_FORMAT_RGB8,
                          30);
    config->enable_stream(RS2_STREAM_DEPTH,
                          -1,
                          vpe::d_width,
                          vpe::d_height,
                          rs2_format::RS2_FORMAT_Z16,
                          30);
}

void vpe::HandleNewFrames(rs2::pipeline *pipeline,
                          const std::string &path_for_images,
                          DebugOutputWriter &debug_output_writer) {
    static unsigned long long frame_number = 0;
    static rs2::frameset frameset;
    static rs2::align align(RS2_STREAM_COLOR);
    static rs2::hole_filling_filter hole_filling_filter(1);
    static rs2::pointcloud point_cloud;
    static std::array<vpe::Point, vpe::no_of_measured_points> measured_points = vpe::InitializeMeasuredPoints();
    static std::array<vpe::IndexRectangles, vpe::no_of_rectangles> index_rectangles = vpe::InitializeIndexRectangles();
    static std::array<vpe::PixelsRing, vpe::no_of_rectangles> pixel_rings = vpe::InitializePixelRings();

    pipeline->poll_for_frames(&frameset);
    if (frameset.size() == 0) {
        return;
    }

    rs2::video_frame color_frame = frameset.get_color_frame();
    if (frame_number == color_frame.get_frame_number()) {
        return;
    }

    auto handle_new_frame_start = std::chrono::high_resolution_clock::now();

    frame_number = color_frame.get_frame_number();
    std::cout << "Frame No: " << frame_number << " New frame." << std::endl;
    debug_output_writer.SaveImage(&color_frame);

    // RealSense filters - align depth frame to color frame and fill holes
    frameset = align.process(frameset);
    frameset = hole_filling_filter.process(frameset);

    rs2::depth_frame depth_frame = frameset.get_depth_frame();
    const rs2::points &points = point_cloud.calculate(depth_frame);
    const rs2::vertex *vertices = points.get_vertices();

    vpe::UpdateMeasuredPointsForCurrentFrame(vertices, measured_points);

    debug_output_writer.WriteGroundPixels(measured_points, frame_number);

    debug_output_writer.WriteGroundPixelRectangles(index_rectangles, measured_points, frame_number);

    vpe::ComputeGroundPolygon(measured_points,
                              index_rectangles,
                              pixel_rings,
                              frame_number,
                              debug_output_writer);

    auto handle_new_frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - handle_new_frame_start);
    std::cout << "Handle new frame (ms) : " << handle_new_frame_duration.count() << std::endl;

}