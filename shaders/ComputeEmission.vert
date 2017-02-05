#version 430

layout (local_size_x = 256) in;

layout(std430, binding = 0) buffer input1 {
    vec4 patchPoints[];
};
layout(std430, binding = 1) buffer input2 {
    vec2 samples[];
};
layout(std430, binding = 2) buffer input3 {
    vec4 colors[];
};
layout(std430, binding = 3) buffer input4 {
    vec4 normals[];
};

layout(std430, binding = 4) buffer output1 {
    vec4 excident[];
};
layout(std430, binding = 5) buffer output2 {
    vec4 indirect[];
};

uniform mat4 lightMatrix;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float innerRadius;
uniform float outterRadius;
uniform sampler2D shadowMap;

void main() {
    int i = int(gl_GlobalInvocationID.x);

    vec3 A = patchPoints[i * 4 + 0].xyz;
    vec3 B = patchPoints[i * 4 + 1].xyz;
    vec3 C = patchPoints[i * 4 + 3].xyz;

    indirect[i] = vec4(0);

    float lightNormalAngle = dot(-lightDirection, normals[i].xyz);

    if (lightNormalAngle <= 0) {
        excident[i] = vec4(0);
        return;
    }

    float resultEmission = 0;
    vec3 AB = (B - A);
    vec3 AC = (C - A);

    for (int j = 0; j < samples.length(); ++j) {
        vec2 quadPoint = samples[j].xy;
        vec3 point = A + AB * quadPoint.x + AC * quadPoint.y;
        vec4 lightPoint = lightMatrix * vec4(point, 1);
        vec3 lightProj = lightPoint.xyz / lightPoint.w / 2 + vec3(0.5);
        if (texture(shadowMap, lightProj.xy).x > lightProj.z - 0.00001f) {
            float currentRadius = length(cross(lightDirection, point - lightPosition));
            float distantImpact = clamp(
                (outterRadius - currentRadius) / (outterRadius - innerRadius),
                0.0f, 1.0f
            );
            resultEmission += distantImpact;
        }
    }
    resultEmission /= samples.length();
    excident[i] = resultEmission * colors[i] * lightNormalAngle;
}
