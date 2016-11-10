#include "Octree.h"

using namespace VM;
using namespace std;

OctreeWithPatches::OctreeWithPatches(OctreeWithTriangles& octree) {
	octree.Root.ReorganizeTriangles();
    Root.CreateFromTriangles(octree.Root, octree.Root, uvec3(0, 0, 0), octree.Side << 1);
}
