#version 430

layout (local_size_x = 256) in;

layout(binding = 0) buffer input1 {
    vec4 array[];
} patchPoints;
layout(binding = 1) buffer input2 {
    vec2 array[];
} samples;
layout(binding = 2) buffer input3 {
    vec4 array[];
} colors;
layout(binding = 3) buffer input4 {
    vec4 array[];
} normals;

layout(binding = 4) buffer output1 {
    vec4 array[];
} excident;
layout(binding = 5)buffer output2 {
    vec4 array[];
} indirect;

uniform mat4 lightMatrix;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float innerRadius;
uniform float outterRadius;
uniform sampler2D shadowMap;

void main() {
    int i = int(gl_GlobalInvocationID.x);

    vec3 A = patchPoints.array[i * 4 + 0].xyz;
    vec3 B = patchPoints.array[i * 4 + 1].xyz;
    vec3 C = patchPoints.array[i * 4 + 3].xyz;

    indirect.array[i] = vec4(0);

    float lightNormalAngle = dot(-lightDirection, normals.array[i].xyz);

    if (lightNormalAngle <= 0) {
        excident.array[i] = vec4(0);
        return;
    }

    float resultEmission = 0;
    vec3 AB = (B - A);
    vec3 AC = (C - A);

    for (int j = 0; j < samples.array.length(); ++j) {
        vec2 quadPoint = samples.array[j].xy;
        vec3 point = A + AB * quadPoint.x + AC * quadPoint.y;
        vec4 lightPoint = lightMatrix * vec4(point, 1);
        vec3 lightProj = lightPoint.xyz / lightPoint.w / 2 + vec3(0.5);
        if (texture(shadowMap, lightProj.xy).x > lightProj.z - 0.00001f) {
            float currentRadius = length(cross(lightDirection, point - lightPosition));
            float distantImpact = clamp((outterRadius - currentRadius)
                                    / (outterRadius - innerRadius), 0.0f, 1.0f);
            resultEmission += distantImpact;
        }
    }
    resultEmission /= samples.array.length();
    excident.array[i] = resultEmission * colors.array[i] * lightNormalAngle;
}
