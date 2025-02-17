#include "core.hpp"
#include "src/slicer.cpp"

#include <iostream>

using namespace std;

int main()
{
    Slicer slicer("input.txt");
    slicer.Slice();
    return 0;
}
