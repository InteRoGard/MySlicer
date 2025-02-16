#include "slicer.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
// #include <libstl/stl.h>
// #include <memory>

using namespace std;

Slicer::Slicer(
    const std::string& filename, 
    double layer_height,
    double extrusion_width4,
    int number_of_perimeters)
{
    ifstream file(filename);
    if (file.is_open()) {
        getline(file, Slicer::objectType);
        if (Slicer::objectType == "куб") {
            double side;
            file >> side;
            Slicer::obj_height = side;
            Slicer::calc_cube_triangles(side, side, side);
        } else if (Slicer::objectType == "параллелепипед") {
            double length, width, height;
            file >> length >> width >> height;
            Slicer::obj_height = height;
            Slicer::calc_cube_triangles(length, width, height);
        } else {
            cout << "Неизвестный тип объекта" << endl;
        }
    }
    file.close();
}

void Slicer::calc_cube_triangles(double length, double width, double height) {
    cout << "Размеры параллелепипеда: " << length << " x " << width << " x " << height << endl;
    std::vector<Vertex> vert = {
        {0,         0,      0},
        {length,    0,      0},
        {length,    width,  0},
        {0,         width,  0},
        {0,         0,      height},
        {length,    0,      height},
        {length,    width,  height},
        {0,         width,  height}
    };
    for (int i = 0; i < 4; i++) {
        vector<Slicer::Vertex> vert3 = vector<Slicer::Vertex>{3, {0, 0, 0}};
        if (i < 3) {
            vector<Slicer::Vertex> vert3 = {vert[i], vert[i+1], vert[i+3], vert[i+4]};
        } else if (i == 3) {
            vector<Slicer::Vertex> vert3 = {vert[i], vert[i-3], vert[i+1], vert[i+4]};
        }
        Slicer::Triangle triangle1, triangle2;
        for (int j = 0; j < 4; j++) {
            triangle1.v[j] = vert3[j];
            triangle2.v[j] = vert3[j+1];
        }
        Slicer::side_triangles.push_back(triangle1);
        Slicer::side_triangles.push_back(triangle2);
    }
    for (int i = 0; i < 2; i++) {
        int k = i * 2;
        Slicer::Triangle triangle1, triangle2;
        triangle1.v = {vert[k], vert[k+1], vert[k+2]};
        triangle2.v = {vert[k], vert[k+2], vert[k+3]};
        Slicer::plain_triangles.push_back(triangle1);
        Slicer::plain_triangles.push_back(triangle2);
    }
}

bool Slicer::is_point_on_side(Slicer::Vertex point, pair<Slicer::Vertex, Slicer::Vertex> side, double height) {
    Slicer::Vertex V1 = side.first;
    Slicer::Vertex V2 = side.second;
    return min(V1.x, V2.x) <= point.x && point.x <= max(V1.x, V2.x) &&
        min(V1.y, V2.y) <= point.y && point.y <= max(V1.y, V2.y) &&
        point.z == height;
}

Slicer::Vertex Slicer::find_intersection_point(pair<Slicer::Vertex, Slicer::Vertex> edge, double height) {
    Vertex V1 = edge.first;
    Vertex V2 = edge.second;
    double x = V1.x + (V2.x - V1.x) * (height - V1.z) / (V2.z - V1.z);
    double y = V1.y + (V2.y - V1.y) * (height - V1.z) / (V2.z - V1.z);
    return {x, y, height};
}

vector<Slicer::Vertex> Slicer::find_cross_points(Slicer::Triangle triangle, double height) {
    if ((triangle.v[0].z > height && triangle.v[1].z > height && triangle.v[2].z > height) || 
        (triangle.v[0].z < height && triangle.v[1].z < height && triangle.v[2].z < height)) {
        return {};
    }
    vector<Slicer::Vertex> cross_points;
    for (auto side: {make_pair(triangle.v[0], triangle.v[1]),
                    make_pair(triangle.v[1], triangle.v[2]),
                    make_pair(triangle.v[0], triangle.v[2])}) {
        Slicer::Vertex intersection_point = Slicer::find_intersection_point(side, height);
        if (Slicer::is_point_on_side(intersection_point, side, height)) {
            cross_points.push_back(intersection_point);
        }
    }
    return cross_points;
}

vector<Slicer::Vertex> Slicer::calc_external_contour(double height){

}

vector<Slicer::Vertex> Slicer::calc_internal_contour(vector<Slicer::Vertex> external_contour, double height){

}

vector<Slicer::Vertex> Slicer::fill_plate(vector<Slicer::Vertex> internal_contour){
    
}

void Slicer::updateGCODE(vector<vector<Slicer::Vertex>> sequence_of_points) {

}

void Slicer::Slice() {
    double obj_h = Slicer::obj_height;
    double layer_h = Slicer::layer_height;
    int num_perimeters = Slicer::number_of_perimeters;

    for (double h = 0; h < obj_h; h += layer_h) {
        vector<vector<Slicer::Vertex>> sequence_of_points;
        vector<Slicer::Vertex> external_contour = Slicer::calc_external_contour(h);
        sequence_of_points.push_back(external_contour);
        
        for (int contour_count = 0; contour_count < num_perimeters-1; contour_count++) {
            vector<Slicer::Vertex> internal_contour = Slicer::calc_internal_contour;
            sequence_of_points.push_back(internal_contour);
        }
        if ((h == 0) || (h == obj_h - layer_h)) {
            vector<Slicer::Vertex> fill_contour = Slicer::fill_plate();
            sequence_of_points.push_back(fill_contour);
        }
        Slicer::updateGCODE(sequence_of_points);
    }
}