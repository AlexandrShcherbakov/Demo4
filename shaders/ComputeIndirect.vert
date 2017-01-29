#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba16i) readonly uniform iimage2D indices;
layout(binding = 1, rgba32f) readonly uniform image2D weights;
layout(binding = 2, rgba32f) readonly uniform image2D patchesIndirect;
layout(binding = 3, rgba32f) writeonly uniform image2D pointsIndirect;

void main () {
    int i = int(gl_GlobalInvocationID.x);
    int x = i % 1024, y = i / 1024;
    if (x > 0) {
        y++;
    }
    vec4 result = vec4(0);
    ivec4 ind = imageLoad(indices, ivec2(x, y));
    vec4 wgh = imageLoad(weights, ivec2(x, y));
    result += imageLoad(patchesIndirect, ivec2(ind.x, 0)) * wgh.x;
    result += imageLoad(patchesIndirect, ivec2(ind.y, 0)) * wgh.y;
    result += imageLoad(patchesIndirect, ivec2(ind.z, 0)) * wgh.z;
    result += imageLoad(patchesIndirect, ivec2(ind.w, 0)) * wgh.w;
    imageStore(pointsIndirect, ivec2(x, y), result);
}
