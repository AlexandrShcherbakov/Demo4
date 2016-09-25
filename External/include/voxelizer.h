#ifndef EXT_TRIBOXOVERLAP_GUARDIAN
#include "EXT_TriBoxOverlap.h"
#endif

#ifndef VOXELIZER_H
#define VOXELIZER_H

#include "LiteMath.h"
#include <map>
#include <set>
#include <vector>

class MyTriangle {
    public:
        float A[3], B[3], C[3];
        MyTriangle(float3 A, float3 B, float3 C) {
            this->A[0] = A.x;
            this->A[1] = A.y;
            this->A[2] = A.z;
            this->B[0] = B.x;
            this->B[1] = B.y;
            this->B[2] = B.z;
            this->C[0] = C.x;
            this->C[1] = C.y;
            this->C[2] = C.z;
        }

        MyTriangle(vec4 A, vec4 B, vec4 C) {
            this->A[0] = A.x;
            this->A[1] = A.y;
            this->A[2] = A.z;
            this->B[0] = B.x;
            this->B[1] = B.y;
            this->B[2] = B.z;
            this->C[0] = C.x;
            this->C[1] = C.y;
            this->C[2] = C.z;
        }
};

class AABB3f {
    public:
        float3 vmax, vmin;
        float3 center() const {
            return (vmax + vmin) * 0.5f;
        }
        void include(const float3& A) {
            vmax.x = std::max(vmax.x, A.x);
            vmax.y = std::max(vmax.y, A.y);
            vmax.z = std::max(vmax.z, A.z);
            vmin.x = std::min(vmin.x, A.x);
            vmin.y = std::min(vmin.y, A.y);
            vmin.z = std::min(vmin.z, A.z);
        }

        void include(const vec4& A) {
            include(float3(A.x, A.y, A.z));
        }
};

struct int3 {
    int x, y, z;
    int3(int x, int y, int z) : x(x), y(y), z(z) {}
};

inline static bool triBoxOverlap(const MyTriangle& tri, const AABB3f& box)
{
  float3 center = box.center();
  float3 half_size = (box.vmax - box.vmin)*0.5f;

  float triverts[3][3];
  for (int i = 0; i<3; i++)
  {
    triverts[0][i] = tri.A[i];
    triverts[1][i] = tri.B[i];
    triverts[2][i] = tri.C[i];
  }

  return EXTERNAL_TOOLS::triBoxOverlap((float*)&center, (float*)&half_size, triverts);
}

inline static bool boxBoxOverlap(const AABB3f& box, const AABB3f& box2)
{
  bool notOverlapX = (box.vmax.x > box2.vmax.x) && (box2.vmax.x < box.vmin.x) || (box.vmax.x < box2.vmin.x);
  bool notOverlapY = (box.vmax.y > box2.vmax.y) && (box2.vmax.y < box.vmin.y) || (box.vmax.y < box2.vmin.y);
  bool notOverlapZ = (box.vmax.z > box2.vmax.z) && (box2.vmax.z < box.vmin.z) || (box.vmax.z < box2.vmin.z);

  return !(notOverlapX || notOverlapY || notOverlapZ);
}

class VoxelTree {
    private:
        std::map<int, std::map<int, std::map<int, int> > > tree;

	public:
	int maxX = 0, maxY = 0, maxZ = 0;
	int minX = 0, minY = 0, minZ = 0;

	int& operator()(int x, int y, int z) {
        maxX = std::max(maxX, x);
        minX = std::min(minX, x);
        maxY = std::max(maxY, y);
        minY = std::min(minY, y);
        maxZ = std::max(maxZ, z);
        minZ = std::min(minZ, z);
        return tree[x][y][z];
    }

    void onlyGoodFaces() {
    	uint c = 0;
		std::map<int, std::map<int, std::set<int> > > badVoxels;
        for (auto px = tree.begin(); px != tree.end(); ++px) {
            for (auto py = px->second.begin(); py != px->second.end(); ++py) {
                for (auto pz = py->second.begin(); pz != py->second.end(); ++pz) {
                    int x = px->first, y = py->first, z = pz->first;
                    if (x > 0 && tree.find(x - 1) != tree.end() && tree[x - 1][y][z]) {
                        tree[x    ][y][z] &= ~(1 << 4);
                        tree[x - 1][y][z] &= ~(1 << 5);
                    }
                    if (y > 0 && tree[x].find(y - 1) != tree[x].end() && tree[x][y - 1][z]) {
                        tree[x][y    ][z] &= ~(1 << 2);
                        tree[x][y - 1][z] &= ~(1 << 3);
                    }
                    if (z > 0 && tree[x][y].find(z - 1) != tree[x][y].end() && tree[x][y][z - 1]) {
                        tree[x][y][z    ] &= ~(1 << 0);
                        tree[x][y][z - 1] &= ~(1 << 1);
                    }
                }
            }
        }
		for (auto px = badVoxels.begin(); px != badVoxels.end(); ++px) {
			int x = px->first;
            for (auto py = px->second.begin(); py != px->second.end(); ++py) {
				int y = py->first;
                for (auto pz = py->second.begin(); pz != py->second.end(); ++pz) {
					int z = *pz;
                    tree[x][y][z] = 0;
                }
            }
        }
    }

    int sizeX() {
    	return maxX - minX;
    }
    int sizeY() {
		return maxY - minY;
    }
    int sizeZ() {
        return maxZ - minZ;
    }
};

template<typename Data, typename ContainerT>
void RasterizeTriangleTo3DArray(const float3& A, const float3& B, const float3& C,
                                AABB3f box, float voxelSize,
                                ContainerT& voxels, Data initVal)
{

  AABB3f triBox;
  triBox.include(A);
  triBox.include(B);
  triBox.include(C);

  MyTriangle tri(A, B, C);

  float3 t1 = (triBox.vmin - box.vmin) / (box.vmax - box.vmin);
  float3 t2 = (triBox.vmax - box.vmin) / (box.vmax - box.vmin);

  float3   startVoxelf = t1*float3(voxels.sizeX(), voxels.sizeY(), voxels.sizeZ());
  float3   endVoxelf   = t2*float3(voxels.sizeX(), voxels.sizeY(), voxels.sizeZ());

  int3     startVoxel = int3(startVoxelf.x, startVoxelf.y, startVoxelf.z);
  int3     endVoxel   = int3(endVoxelf.x + 0.5f, endVoxelf.y + 0.5f, endVoxelf.z + 0.5f);

  for (int x = startVoxel.x; x <= endVoxel.x; x++)
  {
    for (int y = startVoxel.y; y <= endVoxel.y; y++)
    {
      for (int z = startVoxel.z; z <= endVoxel.z; z++)
      {
        AABB3f voxelBox;

        voxelBox.vmin = float3(box.vmin.x + x*voxelSize, box.vmin.y + y*voxelSize, box.vmin.z + z*voxelSize);
        voxelBox.vmax = voxelBox.vmin + 1.0f*float3(voxelSize, voxelSize, voxelSize);

        if (boxBoxOverlap(triBox, voxelBox))
          if (triBoxOverlap(tri, voxelBox))
            voxels(x, y, z) = initVal;
      }
    }
  }

}

/*std::vector<std::vector<vec4> > GenPatches(VoxelTree tree, vec3 bottom, vec3 top, int vCnt) {
    float dx = (top.x - bottom.x) / vCnt;
    float dy = (top.y - bottom.y) / vCnt;
    float dz = (top.z - bottom.z) / vCnt;
    std::vector<std::vector<vec4> > res;
    for (int x = tree.minX; x <= tree.maxX; ++x) {
        for (int y = tree.minY; y <= tree.maxY; ++y) {
            for (int z = tree.minZ; z <= tree.maxZ; ++z) {
                if (tree(x, y, z) & 1) {
                    std::vector<vec4> poly;
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * y, bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * y, bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * (y + 1), bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * (y + 1), bottom.z + dz * z, 1));
                    res.push_back(poly);
                }
                if (tree(x, y, z) & (1 << 1)) {
					std::vector<vec4> poly;
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * y, bottom.z + dz * (z + 1), 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * y, bottom.z + dz * (z + 1), 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * (y + 1), bottom.z + dz * (z + 1), 1));
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * (y + 1), bottom.z + dz * (z + 1), 1));
                    res.push_back(poly);
                }
                if (tree(x, y, z) & (1 << 2)) {
					std::vector<vec4> poly;
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * y, bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * y, bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * y, bottom.z + dz * (z + 1), 1));
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * y, bottom.z + dz * (z + 1), 1));
                    res.push_back(poly);
                }
                if (tree(x, y, z) & (1 << 3)) {
					std::vector<vec4> poly;
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * (y + 1), bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * (y + 1), bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * (y + 1), bottom.z + dz * (z + 1), 1));
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * (y + 1), bottom.z + dz * (z + 1), 1));
                    res.push_back(poly);
                }
                if (tree(x, y, z) & (1 << 4)) {
					std::vector<vec4> poly;
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * y, bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * (y + 1), bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * (y + 1), bottom.z + dz * (z + 1), 1));
                    poly.push_back(vec4(bottom.x + dx * x, bottom.y + dy * y, bottom.z + dz * (z + 1), 1));
                    res.push_back(poly);
                }
                if (tree(x, y, z) & (1 << 5)) {
					std::vector<vec4> poly;
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * y, bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * (y + 1), bottom.z + dz * z, 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * (y + 1), bottom.z + dz * (z + 1), 1));
                    poly.push_back(vec4(bottom.x + dx * (x + 1), bottom.y + dy * y, bottom.z + dz * (z + 1), 1));
                    res.push_back(poly);
                }
            }
        }
    }
    return res;
}*/

/*std::vector<vec4> Voxelize(std::vector<vec4>& triangles) {
    float3 bottom(1.0f / VEC_EPS, 1.0f / VEC_EPS, 1.0f / VEC_EPS);
    float3 top(-1.0f / VEC_EPS, -1.0f / VEC_EPS, -1.0f / VEC_EPS);
    for (uint i = 0; i < triangles.size(); i += 3) {
        for (uint j = 0; j < 3; ++j) {
            bottom.x = std::min(bottom.x, triangles[i + j].x);
            bottom.y = std::min(bottom.y, triangles[i + j].y);
            bottom.z = std::min(bottom.z, triangles[i + j].z);
            top.x = std::max(top.x, triangles[i + j].x);
            top.y = std::max(top.y, triangles[i + j].y);
            top.z = std::max(top.z, triangles[i + j].z);
        }
    }
    std::cout << top.x << ' ' << top.y << ' ' << top.z << std::endl;
    std::cout << bottom.x << ' ' << bottom.y << ' ' << bottom.z << std::endl;
    VoxelTree tree;
    AABB3f box;
    top.z = top.y = top.x = std::max(top.x, std::max(top.y, top.z));
    bottom.z = bottom.y = bottom.x = std::min(bottom.x, std::min(bottom.y, bottom.z));
    box.vmax = top + float3(VEC_EPS, VEC_EPS, VEC_EPS);
    box.vmin = bottom - float3(VEC_EPS, VEC_EPS, VEC_EPS);
    int Cnt = 60;
    float voxelSize = std::min(std::min(box.vmax.x - box.vmin.x, box.vmax.y - box.vmin.y), box.vmax.z - box.vmin.z) / Cnt;
    tree(Cnt, Cnt, Cnt);
    for (uint i = 0; i < triangles.size(); i += 3) {
        float3 A(triangles[i + 0].x, triangles[i + 0].y, triangles[i + 0].z);
		float3 B(triangles[i + 1].x, triangles[i + 1].y, triangles[i + 1].z);
		float3 C(triangles[i + 2].x, triangles[i + 2].y, triangles[i + 2].z);
        RasterizeTriangleTo3DArray(A, B, C, box, voxelSize, tree, 63);
    }
    tree.onlyGoodFaces();
    std::vector<vec4> res;
    for (auto p: GenPatches(tree, vec3(box.vmin.x, box.vmin.y, box.vmin.z), vec3(box.vmax.x, box.vmax.y, box.vmax.z), Cnt)) {
        for (auto i: p)
            res.push_back(i);
    }
    std::cout << "Patches count " << res.size() / 4 << std::endl;
    return res;
}*/

#endif // VOXELIZER_H
