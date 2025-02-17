#ifndef SLICER_HPP
#define SLICER_HPP

#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <memory>

class Slicer {
public:
    Slicer(
        const std::string& filename = "input.txt", 
        double layer_height = 0.02,
        double extrusion_width = 0.4,
        int number_of_perimeters = 2);
    // std::vector<double> object_size = std::vector<double>(3, 10.0);
    std::string objectType = "";
    double layer_height = 0.2;
    double extrusion_width = 0.3;
    int number_of_perimeters = 2;

    void Slice();

private:
    struct Vertex {
        double x, y, z;
    };
    struct Triangle {
        std::vector<Vertex> v = std::vector<Vertex>(3, {0, 0, 0});
        std::vector<double> normal = std::vector<double>(3, 0.0);
    };

    double obj_height = 0.0;
    std::vector<Triangle> side_triangles = std::vector<Triangle> {};
    std::vector<Triangle> plain_triangles = std::vector<Triangle> {};

    void calc_cube_triangles(double length, double width, double height);
    bool is_point_on_side(Vertex point, std::pair<Vertex, Vertex> side, double height);
    Vertex find_intersection_point(std::pair<Vertex, Vertex> edge, double height);
    std::vector<Vertex> find_cross_points(Triangle triangle, double height);
    std::vector<Vertex> shift_edge(Vertex vertex1, Vertex vertex2, double step);
    Vertex calc_edge_intersection(std::vector<Vertex> edge1, std::vector<Vertex> edge2);

    std::vector<Vertex> calc_external_contour(double height);
    std::vector<Vertex> calc_internal_contour(std::vector<Vertex> external_contour);
    std::vector<Vertex> calc_fill_plate(std::vector<Vertex> internal_contour);

    void updateGCODE(std::vector<std::vector<Slicer::Vertex>> sequence_of_points);
};

#endif // SLICER_HPP
