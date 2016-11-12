#include "Octree.h"

using namespace VM;
using namespace std;

OctreeWithPatches::OctreeWithPatches(OctreeWithTriangles& octree) {
	//octree.Root.ReorganizeTriangles();
	uint index = 0;
	octree.Root.SetIndices(index);
    Root.CreateFromTriangles(octree.Root, octree.Root, uvec3(0, 0, 0), octree.Side << 1);
}
