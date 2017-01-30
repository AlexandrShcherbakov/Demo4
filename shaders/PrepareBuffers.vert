#version 430

layout (local_size_x = 256) in;

layout(binding = 0) buffer input1 {
    vec4 array[];
} excident;
layout(binding = 1) buffer input2 {
    vec4 array[];
} incident;
layout(binding = 2) buffer inout1 {
    vec4 array[];
} indirect;
layout(binding = 3) buffer input3 {
    vec4 array[];
} color;

void main()
{
    int i = int(gl_GlobalInvocationID.x);
    vec4 inc = incident.array[i];
    indirect.array[i] += inc;
    excident.array[i] = inc * color.array[i];
}
