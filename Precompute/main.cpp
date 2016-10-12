#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>

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

uint Size = 20;

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

int main(int argc, char **argv) {
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
	cout << octree.GetTriangles().size() << endl;
}
