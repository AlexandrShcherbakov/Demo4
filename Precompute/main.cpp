#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <utility>

#include "HydraExport.h"
#include "GeometryLib\Octree.h"

using namespace std;

HydraGeomData hyFile;
vector<VM::vec4> points, normals;
vector<VM::vec2> texCoords;
vector<uint> materialNum, indices;

map<uint, vector<uint> > splitedIndices;

map<uint, GL::Image *> images;
map<uint, VM::vec4> colors;

Octree scene_space;
Octree patches;

VM::vec4 min_point(1 / VEC_EPS, 1 / VEC_EPS, 1 / VEC_EPS, 1);
VM::vec4 max_point(-1 / VEC_EPS, -1 / VEC_EPS, -1 / VEC_EPS, 1);

vector<VM::vec2> hammersley;

uint Size = 10;
uint HammersleyCount = 10;

void ReadData(const string &path) {
    hyFile.read(path);
    for (uint i = 0; i < hyFile.getVerticesNumber(); i++) {
        points.push_back(VM::vec4(hyFile.getVertexPositionsFloat4Array() + 4 * i));
        normals.push_back(VM::vec4(hyFile.getVertexNormalsFloat4Array() + 4 * i));
        texCoords.push_back(VM::vec2(hyFile.getVertexTexcoordFloat2Array() + 2 * i));
        indices.push_back(hyFile.getTriangleVertexIndicesArray()[i]);
        materialNum.push_back(hyFile.getTriangleMaterialIndicesArray()[i / 3]);
    }
}

void ReadMaterials(const string& path) {
    ifstream in(path);
    while(true) {
        string s;
        do {
            getline(in, s);
        } while (s.find("<material") == string::npos && !in.eof());
        if (in.eof())
            break;
        uint startInd = s.find("maxid = \"") + 9;
        uint endInd = s.find('\"', startInd);
        int ind;
        sscanf(s.substr(startInd, endInd - startInd).c_str(), "%d", &ind);
        do {
            getline(in, s);
        } while (s.find("<color>") == string::npos);
        VM::vec4 color;
        sscanf(s.substr(s.find("<color>") + 7).c_str(), "%f %f %f", &color.x, &color.y, &color.z);
        do {
            getline(in, s);
        } while (s.find("<texture>") == string::npos && s.find("</material>") == string::npos);
        GL::Image * img = nullptr;
        if (s.find("<texture>") != string::npos) {
            startInd = s.find("<texture> ") + 10;
            endInd = s.find(" </texture>");
            s = s.substr(startInd, endInd - startInd);
            img = new GL::Image();
            img->load(s);
        }
        images[ind] = img;
        colors[ind] = color;
    }
}

void FindCube() {
    for (VM::vec4 point : points) {
        min_point = VM::min(min_point, point);
        max_point = VM::max(max_point, point);
    }
    min_point -= VM::vec4(VEC_EPS, VEC_EPS, VEC_EPS, 0);
    max_point += VM::vec4(VEC_EPS, VEC_EPS, VEC_EPS, 0);
}

void FillOctree() {
    scene_space = Octree(min_point, max_point, Size);
    for (uint i = 0; i < points.size(); i += 3) {
        TexturedPolygon poly;
        poly.setMaterialNumber(materialNum[i]);
        poly.setImage(images[materialNum[i]]);
        poly.setColor(colors[materialNum[i]]);
        poly.addPoint(points[i], normals[i], texCoords[i]);
        poly.addPoint(points[i + 1], normals[i + 1], texCoords[i + 1]);
        poly.addPoint(points[i + 2], normals[i + 2], texCoords[i + 2]);
        poly.setIndex(i / 3);
        scene_space.addPolygon(poly);
        if ((i + 3) * 100 / points.size() > i * 100 / points.size()) {
            cout << (i + 3) * 100 / points.size() << "% patches added to octree" << endl;
        }
    }
}

void PrintTreeInfo(const Octree& tree, const string& name) {
    uint patches = tree.getPolygons().size();
    uint subspaces = tree.getNodesCount();
    cout << "Info about Octree " << name << endl;
    cout << "Count of patches: " << patches << endl;
    cout << "Count of subspaces: " << subspaces << endl;
    cout << "Side: " << tree.getSide() << endl;
}

void SavePatchesToFile(const vector<TexturedPolygon>& patches, const string& filename) {
    ofstream out(filename, ios::out | ios::binary);
    uint size = patches.size();
    out.write((char *)&size, sizeof(size));
    VM::vec4 point, color, normal;
    vector<uint> relation;
    vector<float> weights;
    uint idx_size;
    uint poly_size;
    for (uint i = 0; i < patches.size(); ++i) {
        poly_size = patches[i].getSize();
        out.write((char*)&poly_size, sizeof(poly_size));
        for (uint j = 0; j < patches[i].getSize(); ++j) {
            point = patches[i].getPoints()[j];
            out.write((char*)&point, sizeof(point));
        }
        color = patches[i].getColor();
        out.write((char*)&color, sizeof(color));
        normal = patches[i].normal();
        out.write((char*)&normal, sizeof(normal));
        relation = patches[i].getRelations();
        weights = patches[i].getWeights();
        idx_size = relation.size();
        out.write((char*)&idx_size, sizeof(idx_size));
        for (uint j = 0; j < idx_size; ++j) {
            out.write((char*)&relation[j], sizeof(relation[j]));
            out.write((char*)&weights[j], sizeof(weights[j]));
        }
    }
    out.close();
}

void SavePatchesToFile(const Octree& tree, const string& filename) {
    vector<TexturedPolygon> patches = tree.getPolygons();
    SavePatchesToFile(patches, filename);
}

void SaveSplitedGeometry(const Octree& tree, const string& filename) {
    ofstream out(filename, ios::out | ios::binary);
    vector<TexturedPolygon> patches = tree.getPolygons();
    uint size = 0;
    for (auto& poly: patches) {
        size += (poly.getSize() - 2) * 3;
    }
    out.write((char *)&size, sizeof(size));
    VM::vec4 point, normal;
    VM::vec2 tex_coord;
    uint mat_num;
    for (uint i = 0; i < patches.size(); ++i) {
        for (uint j = 2; j < patches[i].getSize(); ++j) {
            point = patches[i].getPoints()[0];
            out.write((char *)&point, sizeof(point));
            normal = patches[i].getNormals()[0];
            out.write((char *)&normal, sizeof(normal));
            tex_coord = patches[i].getTexCoords()[0];
            out.write((char *)&tex_coord, sizeof(tex_coord));
            mat_num = patches[i].getMaterialNumber();
            out.write((char *)&mat_num, sizeof(mat_num));
            point = patches[i].getPoints()[j - 1];
            out.write((char *)&point, sizeof(point));
            normal = patches[i].getNormals()[j - 1];
            out.write((char *)&normal, sizeof(normal));
            tex_coord = patches[i].getTexCoords()[j - 1];
            out.write((char *)&tex_coord, sizeof(tex_coord));
            mat_num = patches[i].getMaterialNumber();
            out.write((char *)&mat_num, sizeof(mat_num));
            point = patches[i].getPoints()[j];
            out.write((char *)&point, sizeof(point));
            normal = patches[i].getNormals()[j];
            out.write((char *)&normal, sizeof(normal));
            tex_coord = patches[i].getTexCoords()[j];
            out.write((char *)&tex_coord, sizeof(tex_coord));
            mat_num = patches[i].getMaterialNumber();
            out.write((char *)&mat_num, sizeof(mat_num));
        }
    }
    out.close();
}

void InitHammersley(const uint count) {
    hammersley.resize(count);
    for (uint i = 0; i < count; ++i) {
        float u = 0;
        int kk = i;
        for (float p = 0.5; kk; p *= 0.5, kk >>= 1)
            if (kk & 1)
                u += p;
        float v = (i + 0.5) / count;
        hammersley[i] = VM::vec2(u, v);
    }
}

bool OrientationTest(const TexturedPolygon& poly1, const TexturedPolygon& poly2) {
    return VM::dot(poly1.getNormals()[0], poly2.center() - poly1.center()) > 0;
}


bool OrientationTest(const Patch& poly1, const Patch& poly2) {
    return VM::dot(poly1.Normal, poly2.Center() - poly1.Center()) > 0 &&
           VM::dot(poly2.Normal, poly1.Center() - poly2.Center()) > 0;
}

float DistanceFromPointToLine(
    const VM::vec4& point,
    const VM::vec4& l_start,
    const VM::vec4& l_end) {
    VM::vec4 line = l_end - l_start;
    VM::vec4 to_point = point - l_start;
    return VM::length(VM::cross(line, to_point)) / VM::length(line) / VM::length(to_point);
}

vector<TexturedPolygon> FilterPolygonsByCylinder(
    const vector<TexturedPolygon>& polys,
    const TexturedPolygon& p1,
    const TexturedPolygon& p2) {
    vector<TexturedPolygon> result;
    VM::vec4 c1 = p1.center();
    VM::vec4 c2 = p2.center();
    float threshold = VM::length(p1.getPoints()[0] - p1.getPoints()[1]) * 2;
    for (const TexturedPolygon& poly: polys) {
        for (VM::vec4& point: poly.getPoints()) {
            if (DistanceFromPointToLine(point, c1, c2) < threshold) {
                result.push_back(poly);
                break;
            }
        }
    }
    return result;
}

vector<vector<pair<uint, float> > > CountFF(const OctreeWithPatches& tree) {
    vector<Patch> patches = tree.GetPatches();
    vector<vector<pair<uint, float> > > sparseMatrix(patches.size());
    for (uint i_point = 0; i_point < patches.size(); ++i_point) {
        Patch& p1 = patches[i_point];
        for (uint j_point = i_point + 1; j_point < patches.size(); ++j_point) {
            Patch& p2 = patches[j_point];
            if (!OrientationTest(p1, p2)) {
                continue;
            }
            //cout << p1.Normal << ' ' << p2.Normal << endl;
            //cout << p1.Center() << ' ' << p2.Center() << endl;

            uint cnt = 0;
            float ff_value = 0;
            Capsule volume(p1.Center(), p2.Center(), p1.Side());
            vector<Patch> middlePatches = tree.Root.GetPatches(&volume);
            for (uint i = 0; i < hammersley.size(); ++i) {
                for (uint j = 0; j < hammersley.size(); ++j) {
                    float iter_res = 0;
                    VM::vec4 on_p1 = p1.PointByCoords(hammersley[i]);
                    VM::vec4 on_p2 = p2.PointByCoords(hammersley[j]);

                    VM::vec4 r = on_p1 - on_p2;
                    float lr = VM::length(r);
                    const float threshold = 0.00001;
                    if (std::abs(lr) < threshold) {
                        cnt++;
                        continue;
                    }

                    //Visibility function
                    int flag = 0;
                    VM::vec4 min_point(min(on_p1, on_p2));
                    VM::vec4 max_point(max(on_p1, on_p2));
                    for (uint k = 0; k < middlePatches.size(); ++k) {
                        if (middlePatches[k] == p1 || middlePatches[k] == p2) continue;
                        flag = middlePatches[k].Intersect(on_p1, on_p2);
                    }
                    if (flag) {
                        continue;
                    }

                    iter_res = VM::dot(r, p2.Normal) / VM::length(r) / VM::length(p2.Normal);
                    if (iter_res < 0) {
                        continue;
                    }
                    iter_res *= VM::dot(-r, p1.Normal) / VM::length(r) / VM::length(p1.Normal);
                    if (iter_res < 0) {
                        continue;
                    }
                    iter_res /=  sqr(lr);
                    if (iter_res > 100000) {
                        continue;
                    }
                    ff_value += iter_res;
                }
            }
            ff_value /= sqr(hammersley.size()) - cnt;
            ff_value /= M_PI;
            if (ff_value > sqr(VEC_EPS)) {
                sparseMatrix[i_point].push_back(make_pair(j_point, ff_value * p1.GetSquare()));
                sparseMatrix[j_point].push_back(make_pair(i_point, ff_value * p2.GetSquare()));
            }
        }
        if (100 * i_point / patches.size() < 100 * (i_point + 1) / patches.size())
            cout << 100 * (i_point + 1) / patches.size() << "% of ff computed" << endl;
    }
    return sparseMatrix;
}

vector<vector<float> > CountFF(const Octree& tree) {
    static ofstream log("log1.txt");
    vector<TexturedPolygon> polygons = tree.getPolygons();
    vector<vector<float> > result(polygons.size());
    for (uint i = 0; i < polygons.size(); ++i)
        result[i].assign(polygons.size(), 0);
    for (uint i_point = 0; i_point < polygons.size(); ++i_point) {
        TexturedPolygon p1 = polygons[i_point];
        //clock_t timestamp = clock();
        for (uint j_point = i_point + 1; j_point < polygons.size(); ++j_point) {
            TexturedPolygon p2 = polygons[j_point];
            if (!OrientationTest(p1, p2)) {
                result[i_point][j_point] = result[j_point][i_point] = 0.0f;
                continue;
            }
            uint cnt = 0;
            float ff_value = 0;
            for (uint i = 0; i < hammersley.size(); ++i) {
                for (uint j = 0; j < hammersley.size(); ++j) {
                    float iter_res = 0;
                    VM::vec4 on_p1 = p1.pointByCoords(hammersley[i]);
                    VM::vec4 on_p2 = p2.pointByCoords(hammersley[j]);

                    VM::vec4 r = on_p1 - on_p2;
                    float lr = VM::length(r);
                    const float threshold = 0.00001;
                    if (std::abs(lr) < threshold) {
                        cnt++;
                        continue;
                    }

                    //Visibility function
                    int flag = 0;
                    VM::vec4 min_point(min(on_p1, on_p2));
                    VM::vec4 max_point(max(on_p1, on_p2));
                    auto patches = tree.getPatchesFromCube(min_point, max_point);
                    patches = FilterPolygonsByCylinder(patches, p1, p2);
                    for (uint k = 0; k < patches.size(); ++k) {
                        if (patches[k] == p1 || patches[k] == p2) continue;
                        flag = patches[k].intersect(on_p1, on_p2);
                    }
                    if (flag) {
                        continue;
                    }

                    iter_res = VM::dot(r, p2.normal()) / VM::length(r) / VM::length(p2.normal());
                    iter_res *= VM::dot(-r, p1.normal()) / VM::length(r) / VM::length(p1.normal());
                    if (iter_res < 0) {
                        continue;
                    }
                    iter_res /=  sqr(lr);
                    if (iter_res > 100000) {
                        continue;
                    }
                    ff_value += iter_res;
                }
            }
            ff_value /= sqr(hammersley.size()) - cnt;
            ff_value /= M_PI;
            result[i_point][j_point] = ff_value * p1.getSquare();
            result[j_point][i_point] = ff_value * p2.getSquare();
        }
        //log << "Time: " << clock() - timestamp << endl;
        if (100 * i_point / polygons.size() < 100 * (i_point + 1) / polygons.size())
            cout << 100 * (i_point + 1) / polygons.size() << "% of ff computed" << endl;
    }
    return result;
}

void FilterMatrix(vector<vector<float> >& ff, vector<TexturedPolygon>& poly) {
    for (int i = poly.size() - 1; i >= 0; --i) {
        float sum = 0;
        for (float f: ff[i])
            sum += f;
        if (sum == 0) {
            for (uint j = 0; j < poly.size(); ++j)
                ff[j].erase(ff[j].begin() + i);
            poly.erase(poly.begin() + i);
            ff.erase(ff.begin() + i);
        }
    }
}

void SaveMatrix(const vector<vector<float> >& matrix, const string& filename) {
    ofstream out(filename, ios::out | ios::binary);
    uint size = matrix.size();
    out.write((char*)&size, sizeof(size));
    for (uint i = 0; i < matrix.size(); ++i) {
        for (uint j = 0; j < matrix[i].size(); ++j) {
            out.write((char*)&matrix[i][j], sizeof(matrix[i][j]));
        }
    }
    out.close();
}

vector<vector<float> > LoadMatrix(const string& filename) {
    ifstream in(filename);
    uint size;
    in >> size;
    vector<vector<float> > matrix(size);
    for (uint i = 0; i < matrix.size(); ++i) {
        matrix[i].resize(size);
        for (uint j = 0; j < matrix[i].size(); ++j) {
            in >> matrix[i][j];
        }
    }
    in.close();
    return matrix;
}

int old_main(int argc, char **argv) {
    cout << "Start" << endl;
    ReadData("../Scenes/dabrovic-sponza/sponza_exported/scene.vsgf");
    cout << "Data readed" << endl;
    ReadMaterials("..\\Scenes\\dabrovic-sponza\\sponza_exported\\hydra_profile_generated.xml");
    cout << "Materials readed" << endl;
    FindCube();
    cout << "Min/max point found" << endl;
    FillOctree();
    cout << "Octree created" << endl;
    PrintTreeInfo(scene_space, "Scene");
    patches = CreateVoxelTreeByScene(scene_space);
    cout << "Patches created" << endl;
    PrintTreeInfo(patches, "Voxel model");
    //SavePatchesToFile(patches, "Patches63");
    //cout << "Patches saved" << endl;
    SaveSplitedGeometry(scene_space, "small_poly_sponza");
    cout << "New geometry saved" << endl;
    //InitHammersley(4);
    //vector<vector<float> > ff = CountFF(patches);
    //vector<TexturedPolygon> polygons = patches.getPolygons();
    //cout << "Form-factors computed" << endl;
    //FilterMatrix(ff, polygons);
    //cout << "Matrix filtered" << endl;
    //vector<vector<float> > ff = LoadMatrix("ff20_up.txt");
    //cout << "Matrix loaded" << endl;
    //FilterMatrix(ff, polygons);
    //cout << "Matrix filtered" << endl;
    //cout << "New size: " << polygons.size() << endl;
    //SavePatchesToFile(polygons, "patches20.bin");
    //cout << "Patches saved" << endl;
    //SaveMatrix(ff, "ff20.bin");
    //cout << "Form-factors saved" << endl;
    return 0;
}

void FillByTriangles(OctreeWithTriangles& octree) {
    for (uint i = 0; i < points.size(); i += 3) {
        octree.SetTriangle(
			points.data() + i,
            normals.data() + i,
			texCoords.data() + i,
            images[materialNum[i]],
            colors[materialNum[i]],
            materialNum[i]
		);
    }
}

void SaveTriangles(const vector<Triangle>& triangles, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    uint size = triangles.size() * 3;
    out.write((char*)&size, sizeof(size));
    VM::vec4 point, normal;
    VM::vec2 texCoord;
    uint materialNumber;
    for (uint i = 0; i < size / 3; ++i) {
        for (uint j = 0; j < 3; ++j) {
            point = triangles[i].Points[j];
            normal = triangles[i].Normals[j];
            texCoord = triangles[i].TexCoords[j];
            materialNumber = triangles[i].MaterialNumber;

            out.write((char*)&point, sizeof(point));
            out.write((char*)&normal, sizeof(normal));
            out.write((char*)&texCoord, sizeof(texCoord));
            out.write((char*)&materialNumber, sizeof(materialNumber));
        }
    }
    out.close();
}

void SavePatches(const vector<Patch>& patches, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    uint size = patches.size();
    out.write((char*)&size, sizeof(size));
    for (uint i = 0; i < size; ++i) {
        out.write((char*)&(patches[i].Color), sizeof(patches[i].Color));
        for (auto& point: patches[i].Points) {
			out.write((char*)&point, sizeof(point));
        }
    }
    out.close();
}

void SaveModel(const pair<vector<Vertex>, vector<uint> >& model, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    uint pointsSize = model.first.size();
    out.write((char*)&pointsSize, sizeof(pointsSize));
    for (uint i = 0; i < pointsSize; ++i) {
        out.write((char*)&(model.first[i].Position), sizeof(model.first[i].Position));
        out.write((char*)&(model.first[i].Normal), sizeof(model.first[i].Normal));
        out.write((char*)&(model.first[i].TexCoord), sizeof(model.first[i].TexCoord));
        out.write((char*)&(model.first[i].MaterialNumber), sizeof(model.first[i].MaterialNumber));
        out.write((char*)&(model.first[i].RelationIndices), sizeof(model.first[i].RelationIndices));
        out.write((char*)&(model.first[i].RelationWeights), sizeof(model.first[i].RelationWeights));
    }

    uint indicesSize = model.second.size();
    out.write((char*)&indicesSize, sizeof(indicesSize));
    for (uint i = 0; i < indicesSize; ++i) {
        out.write((char*)&(model.second[i]), sizeof(model.second[i]));
    }
    out.close();
}

void SaveFF(const vector<vector<pair<uint, float> > >& ff, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    uint globalSize = ff.size();
    out.write((char*)&globalSize, sizeof(globalSize));
    for (uint i = 0; i < globalSize; ++i) {
        uint localSize = ff[i].size();
        out.write((char*)&localSize, sizeof(localSize));
        for (uint j = 0; j < localSize; ++j) {
            out.write((char*)&(ff[i][j].first), sizeof(ff[i][j].first));
            out.write((char*)&(ff[i][j].second), sizeof(ff[i][j].second));
        }
    }
    out.close();
}

vector<Patch> PatchesToRemove(vector<vector<pair<uint, float> > >& ff, const vector<Patch>& patches) {
    vector<Patch> result;
    vector<uint> indices;
    vector<uint> shifts(patches.size(), 0);
    for (uint i = 0; i < patches.size(); ++i) {
        if (i) {
            shifts[i] = shifts[i - 1];
        }
        if (ff[i].empty()) {
            shifts[i]++;
            result.push_back(patches[i]);
            indices.push_back(i);
        }
    }
    for (uint i = 0; i < ff.size(); ++i) {
        for (uint j = 0; j < ff[i].size(); ++j) {
            ff[i][j].first -= shifts[ff[i][j].first];
        }
    }
    for (auto it = indices.rbegin(); it != indices.rend(); ++it) {
        ff.erase(ff.begin() + *it);
    }
    return result;
}

void RemoveBadPatches(OctreeWithPatches& octree, const vector<Patch>& patches) {
    for (uint i = 0; i < patches.size(); ++i) {
        octree.RemovePatch(patches[i]);
    }
}

inline float RevertRelationMeasure(const Patch& patch, const VM::vec4& point, const VM::vec4& normal) {
    VM::vec4 R = patch.Center() - point;
    if (VM::length(R) < VEC_EPS) {
        return 1;
    }
    return max(VM::dot(R, normal) / VM::length(R), 0.0f);
}

vector<pair<VM::vec4, VM::vec4> > GenRevertRelations(const OctreeWithTriangles& triangles, const OctreeWithPatches& patches) {
    vector<VM::vec4> points = triangles.GetPoints();
    vector<VM::vec4> normals = triangles.GetNormals();
    vector<pair<VM::vec4, VM::vec4> > relation(points.size());
    float step = patches.GetPatches()[0].Side();
    for (uint i = 0; i < points.size(); ++i) {
        relation[i].first = VM::vec4(0, 0, 0, 0);
        relation[i].second = VM::vec4(0, 0, 0, 0);
        float radius = step * 3;
        while(relation[i].second.x == 0 && radius <= step * 3) {
            Hemisphere vol(points[i], radius, normals[i]);
            vector<Patch> localPatches = patches.Root.GetPatches(&vol);
            vector<float> measure(localPatches.size());
            for (uint j = 0; j < localPatches.size(); ++j) {
                measure[j] = RevertRelationMeasure(localPatches[j], points[i], normals[i]);
                for (uint h = 0; h < j; ++h) {
                    if (measure[j] > measure[h]) {
                        swap(measure[j], measure[h]);
                        swap(localPatches[j], localPatches[h]);
                    }
                }
            }
            uint j;
            for (j = 0; j < 4 && relation[i].second[j] != 0; ++j) {
            }
            for (uint idx = 0; j < 4 && idx < localPatches.size(); ++j, ++idx) {
                relation[i].first[j] = localPatches[idx].Index;
                relation[i].second[j] = measure[idx];
            }
            radius += step;
        }
        if (100 * i / points.size() < 100 * (i + 1) / points.size())
            cout << 100 * (i + 1) / points.size() << "% of relations computed" << endl;
    }
    return relation;
}

pair< vector<Vertex>, vector<uint> > GenUniqVertices(const vector<Vertex>& vertices) {
    vector<vector<Vertex> > uniq;
    vector<uint> indices(vertices.size());
    for (uint i = 0; i < vertices.size(); ++i) {
        uint mat = vertices[i].MaterialNumber;
        if (mat >= uniq.size()) {
            uniq.resize(mat + 1);
        }

        uint ind;
        for (ind = 0; ind < uniq[mat].size() && uniq[mat][ind] != vertices[i]; ++ind) {
        }
        if (uniq[mat].size() == ind) {
            uniq[mat].push_back(vertices[i]);
        }
        indices[i] = ind;
    }
    vector<uint> offsets(uniq.size(), 0);
    for (uint i = 1; i < uniq.size(); ++i) {
        offsets[i] = offsets[i - 1] + uniq[i - 1].size();
        uniq[0].insert(uniq[0].end(), uniq[i].begin(), uniq[i].end());
    }
    for (uint i = 0; i < indices.size(); ++i) {
        indices[i] += offsets[vertices[i].MaterialNumber];
    }
    return make_pair(uniq[0], indices);
}

string GenFilename(const string& name) {
    stringstream res;
    res << "data\\" << name << Size << ".bin";
    return res.str();
}

vector<Patch> ProcessFF(const OctreeWithPatches& patchedOctree) {
    auto ff = CountFF(patchedOctree);
    cout << "Form-factors computed" << endl;
    cout << "FF rows: " << ff.size() << endl;
    auto patchesToRemove = PatchesToRemove(ff, patchedOctree.GetPatches());
    cout << "Patches filtered" << endl;
    cout << "FF rows: " << ff.size() << endl;
    uint count = 0;
    for (uint i = 0; i < ff.size(); ++i) {
        count += ff[i].size();
    }
    cout << "FF full size: " << count << endl;

    SaveFF(ff, GenFilename("FF"));
    cout << "Form-factors saved" << endl;
    return patchesToRemove;
}

int main(int argc, char **argv) {
	try {
		cout << "Start" << endl;
		ReadData("../Scenes/dabrovic-sponza/sponza_exported/scene.vsgf");
		cout << "Data readed" << endl;
		ReadMaterials("..\\Scenes\\dabrovic-sponza\\sponza_exported\\hydra_profile_generated.xml");
		cout << "Materials readed" << endl;
		FindCube();
		cout << "Min/max point found" << endl;
		OctreeWithTriangles octree(Size, min_point, max_point);
		cout << "Octree with triangles created" << endl;
		FillByTriangles(octree);
		cout << "Fill octree by triangles" << endl;
		OctreeWithPatches patchedOctree(octree);
		cout << "Create octree with patches" << endl;
        InitHammersley(HammersleyCount);
        cout << "Hammersley inited" << endl;

        auto patchesToRemove = ProcessFF(patchedOctree);

        cout << "Patches count: " << patchedOctree.GetPatches().size() << endl;
        RemoveBadPatches(patchedOctree, patchesToRemove);
        cout << "Patches count: " << patchedOctree.GetPatches().size() << endl;
        cout << "Patched octree filtered" << endl;
        patchedOctree.SetIndices();
        cout << "Indices for patches generated" << endl;
        auto backRealation = GenRevertRelations(octree, patchedOctree);
        cout << "Revert relation generated" << endl;

		SavePatches(patchedOctree.GetPatches(), GenFilename("Patches"));
		cout << "Patches saved" << endl;

        auto vertices = octree.GetVertices();
        for (uint i = 0; i < vertices.size(); ++i) {
            vertices[i].RelationIndices = backRealation[i].first;
            vertices[i].RelationWeights = backRealation[i].second;
        }
        cout << "Vertices formed" << endl;

        SaveModel(GenUniqVertices(vertices), GenFilename("Model"));
        cout << "Model saved" << endl;

	} catch (const char* s) {
		cout << s << endl;
	}
}
