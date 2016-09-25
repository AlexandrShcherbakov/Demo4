#include "Octree.h"
#include <fstream>

using namespace VM;

Octree::Octree() {};

Octree::Octree(
    const vec4& min_point,
    const vec4& max_point,
    const uint side) {
    depth = 0;
    while ((1u << depth) < side)
        depth++;
    vec4 cube = max_point - min_point;
    float diameter = std::max(cube.x, std::max(cube.y, cube.z));
    diameter = diameter / side * (1 << depth);
    this->min_point = min_point;
    this->max_point = min_point + vec4(diameter, diameter, diameter, 0);
    this->depth = depth;
    this->leaf = true;
}

TexturedPolygon StripPolygonByPlane(
    TexturedPolygon& polygon,
    const vec4& plane) {
    TexturedPolygon result;
    auto points = polygon.getPoints();
    auto normals = polygon.getNormals();
    auto tex_coords = polygon.getTexCoords();
    for (uint i = 0; i < polygon.getSize(); ++i) {
        uint cur_idx = i % polygon.getSize();
        uint nxt_idx = (i + 1) % polygon.getSize();
        vec4 cur_pnt = points[cur_idx];
        vec4 nxt_pnt = points[nxt_idx];
        if (dot(nxt_pnt, plane) >= 0) {
            if (dot(cur_pnt, plane) < 0) {
                vec4 v = nxt_pnt - cur_pnt;
                float t = dot(-cur_pnt, plane) / dot(v, plane);
                result.addPoint(
                    cur_pnt + v * t,
                    normals[cur_idx] + (normals[nxt_idx] - normals[cur_idx]) * t,
                    tex_coords[cur_idx] + (tex_coords[nxt_idx] - tex_coords[cur_idx]) * t);
            }
            result.addPoint(
                points[nxt_idx],
                normals[nxt_idx],
                tex_coords[nxt_idx]);
        } else if (dot(cur_pnt, plane) > 0) {
            vec4 v = nxt_pnt - cur_pnt;
            float t = dot(-cur_pnt, plane) / dot(v, plane);
            result.addPoint(
                cur_pnt + v * t,
                normals[cur_idx] + (normals[nxt_idx] - normals[cur_idx]) * t,
                tex_coords[cur_idx] + (tex_coords[nxt_idx] - tex_coords[cur_idx]) * t);
        }
    }
    result.setImage(polygon.getImage());
    result.setColor(polygon.getColor());
    result.setMaterialNumber(polygon.getMaterialNumber());
    return result;
}

TexturedPolygon StripPolygonByCube(
    const TexturedPolygon& polygon,
    const vec4& min_point,
    const vec4& max_point) {
    TexturedPolygon result = polygon;
    result = StripPolygonByPlane(result, vec4(1, 0, 0, -min_point.x));
    result = StripPolygonByPlane(result, vec4(0, 1, 0, -min_point.y));
    result = StripPolygonByPlane(result, vec4(0, 0, 1, -min_point.z));
    result = StripPolygonByPlane(result, vec4(-1, 0, 0, max_point.x));
    result = StripPolygonByPlane(result, vec4(0, -1, 0, max_point.y));
    result = StripPolygonByPlane(result, vec4(0, 0, -1, max_point.z));
    return result;
}

void Octree::addPolygon(const TexturedPolygon& polygon) {
    static std::ofstream log("log.txt");
    TexturedPolygon new_poly = StripPolygonByCube(polygon, min_point, max_point);
    if (new_poly.getSquare() < VEC_EPS / 1000.0f)
        return;
    if (!this->depth) {
        static uint globIndex = 0;
        new_poly.setIndex(globIndex);
        globIndex++;
        this->polygons.push_back(new_poly);
        return;
    }
    if (this->leaf) {
        vec4 center = (max_point - min_point) / 2 + min_point;
        this->subtrees[0] = new Octree(min_point, center, 1 << (depth - 1));
        this->subtrees[1] = new Octree(
            vec4(min_point.x, min_point.y, center.z, 1),
            vec4(center.x, center.y, max_point.z, 1), 1 << (depth - 1));
        this->subtrees[2] = new Octree(
            vec4(min_point.x, center.y, min_point.z, 1),
            vec4(center.x, max_point.y, center.z, 1), 1 << (depth - 1));
        this->subtrees[3] = new Octree(
            vec4(min_point.x, center.y, center.z, 1),
            vec4(center.x, max_point.y, max_point.z, 1), 1 << (depth - 1));
        this->subtrees[4] = new Octree(
            vec4(center.x, min_point.y, min_point.z, 1),
            vec4(max_point.x, center.y, center.z, 1), 1 << (depth - 1));
        this->subtrees[5] = new Octree(
            vec4(center.x, min_point.y, center.z, 1),
            vec4(max_point.x, center.y, max_point.z, 1), 1 << (depth - 1));
        this->subtrees[6] = new Octree(
            vec4(center.x, center.y, min_point.z, 1),
            vec4(max_point.x, max_point.y, center.z, 1), 1 << (depth - 1));
        this->subtrees[7] = new Octree(center, max_point, 1 << (depth - 1));
        this->leaf = false;
    }
    for (uint i = 0; i < 8; ++i) {
        this->subtrees[i]->addPolygon(new_poly);
    }
}

std::vector<TexturedPolygon> Octree::getPolygons() const {
    std::vector<TexturedPolygon> result = polygons;
    if (!this->leaf) {
        for (uint i = 0; i < 8; ++i) {
            auto subres = subtrees[i]->getPolygons();
            result.insert(result.end(), subres.begin(), subres.end());
        }
    }
    return result;
}

bool PointInCube(const vec4& point, const vec4& min_point, const vec4& max_point) {
    return min_point.x <= point.x
        && min_point.y <= point.y
        && min_point.z <= point.z
        && max_point.x >= point.x
        && max_point.y >= point.y
        && max_point.z >= point.z;
}

Octree* Octree::getSubtree(const vec4& point) {
    if (this->leaf) {
        if (PointInCube(point, min_point, max_point))
            return this;
        else
            return nullptr;
    }
    for (uint i = 0; i < subtrees.size(); ++i) {
        if (PointInCube(point, subtrees[i]->min_point, subtrees[i]->max_point))
            return subtrees[i]->getSubtree(point);
    }
    return nullptr;
}

bool Octree::pointInSubtree(const vec4& point) const {
    return PointInCube(point, this->min_point, this->max_point);
}

uvec3 Octree::getSubtreeIndex(const vec4& point) const {
    if (!this->depth) return uvec3(0, 0, 0);
    uvec3 shift[8] = {uvec3(0, 0, 0), uvec3(0, 0, 2), uvec3(0, 2, 0), uvec3(0, 2, 2),
                      uvec3(2, 0, 0), uvec3(2, 0, 2), uvec3(2, 2, 0), uvec3(2, 2, 2)};
    for (uint i = 0; i < 8; ++i) {
        if (subtrees[i]->pointInSubtree(point))
            return subtrees[i]->getSubtreeIndex(point) + shift[i];
    }
}

Octree* Octree::operator[](const uvec3& index) {
    if (this->leaf) return this;
    uvec3 new_index = index;
    uint inner_index = 0;
    uint index_split = 1u << (this->depth - 1u);
    if (new_index.x >= index_split) {
        new_index.x -= index_split;
        inner_index += 4;
    }
    if (new_index.y >= index_split) {
        new_index.y -= index_split;
        inner_index += 2;
    }
    if (new_index.z >= index_split) {
        new_index.z -= index_split;
        inner_index += 1;
    }
    return (*subtrees[inner_index])[new_index];
}

const Octree* Octree::operator[](const uvec3& index) const {
    if (this->leaf) return this;
    uvec3 new_index = index;
    uint inner_index = 0;
    uint index_split = 1u << (this->depth - 1u);
    if (new_index.x >= index_split) {
        new_index.x -= index_split;
        inner_index += 4;
    }
    if (new_index.y >= index_split) {
        new_index.y -= index_split;
        inner_index += 2;
    }
    if (new_index.z >= index_split) {
        new_index.z -= index_split;
        inner_index += 1;
    }
    return (*subtrees[inner_index])[new_index];
}

bool SomeToDraw(const std::vector<TexturedPolygon>& polys, const vec4& normal) {
    for (auto& poly: polys) {
        if (dot(poly.normal(), normal) > 0.0f)
            return true;
    }
    return false;
}

vec4 ColorFromOnePolygon(const TexturedPolygon& poly) {
    if (poly.getImage() != nullptr)
        return poly.getImage()->getAverageColor(poly.getTexCoords()) * poly.getColor();
    return poly.getColor() * 256.0f;
}

vec4 ColorForPatch(std::vector<TexturedPolygon>& polys, const vec4& normal) {
    vec4 color(0, 0, 0, 0);
    float square = 0.0f;
    for (auto& poly: polys) {
        float angle = dot(poly.normal(), normal);
        if (angle <= 0) continue;
        float p_square = poly.getSquare() * angle;
        square += p_square;
        color += ColorFromOnePolygon(poly) * p_square;
    }
    return color / square / 256.0f;
}

TexturedPolygon GeneratePatch(const vec4& min_point, const vec4& max_point, const vec4& normal) {
    TexturedPolygon poly;
    //s - small point coord, b - big point coord
    vec4 sss = min_point + vec4(2 * VEC_EPS, 2 * VEC_EPS, 2 * VEC_EPS, 0);
    vec4 ssb = vec4(min_point.x, min_point.y, max_point.z, 1) + vec4(2 * VEC_EPS, 2 * VEC_EPS, -2 * VEC_EPS, 0);
    vec4 sbs = vec4(min_point.x, max_point.y, min_point.z, 1) + vec4(2 * VEC_EPS, -2 * VEC_EPS, 2 * VEC_EPS, 0);
    vec4 sbb = vec4(min_point.x, max_point.y, max_point.z, 1) + vec4(2 * VEC_EPS, -2 * VEC_EPS, -2 * VEC_EPS, 0);
    vec4 bss = vec4(max_point.x, min_point.y, min_point.z, 1) + vec4(-2 * VEC_EPS, 2 * VEC_EPS, 2 * VEC_EPS, 0);
    vec4 bsb = vec4(max_point.x, min_point.y, max_point.z, 1) + vec4(-2 * VEC_EPS, 2 * VEC_EPS, -2 * VEC_EPS, 0);
    vec4 bbs = vec4(max_point.x, max_point.y, min_point.z, 1) + vec4(-2 * VEC_EPS, -2 * VEC_EPS, 2 * VEC_EPS, 0);
    vec4 bbb = max_point + vec4(-2 * VEC_EPS, -2 * VEC_EPS, -2 * VEC_EPS, 0);
    if (std::abs(normal.x) > VEC_EPS) {
        if (normal.x < -VEC_EPS) {
            poly.addPoint(sss, normal, vec2(0, 0));
            poly.addPoint(ssb, normal, vec2(0, 0));
            poly.addPoint(sbb, normal, vec2(0, 0));
            poly.addPoint(sbs, normal, vec2(0, 0));
        } else if (normal.x > VEC_EPS) {
            poly.addPoint(bbb, normal, vec2(0, 0));
            poly.addPoint(bsb, normal, vec2(0, 0));
            poly.addPoint(bss, normal, vec2(0, 0));
            poly.addPoint(bbs, normal, vec2(0, 0));
        }
    } else if (std::abs(normal.y) > VEC_EPS) {
        if (normal.y < -VEC_EPS) {
            poly.addPoint(sss, normal, vec2(0, 0));
            poly.addPoint(ssb, normal, vec2(0, 0));
            poly.addPoint(bsb, normal, vec2(0, 0));
            poly.addPoint(bss, normal, vec2(0, 0));
        }
        if (normal.y > VEC_EPS) {
            poly.addPoint(bbb, normal, vec2(0, 0));
            poly.addPoint(bbs, normal, vec2(0, 0));
            poly.addPoint(sbs, normal, vec2(0, 0));
            poly.addPoint(sbb, normal, vec2(0, 0));
        }
    } else if (std::abs(normal.z) > VEC_EPS) {
        if (normal.z < -VEC_EPS) {
            poly.addPoint(sss, normal, vec2(0, 0));
            poly.addPoint(sbs, normal, vec2(0, 0));
            poly.addPoint(bbs, normal, vec2(0, 0));
            poly.addPoint(bss, normal, vec2(0, 0));
        }
        if (normal.z > VEC_EPS) {
            poly.addPoint(bbb, normal, vec2(0, 0));
            poly.addPoint(bsb, normal, vec2(0, 0));
            poly.addPoint(ssb, normal, vec2(0, 0));
            poly.addPoint(sbb, normal, vec2(0, 0));
        }
    }
    return poly;
}

void TryToAddPatch(const Octree& scene, const uvec3& coords, Octree & patches, const vec4& normal) {
    const Octree * part_of_tree = scene[coords];
    if (part_of_tree->getPolygons().size() == 0) return;
    std::vector<TexturedPolygon> polygons = part_of_tree->getPolygons();
    if (SomeToDraw(polygons, normal)) {
        vec3 diff_coord(normal.x + (float)coords.x, normal.y + (float)coords.y, normal.z + (float)coords.z);
        bool needs_to_draw = min(diff_coord) <= -1 + VEC_EPS || max(diff_coord) >= scene.getSide() - VEC_EPS;
        if (!needs_to_draw) {
            uvec3 new_coord(coords.x + (int)normal.x, coords.y + (int)normal.y, coords.z + (int)normal.z);
            const Octree * near_cube = scene[new_coord];
            std::vector<TexturedPolygon> near_poly = near_cube->getPolygons();
            needs_to_draw = (near_poly.size() == 0);
        }
        if (needs_to_draw) {
            TexturedPolygon new_poly = GeneratePatch(part_of_tree->getMinPoint(), part_of_tree->getMaxPoint(), normal);
            new_poly.setColor(ColorForPatch(polygons, normal));
            for (auto poly: polygons) {
                float angle = dot(normal, poly.normal());
                if (angle > 0) {
                    new_poly.addRelation(poly.getIndex(), angle * poly.getSquare() / new_poly.getSquare());
                }
            }
            patches.addPolygon(new_poly);
        }
    }
}

Octree CreateVoxelTreeByScene(const Octree& scene) {
    Octree patches(scene.getMinPoint(), scene.getMaxPoint(), scene.getSide());
    for (uint x = 0; x < scene.getSide(); ++x) {
        for (uint y = 0; y < scene.getSide(); ++y) {
            for (uint z = 0; z < scene.getSide(); ++z) {
                TryToAddPatch(scene, uvec3(x, y, z), patches, vec4(-1,  0,  0, 0));
                TryToAddPatch(scene, uvec3(x, y, z), patches, vec4( 1,  0,  0, 0));
                TryToAddPatch(scene, uvec3(x, y, z), patches, vec4( 0, -1,  0, 0));
                TryToAddPatch(scene, uvec3(x, y, z), patches, vec4( 0,  1,  0, 0));
                TryToAddPatch(scene, uvec3(x, y, z), patches, vec4( 0,  0, -1, 0));
                TryToAddPatch(scene, uvec3(x, y, z), patches, vec4( 0,  0,  1, 0));
            }
        }
    }
    return patches;
}


uint Octree::getNodesCount() const {
    uint res = 1;
    if (!this->leaf) {
        for (uint i = 0; i < 8; ++i)
            res += this->subtrees[i]->getNodesCount();
    }
    return res;
}

bool LinesIntersection(
    const float& l1, const float& r1,
    const float& l2, const float& r2) {
    return l1 <= l2 && l2 <= r1
        || l1 <= r2 && r2 <= r1
        || l2 <= l1 && l1 <= r2
        || l2 <= r1 && r1 <= r2;
}

bool CubesIntersection(
    const vec4& min1, const vec4& max1,
    const vec4& min2, const vec4& max2) {
    return LinesIntersection(min1.x, max1.x, min2.x, max2.x)
        && LinesIntersection(min1.y, max1.y, min2.y, max2.y)
        && LinesIntersection(min1.z, max1.z, min2.z, max2.z);
}

std::vector<TexturedPolygon> Octree::getPatchesFromCube(
    const vec4& min_point,
    const vec4& max_point) const {
    if (!CubesIntersection(this->min_point, this->max_point, min_point, max_point))
        return std::vector<TexturedPolygon>();
    if (this->leaf) return this->polygons;
    std::vector<TexturedPolygon> result;
    for (Octree * subtree: this->subtrees) {
        std::vector<TexturedPolygon> polys = subtree->getPatchesFromCube(min_point, max_point);
        result.insert(result.end(), polys.begin(), polys.end());
    }
    return result;
}
