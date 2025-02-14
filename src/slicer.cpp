#include "slicer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

using namespace std;

unique_ptr<Slicer> Slicer::createSlicer(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Не удалось открыть файл" << endl;
        return nullptr;
    }

    string objectType;
    file >> objectType;

    unique_ptr<Slicer> slicer;
    if (objectType == "куб") {
        double side;
        file >> side;
        slicer = make_unique<Slicer>();
        slicer->setSize(side);
    } else if (objectType == "прямоугольник") {
        double length, width, height;
        file >> length >> width >> height;
        slicer = make_unique<Slicer>();
        slicer->setSize(length, width, height);
    } else {
        cout << "Неизвестный тип объекта" << endl;
    }

    file.close();
    return slicer;
}

void Slicer::setSize(double size) {
    this->object_size = {size, size, size};
    cout << "Размер куба: " << size << endl;
}

void Slicer::setSize(double length, double width, double height) {
    this->object_size = {length, width, height};
    cout << "Размеры прямоугольника: " << length << ", " << width << ", " << height << endl;
}

void Slicer::Slice() {
    
}

void Slicer::updateGCODE() {

}