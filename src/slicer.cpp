#include "slicer.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
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
            double *side = new double;;
            file >> *side;
            Slicer::obj_height = *side;
            Slicer::calc_cube_triangles(*side, *side, *side);
        } else if (Slicer::objectType == "параллелепипед") {
            double* length = new double;
            double* width = new double;
            double* height = new double;
            file >> *length >> *width >> *height;
            Slicer::obj_height = *height;
            Slicer::calc_cube_triangles(*length, *width, *height);
        } else {
            cout << "Неизвестный тип объекта" << endl;
        }
    }
    file.close();
    // cout << "file was closed" << endl;
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
        vector<Slicer::Vertex>* vert3 = new vector<Slicer::Vertex>;
        if (i < 3) {
            *vert3 = {vert[i], vert[i+1], vert[i+5], vert[i+4]};
        } else if (i == 3) {
            *vert3 = {vert[i], vert[i-3], vert[i+1], vert[i+4]};
        }
        Slicer::Triangle* triangle1 = new Slicer::Triangle;
        Slicer::Triangle* triangle2 = new Slicer::Triangle;
        // for (int j = 0; j < 3; j++) {
        //     triangle1->v[j] = (*vert3)[j];
        //     // triangle2->v[j] = (*vert3)[j+1];
        // }
        triangle1->v[0] = (*vert3)[0];
        triangle1->v[1] = (*vert3)[1];
        triangle1->v[2] = (*vert3)[2];
        triangle2->v[0] = (*vert3)[0];
        triangle2->v[1] = (*vert3)[2];
        triangle2->v[2] = (*vert3)[3];
        Slicer::side_triangles.push_back(*triangle1);
        Slicer::side_triangles.push_back(*triangle2);
    }
    for (int i = 0; i < 2; i++) {
        int k = i * 2;
        Slicer::Triangle triangle1, triangle2;
        triangle1.v = {vert[k], vert[k+1], vert[k+2]};
        triangle2.v = {vert[k], vert[k+2], vert[k+3]};
        Slicer::plain_triangles.push_back(triangle1);
        Slicer::plain_triangles.push_back(triangle2);
    }
    // cout << "calc_cube_triangles finished" << endl;
}

bool Slicer::is_point_on_side(Slicer::Vertex point, pair<Slicer::Vertex, Slicer::Vertex> side, double height) {
    Slicer::Vertex V1 = side.first;
    Slicer::Vertex V2 = side.second;
    // cout << "is_point_on_side finished" << endl;
    return min(V1.x, V2.x) <= point.x && point.x <= max(V1.x, V2.x) &&
        min(V1.y, V2.y) <= point.y && point.y <= max(V1.y, V2.y) &&
        point.z == height;
}

Slicer::Vertex Slicer::find_intersection_point(pair<Slicer::Vertex, Slicer::Vertex> edge, double height) {
    Vertex V1 = edge.first;
    Vertex V2 = edge.second;
    double x = V1.x + (V2.x - V1.x) * (height - V1.z) / (V2.z - V1.z);
    double y = V1.y + (V2.y - V1.y) * (height - V1.z) / (V2.z - V1.z);
    // cout << "find_intersection_point finished" << endl;
    return {x, y, height};
}

vector<Slicer::Vertex> Slicer::find_cross_points(Slicer::Triangle triangle, double height) {
    if ((triangle.v[0].z > height && triangle.v[1].z > height && triangle.v[2].z > height)
    || (triangle.v[0].z < height && triangle.v[1].z < height && triangle.v[2].z < height)) {
        return {};
    }
    vector<Slicer::Vertex> cross_points;
    for (auto side: {make_pair(triangle.v[0], triangle.v[1]),
                    make_pair(triangle.v[1], triangle.v[2]),
                    make_pair(triangle.v[0], triangle.v[2])}){
        if (side.first.z != side.second.z) {
            Slicer::Vertex intersection_point = Slicer::find_intersection_point(side, height);
            if (Slicer::is_point_on_side(intersection_point, side, height)) {
                cross_points.push_back(intersection_point);
            }
        } else if ((side.first.z == side.second.z) && (side.first.z == height)) {
                cross_points.push_back(side.first);
                cross_points.push_back(side.second);
        }
    }
    // cout << "find_cross_points finished" << endl;
    return cross_points;
}

vector<Slicer::Vertex> Slicer::shift_edge(Slicer::Vertex vertex1, Slicer::Vertex vertex2, double step)
{
    vector<Slicer::Vertex> shifted_edge;
    Slicer::Vertex direction = {
        vertex2.x - vertex1.x,
        vertex2.y - vertex1.y,
        vertex2.z - vertex1.z};
        
    double length = sqrt(direction.x * direction.x 
        + direction.y * direction.y + direction.z * direction.z);

    direction.x /= length;
    direction.y /= length;
    direction.z /= length;
    Slicer::Vertex perpendicular = {-direction.y, direction.x, 0};

    length = std::sqrt(perpendicular.x * perpendicular.x 
        + perpendicular.y * perpendicular.y + perpendicular.z * perpendicular.z);
    perpendicular.x /= length;
    perpendicular.y /= length;
    perpendicular.z /= length;

    Slicer::Vertex shiftedVertex1 = {
        vertex1.x + perpendicular.x * step, 
        vertex1.y + perpendicular.y * step, 
        vertex1.z + perpendicular.z * step};

    Slicer::Vertex shiftedVertex2 = {
        vertex2.x + perpendicular.x * step, 
        vertex2.y + perpendicular.y * step, 
        vertex2.z + perpendicular.z * step};

    shifted_edge.push_back(shiftedVertex1);
    shifted_edge.push_back(shiftedVertex2);
    // cout << "shift_edge finished" << endl;
    return shifted_edge;
}

Slicer::Vertex Slicer::calc_edge_intersection(vector<Slicer::Vertex> edge1, vector<Slicer::Vertex> edge2) {
    Slicer::Vertex point1 = edge1[0];
    Slicer::Vertex point2 = edge1[1];
    Slicer::Vertex point3 = edge2[0];
    Slicer::Vertex point4 = edge2[1];

    Slicer::Vertex direction1 = {
        point2.x - point1.x,
        point2.y - point1.y,
        point2.z - point1.z};
    Slicer::Vertex direction2 = {
        point4.x - point3.x,
        point4.y - point3.y,
        point4.z - point3.z};
    Slicer::Vertex check_direction = {
        direction1.x * direction2.x,
        direction1.y * direction2.y,
        direction1.z * direction2.z};
    if ((check_direction.x == direction1.x) && (check_direction.y == direction1.y) && (check_direction.z == direction1.z)) {
        return point2;
    }
    Slicer::Vertex perp1 = {-direction1.y, direction1.x, 0};
    Slicer::Vertex perp2 = {-direction2.y, direction2.x, 0};

    double t = ((point3.x - point1.x) * perp2.x + (point3.y - point1.y) * perp2.y + (point3.z - point1.z) * perp2.z) 
        / (direction1.x * perp2.x + direction1.y * perp2.y + direction1.z * perp2.z);
    double u = ((point1.x - point3.x) * perp1.x + (point1.y - point3.y) * perp1.y + (point1.z - point3.z) * perp1.z) 
        / (direction2.x * perp1.x + direction2.y * perp1.y + direction2.z * perp1.z);

    Slicer::Vertex intersection = {
        point1.x + t * direction1.x, 
        point1.y + t * direction1.y, 
        point1.z + t * direction1.z};
    // cout << "calc_edge_intersection finished" << endl;
    return intersection;
}

vector<Slicer::Vertex> Slicer::calc_external_contour(double height){
    vector<Slicer::Vertex> external_contour;
    Slicer::Vertex buf;
    for (int i = 0; i < Slicer::side_triangles.size(); i += 2) {
        Slicer::Triangle triangle = Slicer::side_triangles[i];
        vector<Slicer::Vertex> cross_points = Slicer::find_cross_points(triangle, height);
        if (cross_points.size() < 2) {
            continue; // пропускаем треугольник, если у него нет двух точек пересечения
        }
        Slicer::Vertex a = cross_points[0];
        if (i==0) { buf = a;}
        // Slicer::Vertex b = cross_points[1];
        external_contour.push_back(a);
        // external_contour.push_back(b);
    }
    external_contour.push_back(buf);
    // cout << "calc_external_contour finished" << endl;
    return external_contour;
}

vector<Slicer::Vertex> Slicer::calc_internal_contour(vector<Slicer::Vertex> external_contour){
    vector<Slicer::Vertex> internal_contour;
    for (int i = 0; i < external_contour.size(); i++) {
        int next1, next2;
        if (i == external_contour.size()-2) {
            next1 = i + 1;
            next2 = 0;
        } else if (i == external_contour.size()-1) {
            next1 = 0;
            next2 = 1;
        } else {
            next1 = i + 1;
            next2 = i + 2;
        }
        Slicer::Vertex vertex1 = external_contour[i];
        Slicer::Vertex vertex2 = external_contour[next1];
        Slicer::Vertex vertex3 = external_contour[next2];

        vector<Slicer::Vertex> shift_edge1 = Slicer::shift_edge(vertex1, vertex2, Slicer::extrusion_width);
        vector<Slicer::Vertex> shift_edge2 = Slicer::shift_edge(vertex2, vertex3, Slicer::extrusion_width);
        Slicer::Vertex shift_vertex = Slicer::calc_edge_intersection(shift_edge1, shift_edge2);

        internal_contour.push_back(shift_vertex);
    }
    // cout << "calc_internal_contour finished" << endl;
    return internal_contour;
}

vector<Slicer::Vertex> Slicer::calc_fill_plate(vector<Slicer::Vertex> internal_contour){
    vector<Slicer::Vertex> fill_plate;
    // cout << "calc_fill_plate finished" << endl;
    return {};
}

void Slicer::updateGCODE(vector<vector<Slicer::Vertex>> sequence_of_points) {
    ofstream output("output.txt");
    if (!output.is_open()) {
        cout << "Error opening file" << endl;
        return;
    }

    double x = 0, y = 0, z = 0;
    for (int i = 0; i < sequence_of_points.size(); i++) {
        for (int j = 0; j < sequence_of_points[i].size(); j++) {
            double f = (j == 0 && i > 0) ? 200 : 100;
            output << "G01 X" << sequence_of_points[i][j].x << " Y" << sequence_of_points[i][j].y << " Z" << sequence_of_points[i][j].z << " F" << f << endl;
            x = sequence_of_points[i][j].x;
            y = sequence_of_points[i][j].y;
            z = sequence_of_points[i][j].z;
        }
    }
    output.close();
}

void Slicer::Slice() {
    double obj_h = Slicer::obj_height;
    double layer_h = Slicer::layer_height;
    int num_perimeters = Slicer::number_of_perimeters;
    vector<vector<Slicer::Vertex>> sequence_of_points;

    for (double h = 0; h < obj_h; h += layer_h) {
        // vector<Slicer::Vertex> sequence_of_points;
        vector<Slicer::Vertex> external_contour = Slicer::calc_external_contour(h);
        sequence_of_points.push_back(external_contour);
        
        // // Расчет внутреннего контура, раскомментировать:
        // for (int contour_count = 0; contour_count < num_perimeters-1; contour_count++) {
        //     vector<Slicer::Vertex> internal_contour = Slicer::calc_internal_contour(external_contour);
        //     sequence_of_points.push_back(internal_contour);
        // }
        
    }
    Slicer::updateGCODE(sequence_of_points);
    cout << "Slice finished!" << endl;
}