#version 430

layout (local_size_x = 256) in;

layout (binding = 4) buffer input1 {
    vec4 excident[];
};
layout (binding = 7) buffer input2 {
    vec4 ff[];
};
layout (binding = 6) buffer output1 {
    vec4 incident[];
};

void main() {
    int i = int(gl_GlobalInvocationID.x);
    int size = int(gl_NumWorkGroups.x) * 256;
    vec4 result = vec4(0);
    for (int j = 0; j < size; ++j) {
        result += ff[i * size + j] * excident[j];
    }
    incident[i] = result;
}
