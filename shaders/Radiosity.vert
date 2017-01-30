#version 430

layout (local_size_x = 256) in;

layout (binding = 0) buffer input1 {
    vec4 array[];
} excident;
layout (binding = 1) buffer input2 {
    vec4 array[];
} ff;
layout (binding = 2) buffer output1 {
    vec4 array[];
} incident;

void main() {
    int i = int(gl_GlobalInvocationID.x);
    int size = int(gl_NumWorkGroups.x) * 256;
    vec4 result = vec4(0);
    for (int j = 0; j < size; ++j) {
        result += ff.array[i * size + j] * excident.array[j];
    }
    incident.array[i] = result;
}
