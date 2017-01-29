#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba32f) readonly uniform image1D excident;
layout (binding = 1, r32f) readonly uniform image2D ff;
layout (binding = 2, rgba32f) writeonly uniform image1D incident;

void main() {
    int i  = int(gl_GlobalInvocationID.x);
    vec4 result = vec4(0);
    for (int j = 0; j < imageSize(ff).x; ++j) {
        result += imageLoad(ff, ivec2(i, j)).x * imageLoad(excident, j);
    }
    imageStore(incident, i, result);
}
