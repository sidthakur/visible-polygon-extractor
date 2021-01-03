//
// Created by Siddharth Thakur on 1/2/21.
//

#ifndef VISIBLE_POLYGON_EXTRACTOR_BASIC_DEFINITIONS_H
#define VISIBLE_POLYGON_EXTRACTOR_BASIC_DEFINITIONS_H

#include <boost/geometry.hpp>
#include <boost/qvm/mat.hpp>
#include <boost/qvm/vec.hpp>

namespace vpe {
    const int rgb_width = 640;
    const int rgb_height = 480;

    const int d_width = 848;
    const int d_height = 480;

    const float max_y_for_ground = 0.130;   // meters
    const float min_y_for_ground = 0.110;   // meters

    const int step = 20;

    const int grid_length_width = (rgb_width - 1) / step;

    const int grid_length_height = (rgb_height - 1) / step;

    const int no_of_measured_points = (grid_length_width + 1) * (grid_length_height + 1);

    struct Point {
        float x;
        float y;
        float z;
        int w;
        int h;
        int i;
        bool near_ground;
    };

    typedef boost::geometry::model::d3::point_xyz<float> PointXYZ;

    typedef boost::geometry::model::ring<PointXYZ> PointsRing;

    const int no_of_rectangles = (grid_length_width) * (grid_length_height);

    typedef boost::geometry::model::d2::point_xy<int> PixelXY;

    typedef boost::geometry::model::ring<PixelXY> PixelsRing;

    // Store index of points_index for easy access
    struct IndexRectangles {
        int a;
        int b;
        int c;
        int d;
    };

    typedef boost::qvm::vec<float, 3> Point3F;

    const float roll_angle = -10 * M_PI / 180.0f;

    const float yaw_angle = 0.5 * M_PI / 180.0f;

    const boost::qvm::mat<float, 3, 3> roll_matrix = boost::qvm::rotx_mat<3>(roll_angle);

    const boost::qvm::mat<float, 3, 3> yaw_matrix = boost::qvm::rotz_mat<3>(yaw_angle);

} // namespace vpe
#endif //VISIBLE_POLYGON_EXTRACTOR_BASIC_DEFINITIONS_H
