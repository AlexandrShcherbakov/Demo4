#include "Image.h"

using namespace VM;

namespace GL {

Image::Image() {
	width = height = 0;
}

Image::Image(const std::string& filename) {
    load(filename);
}

void Image::load(const std::string& filename) {
	width = height = 0;
    int channels = 0;
    unsigned char * img = SOIL_load_image(filename.c_str(), (int*)&width, (int*)&height, &channels, SOIL_LOAD_RGBA);

    data.resize(height);
    for (uint i = 0; i < height; ++i) {
        data[i].resize(width);
        for (uint j = 0; j < width; ++j) {
            for (uint h = 0; h < 4; ++h) {
                data[i][j][h] = img[(i * width + j) * 4 + h];
            }
        }
    }
    SOIL_free_image_data(img);
}

const std::vector<vec4>& Image::operator[] (uint i) const {
    if (i >= height) {
        char error[1024];
        sprintf(error, "%d is too big index for image with height %d", i, height);
        throw std::string(error);
    }
    return data[i];
}

std::vector<vec4>& Image::operator[] (uint i) {
	if (i >= height) {
        char error[1024];
        sprintf(error, "%d is too big index for image with height %d", i, height);
        throw std::string(error);
    }
    return data[i];
}

vec4 Image::getAverageColor(const std::vector<vec2>& texCoords) const {
    vec2 minCoord = texCoords[0];
    vec2 maxCoord = texCoords[0];
    vec2 center = texCoords[0];
    for (uint i = 1; i < texCoords.size(); ++i) {
        minCoord = min(minCoord, texCoords[i]);
        maxCoord = max(maxCoord, texCoords[i]);
        center += texCoords[i];
    }

    minCoord = vec2(minCoord.x - std::floor(minCoord.x), minCoord.y - std::floor(minCoord.y)) * vec2(width, height);
    maxCoord = vec2(maxCoord.x - std::floor(maxCoord.x), maxCoord.y - std::floor(maxCoord.y)) * vec2(width, height);
    center /= texCoords.size();
    center = vec2(center.x - std::floor(center.x), center.y - std::floor(center.y)) * vec2(width, height);

    std::vector<vec2> normals(texCoords.size());
    for (uint i = 0; i < normals.size(); ++i) {
		if (length(texCoords[(i + 1) % normals.size()] - texCoords[i]) < VEC_EPS)
            return data[center.y][center.x];
        normals[i] = normalize(texCoords[(i + 1) % normals.size()] - texCoords[i]);
        normals[i] = vec2(-normals[i].y, normals[i].x);
    }

	vec4 color(0, 0, 0, 0);
    uint cnt = 0;

    for (uint i = minCoord.x; i < maxCoord.x; ++i) {
        for (uint j = minCoord.y; j < maxCoord.y; ++j) {
            ///Test
            bool testRes = true;
            for (uint t = 0; t < normals.size() && testRes; ++t)
                testRes = dot(normals[t], vec2(i + 0.5, j + 0.5)) * dot(normals[t], center) >= 0;
			testRes = true;
            if (testRes)
                color += data[j][i];
            cnt += testRes;
        }
    }

    if (!cnt) return data[center.y][center.x];
    return color / cnt;
}

std::vector<char> Image::getData(uint channels) const {
	std::vector<char> result;
	for (uint i = 0; i < height; ++i)
		for (uint j = 0; j < width; ++j)
			for (uint h = 0; h < channels; ++h)
				result.push_back((char)data[i][j][h]);
	return result;
}

}
