#version 430

layout (local_size_x = 256) in;

layout(binding = 4) buffer input1 {
    vec4 excident[];
};
layout(binding = 6) buffer input2 {
    vec4 incident[];
};
layout(binding = 5) buffer inout1 {
    vec4 indirect[];
};

void main()
{
    int i = int(gl_GlobalInvocationID.x);
    vec4 inc = incident[i];
    indirect[i] += inc;
    excident[i] = inc;
}
