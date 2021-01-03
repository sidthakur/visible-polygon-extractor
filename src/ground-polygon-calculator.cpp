//
// Created by Siddharth Thakur on 12/28/20.
//

#include "ground-polygon-calculator.h"
#include "debug-output-writer.h"

inline int vpe::Reshape2dIndexTo1dIndex(int w, int h, int width) {
    return (width * h) + w;
}

inline bool vpe::is_not_in_the_right_most_column_of_the_grid(int i) {
    return ((i + 1) % vpe::grid_length_width) != 0;
}

inline bool vpe::is_not_in_the_bottom_row_of_the_grid(int i) {
    return (i / vpe::grid_length_width) + 1 != vpe::grid_length_height;
}

vpe::Point3F vpe::RotatedPointForGivenIndex(const rs2::vertex *vertices, int i) {
    static boost::qvm::mat<float, 3, 3> rotation_matrix = vpe::roll_matrix * vpe::yaw_matrix;
    vpe::Point3F point = {vertices[i].x, vertices[i].y, vertices[i].z};
    point = rotation_matrix * point;
    return point;
}

bool vpe::IsPointNearGround(vpe::Point3F point) {
    if (point.a[1] < vpe::min_y_for_ground || point.a[1] > vpe::max_y_for_ground)
        return false;
    return true;
}

std::array<vpe::Point, vpe::no_of_measured_points> vpe::InitializeMeasuredPoints() {
    std::array<vpe::Point, vpe::no_of_measured_points> points = {};
    int i = 0;
    for (int h = 0; h < vpe::rgb_height; h = h + vpe::step) {
        for (int w = 0; w < vpe::rgb_width; w = w + vpe::step) {
            points[i].w = w;
            points[i].h = h;
            points[i].near_ground = false;
            points[i].i = vpe::Reshape2dIndexTo1dIndex(w, h, vpe::rgb_width);
            ++i;
        }
    }
    return points;
}

std::array<vpe::IndexRectangles, vpe::no_of_rectangles> vpe::InitializeIndexRectangles() {
    std::array<vpe::IndexRectangles, vpe::no_of_rectangles> rectangles = {};
    int i = 0;
    for (int h = 0; h < (vpe::grid_length_height); ++h) {
        for (int w = 0; w < (vpe::grid_length_width); ++w) {
            rectangles[i] = vpe::IndexRectangles(
                    {Reshape2dIndexTo1dIndex(w, h, vpe::grid_length_width + 1),
                     Reshape2dIndexTo1dIndex(w + 1, h, vpe::grid_length_width + 1),
                     Reshape2dIndexTo1dIndex(w + 1, h + 1, vpe::grid_length_width + 1),
                     Reshape2dIndexTo1dIndex(w, h + 1, vpe::grid_length_width + 1)}
            );
            ++i;
        }
    }
    return rectangles;
}

std::array<vpe::PixelsRing, vpe::no_of_rectangles> vpe::InitializePixelRings() {
    std::array<vpe::PixelsRing, vpe::no_of_rectangles> pixel_rings = {};
    int i = 0;
    for (int h = 0; h < vpe::rgb_height - vpe::step; h = h + vpe::step) {
        for (int w = 0; w < vpe::rgb_width - vpe::step; w = w + vpe::step) {
            // Pixel Ring defined in counter clockwise order
            pixel_rings[i] = vpe::PixelsRing{
                    vpe::PixelXY{w, h},
                    vpe::PixelXY{w, h + vpe::step},
                    vpe::PixelXY{w + vpe::step, h + vpe::step},
                    vpe::PixelXY{w + vpe::step, h},
                    vpe::PixelXY{w, h}
            };
            ++i;
        }
    }
    return pixel_rings;
}

void vpe::UpdateMeasuredPointsForCurrentFrame(const rs2::vertex *vertices,
                                              std::array<Point, no_of_measured_points> &points) {
    for (int i = 0; i < vpe::no_of_measured_points; ++i) {
        vpe::Point3F point_3f = vpe::RotatedPointForGivenIndex(vertices, points[i].i);
        points[i].x = point_3f.a[0];
        points[i].y = point_3f.a[1];
        points[i].z = point_3f.a[2];
        points[i].near_ground = vpe::IsPointNearGround(point_3f);
    }
}

void vpe::GraphBfsVisitor::discover_vertex(const Graph::vertex_descriptor &s, const Graph &g) {
    int i = (int) s;
    merge_order[components[i]].push_back(i);
}

std::vector<vpe::PointsRing> vpe::ComputeGroundPolygon(std::array<Point, no_of_measured_points> &measured_points,
                                                       std::array<IndexRectangles, no_of_rectangles> &index_rectangles,
                                                       std::array<PixelsRing, no_of_rectangles> &pixel_rings,
                                                       const unsigned long long int &frame_number,
                                                       vpe::DebugOutputWriter &debug_output_writer) {
    auto compute_ground_polygon_start = std::chrono::high_resolution_clock::now();

    std::vector<vpe::PixelsRing> ground_pixel_rings;
    std::vector<int> ground_pixel_rectangle_indexes;

    int i = 0;
    std::for_each(index_rectangles.begin(), index_rectangles.end(), [&](vpe::IndexRectangles rectangle) {
        if (measured_points[rectangle.a].near_ground &&
            measured_points[rectangle.b].near_ground &&
            measured_points[rectangle.c].near_ground &&
            measured_points[rectangle.d].near_ground) {
            ground_pixel_rectangle_indexes.push_back(i);
            ground_pixel_rings.push_back(pixel_rings[i]);
        }
        ++i;
    });

    // Populate a graph with adjacent pixel index_rectangles
    vpe::Graph g;
    i = 0;
    std::for_each(ground_pixel_rectangle_indexes.begin(), ground_pixel_rectangle_indexes.end(), [&](int index) {
        if (vpe::is_not_in_the_right_most_column_of_the_grid(index)) {
            auto rectangle_to_the_right = std::find(ground_pixel_rectangle_indexes.begin(),
                                                    ground_pixel_rectangle_indexes.end(),
                                                    index + 1);
            if (rectangle_to_the_right != ground_pixel_rectangle_indexes.end()) {
                int j = std::distance(ground_pixel_rectangle_indexes.begin(), rectangle_to_the_right);
                boost::add_edge(i, j, g);
            }
        }
        if (vpe::is_not_in_the_bottom_row_of_the_grid(index)) {
            auto rectangle_below = std::find(ground_pixel_rectangle_indexes.begin(),
                                             ground_pixel_rectangle_indexes.end(),
                                             index + vpe::grid_length_width);
            if (rectangle_below != ground_pixel_rectangle_indexes.end()) {
                int j = std::distance(ground_pixel_rectangle_indexes.begin(), rectangle_below);
                boost::add_edge(i, j, g);
            }
        }
        ++i;
    });

    // Identify connected components in the adjacent index_rectangles graph
    std::vector<int> components(boost::num_vertices(g));
    int no_of_connected_components = boost::connected_components(g, &components[0]);

    // Initialize output vectors
    std::vector<vpe::PointsRing> ground_polygons(no_of_connected_components);
    std::vector<vpe::PixelsRing> merged_ground_pixel_polygons(no_of_connected_components);

    // Sort the ground pixel index_rectangles index using breadth first search
    // to ensure clustering adjacent index_rectangles at each step
    std::vector<std::vector<int>> merge_order_for_clustering(no_of_connected_components);
    std::vector<bool> merge_ordered(no_of_connected_components, false);
    vpe::GraphBfsVisitor vis(merge_order_for_clustering, components);

    for (i = 0; i < components.size(); ++i) {
        if (merge_ordered[components[i]])
            continue;
        merge_ordered[components[i]] = true;
        boost::breadth_first_search(g, (vpe::Graph::vertex_descriptor) i, boost::visitor(vis));
    }

    // Merge pixel index_rectangles in each cluster using the sorted order
    // and simplify the resulting polygon to reduce points
    i = 0;
    std::for_each(merge_order_for_clustering.begin(), merge_order_for_clustering.end(),
                  [&](const std::vector<int> &order) {
                      std::vector<vpe::PixelsRing> cluster_merged_output(1);
                      std::for_each(order.begin(), order.end(), [&](int j) {
                          if (boost::geometry::is_empty(cluster_merged_output[0])) {
                              cluster_merged_output[0] = ground_pixel_rings[j];
                          } else {
                              std::vector<vpe::PixelsRing> merged_output;
                              boost::geometry::union_(ground_pixel_rings[j],
                                                      cluster_merged_output[0],
                                                      merged_output);
                              cluster_merged_output[0] = merged_output[0];
                          }
                      });
                      vpe::PixelsRing simplified_output;
                      boost::geometry::simplify(cluster_merged_output[0],
                                                simplified_output,
                                                1);
                      merged_ground_pixel_polygons[i] = simplified_output;
                      ++i;
                  });

    debug_output_writer.WriteMergedGroundPixelPolygon(merged_ground_pixel_polygons, frame_number);

    // Convert pixel polygon into points polygon
    i = 0;
    std::for_each(merged_ground_pixel_polygons.begin(),
                  merged_ground_pixel_polygons.end(),
                  [&](const vpe::PixelsRing &pixels_ring) {
                      vpe::PointsRing points_ring;
                      std::for_each(pixels_ring.begin(), pixels_ring.end(), [&](const vpe::PixelXY &pixel) {
                          int row = pixel.y() / vpe::step;
                          int column = pixel.x() / vpe::step;
                          int index_in_measured_points = (row * (vpe::grid_length_width + 1)) + column;
                          points_ring.push_back(vpe::PointXYZ({measured_points[index_in_measured_points].x,
                                                               measured_points[index_in_measured_points].y,
                                                               measured_points[index_in_measured_points].z}));
                      });
                      ground_polygons[i] = points_ring;
                      ++i;
                  });

    debug_output_writer.WriteGroundPolygon(ground_polygons, frame_number);

    auto compute_ground_polygon_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - compute_ground_polygon_start);
//    std::cout << "Compute ground polygon (ms) : " << compute_ground_polygon_duration.count() << std::endl;

    return ground_polygons;
}


