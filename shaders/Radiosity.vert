#version 430

layout (local_size_x = 256) in;

layout (binding = 4) buffer input1 {
    vec4 excident[];
};
layout (binding = 6) buffer output1 {
    vec4 incident[];
};
layout (binding = 0) buffer input3 {
    uint correctIndices[];
};
layout (binding = 1) buffer input2 {
    float correctValues[];
};

uniform sampler2D ffTexture;
uniform vec4 lower;
uniform vec4 upper;
uniform int correctLimit;


void main() {
    int i = int(gl_GlobalInvocationID.x);
    int size = int(gl_NumWorkGroups.x) * 256;
    vec4 result = vec4(0);
    for (int j = 0; j < size; ++j) {
        vec4 decodedFF = exp(texture(ffTexture, vec2(i, j) / textureSize(ffTexture, 0)) * vec4(25) - vec4(25)) * upper;
        result += excident[j] * decodedFF;
    }

    int start = correctLimit * 3 * i;
    int finish = start + correctLimit;
    for (int j = start; j < finish; ++j) {
        result.x += excident[correctIndices[j]].x * correctValues[j];
        result.y += excident[correctIndices[j + correctLimit]].y * correctValues[j + correctLimit];
        result.z += excident[correctIndices[j + correctLimit * 2]].z * correctValues[j + correctLimit * 2];
    }
    incident[i] = result;
}
