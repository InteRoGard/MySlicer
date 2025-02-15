#include "slicer.hpp"

#include <iostream>
#include <fstream>
#include <string>
// #include <memory>

using namespace std;

Slicer::Slicer(const string& filename,
    double layer_height,
    double extrusion_width,
    int number_of_perimeters)
{
    ifstream file(filename);
    if (file.is_open())
    {
        getline(file, objectType);
        // unique_ptr<Slicer> slicer;
        if (objectType == "куб")
        {
            double side;
            file >> side;
            // slicer = make_unique<Slicer>();
            this->setSize(side);
        }
        else if (objectType == "прямоугольник")
        {
            double length, width, height;
            file >> length >> width >> height;
            // slicer = make_unique<Slicer>();
            this->setSize(length, width, height);
        }
        else
        {
            cout << "Неизвестный тип объекта" << endl;
        }
    }

    file.close();
}

void Slicer::setSize(double size) {
    this->object_size = {size, size, size};
    cout << "Размер куба: " << size << endl;
}

void Slicer::setSize(double length, double width, double height) {
    this->object_size = {length, width, height};
    cout << "Размеры прямоугольника: " << length << " x " << width << " x " << height << endl;
}

bool Slicer::is_point_on_side(Slicer::Point point, std::pair<Slicer::Point, Slicer::Point> side, double height) {
    Slicer::Point A = side.first;
    Slicer::Point B = side.second;
    return std::min(A.x, B.x) <= point.x && point.x <= std::max(A.x, B.x) &&
        std::min(A.y, B.y) <= point.y && point.y <= std::max(A.y, B.y) &&
        point.z == height;
}

Slicer::Point Slicer::find_intersection_point(std::pair<Slicer::Point, Slicer::Point> edge, double height) {
    Point A = edge.first;
    Point B = edge.second;
    double x = A.x + (B.x - A.x) * (height - A.z) / (B.z - A.z);
    double y = A.y + (B.y - A.y) * (height - A.z) / (B.z - A.z);
    return {x, y, height};
}

std::vector<Slicer::Point> Slicer::find_cross_points(Slicer::Triangle triangle, double height) {
    if ((triangle.A.z > height && triangle.B.z > height && triangle.C.z > height) || 
        (triangle.A.z < height && triangle.B.z < height && triangle.C.z < height)) {
        return {};
    }
    std::vector<Slicer::Point> cross_points;
    for (auto side: {std::make_pair(triangle.A, triangle.B),
                    std::make_pair(triangle.B, triangle.C),
                    std::make_pair(triangle.A, triangle.C)}) {
        Slicer::Point intersection_point = Slicer::find_intersection_point(side, height);
        if (Slicer::is_point_on_side(intersection_point, side, height)) {
            cross_points.push_back(intersection_point);
        }
    }
    return cross_points;
}

void Slicer::Slice() {
    
}

void Slicer::updateGCODE() {

}
