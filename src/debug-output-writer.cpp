//
// Created by Siddharth Thakur on 1/2/21.
//

#include "debug-output-writer.h"

vpe::DebugOutputWriter::DebugOutputWriter(std::string &outputDir)
        : output_dir(outputDir) {}

void vpe::DebugOutputWriter::InitializeOutputDirectory() {
    if (debug_output_enabled) {
        std::cout << "Debug output enabled." << std::endl;
        std::cout << " |- Output dir: " << output_dir << std::endl;
        boost::filesystem::remove_all(output_dir);
        boost::filesystem::create_directory(output_dir);
        if (images_output_enabled) {
            path_for_images = output_dir + "/images";
            std::cout << " |- Images enabled. dir: " << path_for_images << std::endl;
            boost::filesystem::create_directory(path_for_images);
        }
        if (ground_pixel_output_enabled) {
            path_for_ground_pixels = output_dir + "/ground-pixels";
            std::cout << " |- Ground pixels enabled. dir: " << path_for_ground_pixels << std::endl;
            boost::filesystem::create_directory(path_for_ground_pixels);
        }
        if (ground_pixel_rectangle_output_enabled) {
            path_for_ground_pixels_rectangles = output_dir + "/ground-pixel-rectangles";
            std::cout << " |- Ground pixel rectangles enabled. dir: " << path_for_ground_pixels_rectangles << std::endl;
            boost::filesystem::create_directory(path_for_ground_pixels_rectangles);
        }
        if (merged_ground_pixel_polygon_output_enabled) {
            path_for_merged_ground_pixel_polygon = output_dir + "/merged-ground-pixel-polygons";
            std::cout << " |- Merged ground pixel rectangles enabled. dir: " << path_for_merged_ground_pixel_polygon
                      << std::endl;
            boost::filesystem::create_directory(path_for_merged_ground_pixel_polygon);
        }
        if (ground_polygon_enabled) {
            path_for_ground_polygons = output_dir + "/ground-polygons";
            std::cout << " |- Ground polygons enabled. dir: " << path_for_ground_polygons << std::endl;
            boost::filesystem::create_directory(path_for_ground_polygons);
        }
    }
}

void vpe::DebugOutputWriter::SaveImage(rs2::video_frame *color_frame_ptr) {
    if (debug_output_enabled && images_output_enabled) {
        static int w = color_frame_ptr->get_width();
        static int h = color_frame_ptr->get_height();
        unsigned long long int frame_number = color_frame_ptr->get_frame_number();
        std::string filename = OutputFilename(path_for_images, frame_number, FileExtension::JPEG);
        if (color_frame_ptr->get_profile().format() != RS2_FORMAT_RGB8) {
            return;
        }
        cv::Mat mat_rgb = cv::Mat(cv::Size(w, h),
                                  CV_8UC3,
                                  (void *) color_frame_ptr->get_data(),
                                  cv::Mat::AUTO_STEP);
        cv::Mat mat_bgr;
        cvtColor(mat_rgb, mat_bgr, cv::COLOR_RGB2BGR);
        cv::imwrite(filename, mat_bgr);
    }
}

void vpe::DebugOutputWriter::WriteGroundPixels(std::array<vpe::Point, no_of_measured_points> &points,
                                               const unsigned long long &frame_number) {
    if (debug_output_enabled && ground_pixel_output_enabled) {
        std::vector<std::array<int, 2>> ground_pixels = {};
        std::for_each(points.begin(), points.end(), [&](const vpe::Point &point) {
            if (point.near_ground) {
                ground_pixels.push_back(std::array<int, 2>{point.w, point.h});
            }
        });
        nlohmann::json output = {{"groundPixels", ground_pixels}};
        std::string filename = OutputFilename(path_for_ground_pixels, frame_number, FileExtension::JSON);
        WriteJson(output, filename);
    }
}


void
vpe::DebugOutputWriter::WriteGroundPixelRectangles(std::array<vpe::IndexRectangles, no_of_rectangles> &index_rectangles,
                                                   std::array<vpe::Point, no_of_measured_points> &points,
                                                   const unsigned long long &frame_number) {
    if (debug_output_enabled && ground_pixel_rectangle_output_enabled) {
        std::vector<std::array<std::array<int, 2>, 4>> ground_pixel_rectangles = {};
        std::for_each(index_rectangles.begin(), index_rectangles.end(), [&](vpe::IndexRectangles rectangle) {
            if (points[rectangle.a].near_ground &&
                points[rectangle.b].near_ground &&
                points[rectangle.c].near_ground &&
                points[rectangle.d].near_ground) {
                ground_pixel_rectangles.push_back(std::array<std::array<int, 2>, 4>{
                        std::array<int, 2>{points[rectangle.a].w, points[rectangle.a].h},
                        std::array<int, 2>{points[rectangle.b].w, points[rectangle.b].h},
                        std::array<int, 2>{points[rectangle.c].w, points[rectangle.c].h},
                        std::array<int, 2>{points[rectangle.d].w, points[rectangle.d].h}
                });
            }
        });
        nlohmann::json output = {{"groundPixelRectangles", ground_pixel_rectangles}};
        std::string filename = OutputFilename(path_for_ground_pixels_rectangles,
                                              frame_number,
                                              FileExtension::JSON);
        WriteJson(output, filename);
    }
}

void vpe::DebugOutputWriter::WriteMergedGroundPixelPolygon(std::vector<vpe::PixelsRing> &merged_ground_pixel_polygons,
                                                           const unsigned long long &frame_number) {
    if (debug_output_enabled && merged_ground_pixel_polygon_output_enabled) {
        std::vector<std::vector<std::array<int, 2>>> merged_output_ground_pixel_rectangles(
                merged_ground_pixel_polygons.size());
        int i = 0;
        std::for_each(merged_ground_pixel_polygons.begin(), merged_ground_pixel_polygons.end(),
                      [&](const vpe::PixelsRing &ring) {
                          std::vector<std::array<int, 2>> pixels(ring.size());
                          int j = 0;
                          std::for_each(ring.begin(), ring.end(), [&](const vpe::PixelXY &pixel_xy) {
                              pixels[j] = std::array<int, 2>{pixel_xy.x(), pixel_xy.y()};
                              ++j;
                          });
                          merged_output_ground_pixel_rectangles[i] = pixels;
                          ++i;
                      });
        nlohmann::json output = {{"groundPixelPolygons", merged_output_ground_pixel_rectangles}};
        std::string filename = OutputFilename(path_for_merged_ground_pixel_polygon,
                                              frame_number,
                                              FileExtension::JSON);
        WriteJson(output, filename);
    }
}

void vpe::DebugOutputWriter::WriteGroundPolygon(std::vector<vpe::PointsRing> &ground_polygons,
                                                const unsigned long long &frame_number) {
    if (debug_output_enabled && ground_polygon_enabled) {
        std::vector<std::vector<std::array<float, 3>>> output_ground_polygons(ground_polygons.size());
        int i = 0;
        std::for_each(ground_polygons.begin(), ground_polygons.end(), [&](const vpe::PointsRing &points_ring) {
            std::vector<std::array<float, 3>> points(points_ring.size());
            int j = 0;
            std::for_each(points_ring.begin(), points_ring.end(), [&](const vpe::PointXYZ &point) {
                points[j] = std::array<float, 3>{point.x(), point.y(), point.z()};
                ++j;
            });
            output_ground_polygons[i] = points;
            ++i;
        });
        nlohmann::json output = {{"groundPolygons", output_ground_polygons}};
        std::string filename = OutputFilename(path_for_ground_polygons,
                                              frame_number,
                                              FileExtension::JSON);
        WriteJson(output, filename);
    }
}

std::string vpe::DebugOutputWriter::OutputFilename(std::string &path,
                                                   const unsigned long long &frame_number,
                                                   FileExtension extension) {
    std::string filename = path + "/frame_";
    std::string frame_number_s = std::to_string(frame_number);
    frame_number_s = std::string(5 - frame_number_s.length(), '0') + frame_number_s;
    filename += frame_number_s;
    switch (extension) {
        case FileExtension::JPEG: {
            filename += ".jpeg";
            break;
        }
        case FileExtension::JSON: {
            filename += ".json";
            break;
        }
    }
    return filename;
}

void vpe::DebugOutputWriter::WriteJson(nlohmann::json &json, std::string &filename) {
    std::ofstream o(filename);
    o << std::setw(2) << json << std::endl;
}

