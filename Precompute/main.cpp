#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
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

vector<const GL::Image *> images;
vector<VM::vec4> colors;
vector<uint> newOrder;

VM::vec4 min_point(1 / VEC_EPS, 1 / VEC_EPS, 1 / VEC_EPS, 1);
VM::vec4 max_point(-1 / VEC_EPS, -1 / VEC_EPS, -1 / VEC_EPS, 1);

vector<VM::vec2> hammersley;

string sceneName = "colored-sponza";
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
            texCoords.back().x = 1 - texCoords.back().x;
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
        while (images.size() <= ind) {
            images.push_back(nullptr);
            colors.push_back(VM::vec4(1.0f));
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
    return VM::dot(poly1.GetNormal(), poly2.GetCenter() - poly1.GetCenter()) > 0
        && VM::dot(poly2.GetNormal(), poly1.GetCenter() - poly2.GetCenter()) > 0;
}

vector<vector<float> > CountFF(const Octree& tree) {
    vector<Patch> patches = tree.GetPatches();
    vector<vector<float> > matrix(patches.size());
    for (uint i = 0; i < matrix.size(); ++i) {
        matrix[i].assign(patches.size(), 0);
    }
    for (uint i_point = 0; i_point < patches.size(); ++i_point) {
        Patch& p1 = patches[i_point];
        for (uint j_point = i_point + 1; j_point < patches.size(); ++j_point) {
            matrix[i_point][j_point] = 0;
            Patch& p2 = patches[j_point];
            if (!OrientationTest(p1, p2)) {
                continue;
            }

            uint cnt = 0;
            float ff_value = 0;
            Capsule volume(p1.GetCenter(), p2.GetCenter(), p1.GetSide());
            vector<Patch> middlePatches = tree.GetPatches(volume);
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
                    for (uint k = 0; k < middlePatches.size() && !flag; ++k) {
                        if (middlePatches[k].GetIndex() == i_point || middlePatches[k].GetIndex() == j_point) continue;
                        flag = middlePatches[k].Intersect(on_p1, on_p2);
                    }
                    if (flag) {
                        continue;
                    }

                    iter_res = VM::dot(r, p2.GetNormal()) / VM::length(r) / VM::length(p2.GetNormal());
                    if (iter_res < 0) {
                        continue;
                    }
                    iter_res *= VM::dot(-r, p1.GetNormal()) / VM::length(r) / VM::length(p1.GetNormal());
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
                matrix[i_point][j_point] = ff_value * p1.GetSquare();
                matrix[j_point][i_point] = ff_value * p2.GetSquare();
            }
        }
        if (100 * i_point / patches.size() < 100 * (i_point + 1) / patches.size())
            cout << 100 * (i_point + 1) / patches.size() << "% of ff computed" << endl;
    }
    return matrix;
}

void SavePatches(const vector<Patch>& patches, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    uint size = patches.size();
    uint newSize = (size / 256 + (size % 256 > 0)) * 256;
    std::cout << "RESULT PATCHES COUNT: " << newSize << endl;
    out.write((char*)&newSize, sizeof(newSize));
    for (uint i = 0; i < size; ++i) {
        VM::vec4 color = patches[i].GetColor();
        VM::vec4 normal = patches[i].GetNormal();
        out.write((char*)&color, sizeof(color));
        out.write((char*)&normal, sizeof(normal));
        for (auto& point: patches[i].GetPoints()) {
			out.write((char*)&point, sizeof(point));
        }
    }
    for (uint i = size; i < newSize; ++i) {
        VM::vec4 color(0.0f);
        VM::vec4 normal(0.0f);
        out.write((char*)&color, sizeof(color));
        out.write((char*)&normal, sizeof(normal));
        VM::vec4 points[] = {
            VM::vec4(0, 0, 0, 0),
            VM::vec4(1, 0, 0, 0),
            VM::vec4(1, 1, 0, 0),
            VM::vec4(0, 1, 0, 0),
        };
        for (auto& point: points) {
			out.write((char*)&point, sizeof(point));
        }
    }
    out.close();
}

void SaveModel(const Octree& octree, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    auto vertices = octree.GetVertices();
    uint pointsSize = vertices.size();
    uint newSize = (pointsSize / 256 + (pointsSize % 256 > 0)) * 256;
    out.write((char*)&newSize, sizeof(newSize));
    for (uint i = 0; i < pointsSize; ++i) {
        VM::vec4 position = vertices[i].GetPosition();
        VM::vec4 normal = vertices[i].GetNormal();
        VM::vec2 texCoord = vertices[i].GetTexCoord();
        uint materialNumber = vertices[i].GetMaterialNumber();
        VM::uvec4 relIndices = vertices[i].GetRelationIndices();
        VM::vec4 relWeights = vertices[i].GetRelationWeights();
        out.write((char*)&position, sizeof(position));
        out.write((char*)&normal, sizeof(normal));
        out.write((char*)&texCoord, sizeof(texCoord));
        out.write((char*)&materialNumber, sizeof(materialNumber));
        out.write((char*)&relIndices, sizeof(relIndices));
        out.write((char*)&relWeights, sizeof(relWeights));
    }
    for (uint i = pointsSize; i < newSize; ++i) {
        VM::vec4 v1(0.0f);
        VM::vec2 v2(0.0f);
        uint v3 = 0;
        VM::uvec4 v4;
        out.write((char*)&v1, sizeof(v1));
        out.write((char*)&v1, sizeof(v1));
        out.write((char*)&v2, sizeof(v2));
        out.write((char*)&v3, sizeof(v3));
        out.write((char*)&v4, sizeof(v4));
        out.write((char*)&v1, sizeof(v1));
    }

    auto indices = octree.GetTrianglesIndices();
    uint indicesSize = indices.size();
    out.write((char*)&indicesSize, sizeof(indicesSize));
    for (uint i = 0; i < indicesSize; ++i) {
        out.write((char*)&(indices[i]), sizeof(indices[i]));
    }
    out.close();
}

void SaveFF(const vector<vector<float> >& ff, const string& output) {
    ofstream out(output, ios::out | ios::binary);
    uint size = ff.size();
    uint newSize = (size / 256 + (size % 256 > 0)) * 256;
    out.write((char*)&newSize, sizeof(newSize));
    for (uint i = 0; i < size; ++i) {
        for (uint j = 0; j < size; ++j) {
            out.write((char*)&ff[i][j], sizeof(ff[i][j]));
        }
        for (uint j = size; j < newSize; ++j) {
            float f = 0;
            out.write((char*)&f, sizeof(f));
        }
    }
    for (uint i = size; i < newSize; ++i) {
        for (uint j = 0; j < newSize; ++j) {
            float f = 0;
            out.write((char*)&f, sizeof(f));
        }
    }
    out.close();
}

float sum(const std::vector<float>& v) {
    float res = 0;
    for (float f: v) {
        res += f;
    }
    return res;
}

vector<uint> PatchesToRemove(vector<vector<float> >& ff, const vector<Patch>& patches) {
    vector<uint> result;
    vector<uint> indices;
    vector<uint> shifts(patches.size(), 0);
    for (uint i = 0; i < patches.size(); ++i) {
        if (i) {
            shifts[i] = shifts[i - 1];
        }
        if (!sum(ff[i])) {
            shifts[i]++;
            result.push_back(i);
            indices.push_back(i);
        }
    }
    for (int i = indices.size() - 1; i >= 0; --i) {
        ff.erase(ff.begin() + indices[i]);
    }
    for (uint i = 0; i < ff.size(); ++i) {
        for (int j = indices.size() - 1; j >= 0; --j) {
            ff[i].erase(ff[i].begin() + indices[j]);
        }
    }
    return result;
}

pair< vector<Vertex>, vector<uint> > GenUniqVertices(const vector<Vertex>& vertices) {
    vector<vector<Vertex> > uniq;
    vector<uint> indices(vertices.size());
    for (uint i = 0; i < vertices.size(); ++i) {
        uint mat = vertices[i].GetMaterialNumber();
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
        indices[i] += offsets[vertices[i].GetMaterialNumber()];
    }
    return make_pair(uniq[0], indices);
}

string GenFilename(const string& part) {
    stringstream res;
    res << "../Scenes/" << sceneName << "/" << part << Size << ".bin";
    return res.str();
}


float CountMeasure(
    vector<float>& row1,
    vector<float>& row2,
    float (*measure)(const float a, const float b)
) {
    float result = 0;
    for (uint i = 0; i < row1.size(); ++i) {
        result += measure(row1[i], row2[i]);
    }
    return result;
}


const vector<uint> ReorderFF(vector<vector<float> >& ff) {
    auto measure = [](const float a, const float b) {return sqr(a) + sqr(b);};
    vector<uint> newOrder(ff.size());
    for (uint i = 0; i < newOrder.size(); ++i) {
        newOrder[i] = i;
    }
    for (uint i = 1; i < ff.size() - 1; ++i) {
        float optimal = CountMeasure(ff[i - 1], ff[i], measure);
        uint optimalIndex = i;
        for (uint j = i + 1; j < ff.size(); ++j) {
            float currentMeasure = CountMeasure(ff[i - 1], ff[j], measure);
            if (optimal > currentMeasure) {
                optimal = currentMeasure;
                optimalIndex = j;
            }
        }
        if (optimalIndex != i) {
            std::swap(newOrder[i], newOrder[optimalIndex]);
            ff[i].swap(ff[optimalIndex]);
            for (uint j = 0; j < ff.size(); ++j) {
                std::swap(ff[j][i], ff[j][optimalIndex]);
            }
        }
    }
    vector<uint> reverseOrder(newOrder.size());
    for (uint i = 0; i < newOrder.size(); ++i) {
        reverseOrder[newOrder[i]] = i;
    }
    return reverseOrder;
}


void ProcessFF(Octree& octree) {
    auto ff = CountFF(octree);
    cout << "Form-factors computed" << endl;
    cout << "FF rows: " << ff.size() << endl;
    auto patchesToRemove = PatchesToRemove(ff, octree.GetPatches());
    cout << "Patches filtered" << endl;
    cout << "Patches count was: " << octree.GetPatches().size() << endl;
    octree.RemovePatchesByIndices(patchesToRemove);
    cout << "Patches count became: " << octree.GetPatches().size() << endl;
    newOrder = ReorderFF(ff);
    octree.SetPatchesIndices(newOrder);
    cout << "FF reordered" << endl;

    SaveFF(ff, GenFilename("FF"));
    cout << "Form-factors saved" << endl;
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
		Octree octree(min_point, max_point, Size);
		cout << "Octree with triangles created" << endl;
        octree.Init(points, normals, texCoords, materialNum, images, colors);
		cout << "Octree initialized" << endl;
		InitHammersley(HammersleyCount);
        cout << "Hammersley inited" << endl;
        ProcessFF(octree);
        octree.GenerateRevertRelation();
        cout << "Revert relation generated" << endl;
        vector<Patch> patches = octree.GetPatches();
        sort(patches.begin(), patches.end(), [](const Patch& a, const Patch& b) {return a.GetIndex() < b.GetIndex();});
		SavePatches(patches, GenFilename("Patches"));
		cout << "Patches saved" << endl;
        SaveModel(octree, GenFilename("Model"));
        cout << "Model saved" << endl;
	} catch (const char* s) {
		cout << s << endl;
	}
}
