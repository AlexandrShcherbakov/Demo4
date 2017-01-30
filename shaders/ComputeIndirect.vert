#version 430

layout (local_size_x = 256) in;

layout(binding = 0) buffer input1 {
    ivec4 array[];
} indices;
layout(binding = 1) buffer input2 {
    vec4 array[];
} weights;
layout(binding = 2) buffer input3 {
    vec4 array[];
} patchesIndirect;
layout(binding = 3) buffer output1 {
    vec4 array[];
} pointsIndirect;

void main () {
    int i = int(gl_GlobalInvocationID.x);
    vec4 result = vec4(0);
    ivec4 ind = indices.array[i];
    vec4 wgh = weights.array[i];
    result += patchesIndirect.array[ind.x] * wgh.x;
    result += patchesIndirect.array[ind.y] * wgh.y;
    result += patchesIndirect.array[ind.z] * wgh.z;
    result += patchesIndirect.array[ind.w] * wgh.w;
    pointsIndirect.array[i] = result;
}
