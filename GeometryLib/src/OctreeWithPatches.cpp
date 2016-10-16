#include "Octree.h"

using namespace VM;

OctreeWithPatches::OctreeWithPatches(const OctreeWithTriangles& octree) {
    Root.CreateFromTriangles(octree.Root, octree.Root, uvec3(0, 0, 0), octree.Side << 1);
}
