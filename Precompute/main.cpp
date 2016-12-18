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

uint Size = 20;
uint HammersleyCount = 10;

void ReadData(const string &path) {
    hyFile.read(path);
    for (uint i = 0; i < hyFile.getVerticesNumber(); i++) {
        points.push_back(VM::vec4(hyFile.getVertexPositionsFloat4Array() + 4 * i));
        normals.push_back(VM::vec4(hyFile.getVertexNormalsFloat4Array() + 4 * i));
        texCoords.push_back(VM::vec2(hyFile.getVertexTexcoordFloat2Array() + 2 * i));
        indices.push_back(hyFile.getTriangleVertexIndicesArray()[i]);
        materialNum.push_back(hyFile.getTriangleMaterialIndicesArray()[i / 3]);

        if (materialNum.back() == 19) {
            texCoords.back() = texCoords.back() * VM::vec2(0.25, 0.25) + VM::vec2(0.375, 0.375);
        }
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
            img->UndoGamma();
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

bool OrientationTest(const Patch& poly1, const Patch& poly2) {
    return VM::dot(poly1.Normal, poly2.Center() - poly1.Center()) > 0 &&
           VM::dot(poly2.Normal, poly1.Center() - poly2.Center()) > 0;
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

void SavePatches(const vector<Patch>& patches, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    uint size = patches.size();
    out.write((char*)&size, sizeof(size));
    for (uint i = 0; i < size; ++i) {
        out.write((char*)&(patches[i].Color), sizeof(patches[i].Color));
        out.write((char*)&(patches[i].Normal), sizeof(patches[i].Normal));
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
        float radius = step * 2;
        //while(relation[i].second.x == 0 && radius <= step * 3) {
        Hemisphere vol(points[i], radius, normals[i]);
        vector<Patch> localPatches = patches.Root.GetPatches(&vol);
        vector<float> measure(localPatches.size());
        for (uint j = 0; j < localPatches.size(); ++j) {
            measure[j] = max(VM::dot(normals[i], localPatches[j].Normal), 0.1f);
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
        if (100 * i / points.size() < 100 * (i + 1) / points.size()) {
            cout << 100 * (i + 1) / points.size() << "% of relations computed" << endl;
        }
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
    res << "data\\colored-sponza\\" << name << Size << ".bin";
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
		ReadData("../Scenes/colored-sponza/sponza_exported/scene.vsgf");
		cout << "Data readed" << endl;
		ReadMaterials("..\\Scenes\\colored-sponza\\sponza_exported\\hydra_profile_generated.xml");
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
