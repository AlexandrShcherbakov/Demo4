#include "Cube.h"

using namespace VM;
using namespace std;

Cube::Cube()
{
	MinPoint = vec4(0, 0, 0, 1);
	MaxPoint = vec4(1, 1, 1, 1);
	Subnodes = nullptr;
}

Cube::Cube(const VM::vec4& minPoint, const VM::vec4& maxPoint) {
    MinPoint = minPoint;
    MaxPoint = maxPoint;
    Subnodes = nullptr;
}

bool Cube::IntersectsWithVolume(const Volume* volume) {
    return volume->IntersectsWithCube(MinPoint, MaxPoint);
}

class PolygonForTriangles {
public:
	PolygonForTriangles() {};
    PolygonForTriangles(const Triangle& triangle) {
        Points = vector<vec4>(triangle.PointsBegin(), triangle.PointsEnd());
        Normals = vector<vec4>(triangle.NormalsBegin(), triangle.NormalsEnd());
        TexCoords = vector<vec2>(triangle.TexCoordsBegin(), triangle.TexCoordsEnd());
    }

    void AddPoint(const vec4& point, const vec4& normal, const vec2& texCoord) {
        Points.push_back(point);
        Normals.push_back(normal);
        TexCoords.push_back(texCoord);
    }

    float Square() const {
        float square = 0;
        for (uint i = 2; i < Points.size(); ++i) {
            square += length(cross(Points[i] - Points[0], Points[i - 1] - Points[0]));
        }
        return square / 2;
    }

    vector<vec4> Points;
    vector<vec4> Normals;
    vector<vec2> TexCoords;
};

PolygonForTriangles StripPolygonByPlane(
    const PolygonForTriangles& polygon,
    const vec4& plane)
{
    PolygonForTriangles result;
    auto points = polygon.Points;
    auto normals = polygon.Normals;
    auto tex_coords = polygon.TexCoords;
    for (uint i = 0; i < points.size(); ++i) {
        uint cur_idx = i % points.size();
        uint nxt_idx = (i + 1) % points.size();
        vec4 cur_pnt = points[cur_idx];
        vec4 nxt_pnt = points[nxt_idx];
        if (dot(nxt_pnt, plane) >= 0) {
            if (dot(cur_pnt, plane) < 0) {
                vec4 v = nxt_pnt - cur_pnt;
                float t = dot(-cur_pnt, plane) / dot(v, plane);
                result.AddPoint(
                    cur_pnt + v * t,
                    normals[cur_idx] + (normals[nxt_idx] - normals[cur_idx]) * t,
                    tex_coords[cur_idx] + (tex_coords[nxt_idx] - tex_coords[cur_idx]) * t);
            }
            result.AddPoint(
                points[nxt_idx],
                normals[nxt_idx],
                tex_coords[nxt_idx]);
        } else if (dot(cur_pnt, plane) > 0) {
            vec4 v = nxt_pnt - cur_pnt;
            float t = dot(-cur_pnt, plane) / dot(v, plane);
            result.AddPoint(
                cur_pnt + v * t,
                normals[cur_idx] + (normals[nxt_idx] - normals[cur_idx]) * t,
                tex_coords[cur_idx] + (tex_coords[nxt_idx] - tex_coords[cur_idx]) * t);
        }
    }
    return result;
}

PolygonForTriangles StripPolygonByCube(
    const PolygonForTriangles& polygon,
    const vec4& min_point,
    const vec4& max_point)
{
    PolygonForTriangles result = polygon;
    result = StripPolygonByPlane(result, vec4(1, 0, 0, -min_point.x));
    result = StripPolygonByPlane(result, vec4(0, 1, 0, -min_point.y));
    result = StripPolygonByPlane(result, vec4(0, 0, 1, -min_point.z));
    result = StripPolygonByPlane(result, vec4(-1, 0, 0, max_point.x));
    result = StripPolygonByPlane(result, vec4(0, -1, 0, max_point.y));
    result = StripPolygonByPlane(result, vec4(0, 0, -1, max_point.z));
    return result;
}

vector<Triangle> Cube::PartsInCube(const Triangle& triangle) const {
    PolygonForTriangles polygonInCube = StripPolygonByCube(
		PolygonForTriangles(triangle),
        MinPoint,
		MaxPoint
	);
    vector<Triangle> triangles;
    if (polygonInCube.Square() < VEC_EPS) {
        return triangles;
    }
    for (uint i = 2; i < polygonInCube.Points.size(); ++i) {
        Triangle smallTriangle;
        array<vec4, 3> points = {
        	polygonInCube.Points[0],
        	polygonInCube.Points[i - 1],
        	polygonInCube.Points[i]
		};
        array<vec4, 3> normals = {
            polygonInCube.Normals[0],
            polygonInCube.Normals[i - 1],
            polygonInCube.Normals[i]
        };
        array<vec2, 3> texCoords {
            polygonInCube.TexCoords[0],
            polygonInCube.TexCoords[i - 1],
            polygonInCube.TexCoords[i]
        };
        smallTriangle.SetPoints(points.begin(), normals.begin(), texCoords.begin());
        smallTriangle.InheritParametersFrom(triangle);
        if (smallTriangle.GetSquare() > VEC_EPS) {
			triangles.push_back(smallTriangle);
        }
    }
    return triangles;
}

bool Between(const vec4& point, const vec4& minPoint, const vec4& maxPoint) {
    bool result = true;
    for (uint i = 0; i < 3 && result; ++i) {
        result = minPoint[i] <= point[i] && maxPoint[i] >= point[i];
    }
    return result;
}

bool Cube::ContainsPatch(const Patch& patch) const {
    Between((patch.Points[0] + patch.Points[2]) / 2, MinPoint, MaxPoint);
}
