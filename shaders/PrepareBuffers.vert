#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32f) writeonly uniform image1D excident;
layout(binding = 1, rgba32f) readonly uniform image1D incident;
layout(binding = 2, rgba32f) uniform image1D indirect;
layout(binding = 3, rgba32f) readonly uniform image1D color;

void main()
{
    int i = int(gl_GlobalInvocationID.x);
    vec4 inc = imageLoad(incident, i);
    imageStore(indirect, i, inc + imageLoad(indirect, i));
    imageStore(excident, i, inc * imageLoad(color, i));
}
