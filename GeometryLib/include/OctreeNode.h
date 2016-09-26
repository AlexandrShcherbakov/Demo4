#ifndef OCTREENODE_H
#define OCTREENODE_H

#include <array>

#include "Utility.h"

#include "Cube.h"

class OctreeNode : public Cube {
private:
    std::array<Cube, 8> Nodes;
    uint Depth;
};

#endif // OCTREENODE_H
