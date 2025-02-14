#ifndef SLICER_HPP
#define SLICER_HPP

#include <memory>
#include <string>
#include <vector>

class Slicer {
public:
    static std::unique_ptr<Slicer> createSlicer(const std::string& filename);
    
    virtual void setSize(double size) = 0;
    virtual void setSize(double length, double width, double height) = 0;

    void Slice();
    void updateGCODE();

    std::vector<double> object_size = std::vector<double>(3, 0.0);
};

#endif // SLICER_HPP
