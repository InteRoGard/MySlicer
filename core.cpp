#include "core.hpp"
#include "src/slicer.cpp"

#include <iostream>
#include <memory>

using namespace std;

int main()
{
    auto slicer = make_unique<Slicer>("input.txt");
    slicer->Slice();
    

    return 0;
}
