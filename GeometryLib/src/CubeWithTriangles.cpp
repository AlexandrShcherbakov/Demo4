#include "CubeWithTriangles.h"

using namespace std;
using namespace VM;

CubeWithTriangles::CubeWithTriangles(const VM::vec4& minPoint, const VM::vec4& maxPoint) {
    this->MaxPoint = maxPoint;
    this->MinPoint = minPoint;
    this->Triangles = vector<Triangle>();
    //cout << "TRIANGLES: " << this << endl;
}

vector<vec4> CubeWithTriangles::GetPoints() const {
    vector<vec4> points;
    for (auto& triangle: Triangles) {
        points.insert(points.end(), triangle.PointsBegin(), triangle.PointsEnd());
    }
    return points;
}

vector<vec4> CubeWithTriangles::GetNormals() const {
	vector<vec4> normals;
    for (auto& triangle: Triangles) {
        normals.insert(normals.end(), triangle.NormalsBegin(), triangle.NormalsEnd());
    }
    return normals;
}

vector<vec2> CubeWithTriangles::GetTexCoords() const {
	vector<vec2> texCoords;
    for (auto& triangle: Triangles) {
        texCoords.insert(texCoords.end(), triangle.TexCoordsBegin(), triangle.TexCoordsEnd());
    }
    return texCoords;
}

vector<uint> CubeWithTriangles::GetMaterialNumbers() const {
    vector<uint> materialNumbers;
    for (auto& triangle: Triangles) {
        for (uint i = 0; i < 3; ++i) {
            materialNumbers.push_back(triangle.GetMaterialNumber());
        }
    }
    return materialNumbers;
}

vector<vec4> CubeWithTriangles::GetAmbientColors() const {
	vector<vec4> ambientColors;
    for (auto& triangle: Triangles) {
        for (uint i = 0; i < 3; ++i) {
            ambientColors.push_back(triangle.GetAmbientColor());
        }
    }
    return ambientColors;
}

void CubeWithTriangles::SetIndices(uint& index) {
    for (uint i = 0; i < Triangles.size(); ++i)
		Indices[i] = index++;
}

const Cube* CubeWithTriangles::operator[](const VM::uvec3& index) const {
	this->Unrealized(__FUNCTION__);
	return nullptr;
}

void CubeWithTriangles::CreateFromTriangles(
	const Cube& octree,
	const Cube& node,
	const VM::uvec3& index,
	const uint side)
{
    this->Unrealized(__FUNCTION__);
}

vector<Patch> CubeWithTriangles::GetPatches(const Volume* volume) const {
	this->Unrealized(__FUNCTION__);
	return vector<Patch>();
}

void CubeWithTriangles::RemovePatch(const std::vector<uint>& patches) {
    this->Unrealized(__FUNCTION__);
}

class SetOfPoints {
public:
    vector<vec4> Points;
    vector<vector<vec4> > Neighbors;
    vector<vec4> Normals;
    vector<vec2> TexCoords;
    uint MaterialNumber;
    vec4 AmbientColor;
    vec4 Plane;
    const GL::Image * ImagePointer;

    SetOfPoints(const Triangle& triangle) {
        Points = vector<vec4>(triangle.PointsBegin(), triangle.PointsEnd());
        Normals = vector<vec4>(triangle.NormalsBegin(), triangle.NormalsEnd());
        TexCoords = vector<vec2>(triangle.TexCoordsBegin(), triangle.TexCoordsEnd());
        MaterialNumber = triangle.MaterialNumber;
        AmbientColor = triangle.AmbientColor;
        ImagePointer = triangle.ImagePointer;
        vec3 normal = cross(Points[0] - Points[1], Points[0] - Points[2]);
        normal = normalize(normal);
        Plane = vec4(normal, dot(normal, Points[0].xyz()));
        if (Plane.w < 0) {
            Plane = Plane * -1;
        }

        Neighbors.resize(Points.size());
        for (uint i = 0; i < Points.size(); ++i) {
            uint next = (i + 1) % Points.size();
            uint prev = (i + Points.size() - 1) % Points.size();
            Neighbors[i].push_back(Points[next]);
            Neighbors[i].push_back(Points[prev]);
        }
    }

    void AddPoint(const vec4& point, const vec4& normal, const vec2 texCoord, const vector<vec4>& neighbors) {
    	uint i;
        for (i = 0; i < Points.size() && Points[i] != point; ++i) {
        }
        if (i == Points.size()) {
            Points.push_back(point);
            Normals.push_back(normal);
            TexCoords.push_back(texCoord);
            Neighbors.resize(Points.size());
        }
        for (auto neib: neighbors) {
            auto position = find(Neighbors[i].begin(), Neighbors[i].end(), neib);
            if (position != Neighbors[i].end()) {
                Neighbors[i].erase(position);
            } else {
                Neighbors[i].push_back(neib);
            }
        }

        if (Neighbors[i].empty()) {
            Points.erase(Points.begin() + i);
            Normals.erase(Normals.begin() + i);
            TexCoords.erase(TexCoords.begin() + i);
            Neighbors.erase(Neighbors.begin() + i);
        }
    }

    vector<Triangle> CreateTriangles() const {
        if (Points.size() < 3) {
            return vector<Triangle>();
        }
        vector<uint> order(1, 0);
        uint currentPoint = 0;
        for (uint i = 1; i < Points.size(); ++i) {
            for (uint j = 0; j < Neighbors[currentPoint].size(); ++j) {
                auto candidate = Neighbors[currentPoint][j];
                uint candidateIdx = find(Points.begin(), Points.end(), candidate) - Points.begin();
                if (find(order.begin(), order.end(), candidateIdx) == order.end()) {
                    order.push_back(candidateIdx);
                    currentPoint = candidateIdx;
                    break;
                }
            }
        }

        vector<uint> badNumbers;
        for (uint i = 0; i < order.size(); ++i) {
			vec4 point = Points[order[i]];
            vec4 prev = Points[order[(i + order.size() - 1) % order.size()]];
            vec4 next = Points[order[(i + 1) % order.size()]];
            if (dot(normalize((point - next).xyz()), normalize((prev - point).xyz())) > 1 - VEC_EPS) {
                badNumbers.push_back(i);
            }
        }
        for (int i = badNumbers.size() - 1; i >= 0; --i) {
            order.erase(order.begin() + badNumbers[i]);
        }
        if (order.size() < 3) {
			return vector<Triangle>();
        }

		/*for (uint i = 0; i < order.size(); ++i) {
            cout << Points[order[i]] << endl;
		}*/

        vector<Triangle> triangles(order.size() - 2);
        for (uint i = 2; i < order.size(); ++i) {
            uint indices[3] = {0, i - 1, i};
            for (uint j = 0; j < triangles[0].Points.size(); ++j) {
				triangles[i - 2].Points[j] = Points[order[indices[j]]];
				triangles[i - 2].Normals[j] = Normals[order[indices[j]]];
				triangles[i - 2].TexCoords[j] = TexCoords[order[indices[j]]];
            }
            triangles[i - 2].AmbientColor = AmbientColor;
            triangles[i - 2].MaterialNumber = MaterialNumber;
            triangles[i - 2].ImagePointer = ImagePointer;
        }
        return triangles;
    }
};

bool Related(const SetOfPoints& poly1, const SetOfPoints& poly2) {
	if (poly1.MaterialNumber != poly2.MaterialNumber) {
        return false;
	}

    if (poly1.Plane != poly2.Plane) {
        return false;
    }

    for (uint i = 0; i < poly1.Points.size() - 1; ++i) {
        for (uint j = 0; j < poly2.Points.size(); ++j) {
            if (poly1.Points[i] == poly2.Points[j]) {
                for (uint h = 0; h < poly1.Neighbors[i].size(); ++h) {
                    if (find(poly2.Neighbors[j].begin(), poly2.Neighbors[j].end(), poly1.Neighbors[i][h]) != poly2.Neighbors[j].end()) {
						return true;
					}
                }
            }
        }
    }
    return false;
}

SetOfPoints JoinPolygons(const SetOfPoints& poly1, const SetOfPoints& poly2) {
    SetOfPoints newPoly = poly1;
    for (uint i = 0; i < poly2.Points.size(); ++i) {
        newPoly.AddPoint(poly2.Points[i], poly2.Normals[i], poly2.TexCoords[i], poly2.Neighbors[i]);
    }
    return newPoly;
}


void CubeWithTriangles::ReorganizeTriangles() {
	vector<SetOfPoints> polygons;
    for (Triangle& triangle: Triangles) {
        polygons.push_back(SetOfPoints(triangle));
    }

    for (int i = polygons.size() - 1; i >= 0; --i) {
        for (int j = polygons.size() - 1; j > i; --j) {
            if (Related(polygons[i], polygons[j])) {
                polygons[i] = JoinPolygons(polygons[i], polygons[j]);
                polygons.erase(polygons.begin() + j);
            }
        }
    }
    Triangles.clear();
    for (uint i = 0; i < polygons.size(); ++i) {
        vector<Triangle> triangles = polygons[i].CreateTriangles();
        Triangles.insert(Triangles.end(), triangles.begin(), triangles.end());
    }
}

vector<Vertex> CubeWithTriangles::GetVertices() const {
    vector<Vertex> result;
    for (auto& triangle: Triangles) {
        auto vertices = triangle.GetVertices();
        result.insert(result.end(), vertices.begin(), vertices.end());
    }
    return result;
}
