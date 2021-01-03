//
// Created by Siddharth Thakur on 1/2/21.
//

#ifndef VISIBLE_POLYGON_EXTRACTOR_DEBUG_OUTPUT_WRITER_H
#define VISIBLE_POLYGON_EXTRACTOR_DEBUG_OUTPUT_WRITER_H

#include "basic-definitions.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/geometry.hpp>
#include <nlohmann/json.hpp>
#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

namespace vpe {
    class DebugOutputWriter {
        bool debug_output_enabled = true;
        bool images_output_enabled = true;
        bool ground_pixel_output_enabled = true;
        bool ground_pixel_rectangle_output_enabled = true;
        bool merged_ground_pixel_polygon_output_enabled = true;
        bool ground_polygon_enabled = true;

        std::string path_for_images;
        std::string path_for_ground_pixels;
        std::string path_for_ground_pixels_rectangles;
        std::string path_for_merged_ground_pixel_polygon;
        std::string path_for_ground_polygons;
        std::string &output_dir;

        enum FileExtension {
            JPEG,
            JSON
        };

    public:
        explicit DebugOutputWriter(std::string &outputDir);

    public:
        void InitializeOutputDirectory();

        void SaveImage(rs2::video_frame *color_frame_ptr);

        void WriteGroundPixels(std::array<vpe::Point, no_of_measured_points> &points,
                               const unsigned long long &frame_number);

        void WriteGroundPixelRectangles(std::array<vpe::IndexRectangles, no_of_rectangles> &index_rectangles,
                                        std::array<vpe::Point, no_of_measured_points> &points,
                                        const unsigned long long &frame_number);

        void WriteMergedGroundPixelPolygon(std::vector<vpe::PixelsRing> &merged_ground_pixel_polygons,
                                           const unsigned long long &frame_number);

        void WriteGroundPolygon(std::vector<vpe::PointsRing> &ground_polygons,
                                const unsigned long long &frame_number);

    private:
        static std::string OutputFilename(std::string &path,
                                          const unsigned long long &frame_number,
                                          FileExtension extension);

        static void WriteJson(nlohmann::json &json, std::string &filename);
    };
} // namespace vpe

#endif //VISIBLE_POLYGON_EXTRACTOR_DEBUG_OUTPUT_WRITER_H
