#ifndef SLICER_HPP
#define SLICER_HPP

#include <memory>
#include <string>
#include <vector>
#include <cmath>

class Slicer {
public:
    Slicer(const std::string& filename = "input.txt", 
        double layer_height = 0.02,
        double extrusion_width = 0.4,
        int number_of_perimeters = 2);
    
    void setSize(double size);
    void setSize(double length, double width, double height);
    
    std::vector<double> object_size = std::vector<double>(3, 0.0);
    std::string objectType;
    double layer_height;
    double extrusion_width;
    int number_of_perimeters;

private:
    struct Point {
        double x, y, z;
    };
    struct Triangle {
        Point A, B, C;
    };

    bool is_point_on_side(Point point, std::pair<Point, Point> side, double height);
    Point find_intersection_point(std::pair<Point, Point> edge, double height);
    std::vector<Point> find_cross_points(Triangle triangle, double height);
    void Slice();
    void updateGCODE();

};

#endif // SLICER_HPP
