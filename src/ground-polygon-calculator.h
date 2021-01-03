//
// Created by Siddharth Thakur on 12/28/20.
//

#ifndef VISIBLE_POLYGON_EXTRACTOR_GROUND_POLYGON_CALCULATOR_H
#define VISIBLE_POLYGON_EXTRACTOR_GROUND_POLYGON_CALCULATOR_H

#include "basic-definitions.h"
#include "debug-output-writer.h"

#include <iostream>
#include <chrono>
#include <cmath>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>

#include <librealsense2/rs.hpp>

namespace vpe {
    inline int Reshape2dIndexTo1dIndex(int w, int h, int width);

    Point3F RotatedPointForGivenIndex(const rs2::vertex *vertices, int i);

    bool IsPointNearGround(vpe::Point3F point);

    inline bool is_not_in_the_right_most_column_of_the_grid(int i);

    inline bool is_not_in_the_bottom_row_of_the_grid(int i);

    std::array<Point, no_of_measured_points> InitializeMeasuredPoints();

    std::array<IndexRectangles, no_of_rectangles> InitializeIndexRectangles();

    std::array<PixelsRing, no_of_rectangles> InitializePixelRings();

    void UpdateMeasuredPointsForCurrentFrame(const rs2::vertex *vertices,
                                             std::array<Point, no_of_measured_points> &points);

    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;

    class GraphBfsVisitor : public boost::default_bfs_visitor {
        std::vector<std::vector<int>> &merge_order;
        std::vector<int> &components;

    public:
        GraphBfsVisitor(std::vector<std::vector<int>> &mergeOrder,
                        std::vector<int> &components) : merge_order(mergeOrder),
                                                        components(components) {}

        void discover_vertex(const Graph::vertex_descriptor &s, const Graph &g);
    };

    std::vector<vpe::PointsRing> ComputeGroundPolygon(std::array<Point, no_of_measured_points> &measured_points,
                                                      std::array<IndexRectangles, no_of_rectangles> &index_rectangles,
                                                      std::array<PixelsRing, no_of_rectangles> &pixel_rings,
                                                      const unsigned long long int &frame_number,
                                                      vpe::DebugOutputWriter &debug_output_writer);

}   // namespace vpe

#endif //VISIBLE_POLYGON_EXTRACTOR_GROUND_POLYGON_CALCULATOR_H
