#version 430

layout (local_size_x = 16, local_size_y = 16) in;

uniform mat4 lightMatrix;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float innerRadius;
uniform float outterRadius;
uniform sampler2D shadowMap;

layout(binding = 0, rgba32f) readonly uniform image1D patchPoints;
layout(binding = 1, rg32f) readonly uniform image1D samples;
layout(binding = 2, rgba32f) readonly uniform image1D colors;
layout(binding = 3, rgba32f) readonly uniform image1D normals;

layout(binding = 4, rgba32f) writeonly uniform image1D excident;
layout(binding = 5, rgba32f) writeonly uniform image1D indirect;

void main() {
    int i = int(gl_GlobalInvocationID.x);

    vec3 A = imageLoad(patchPoints, i * 4 + 0).xyz;
    vec3 B = imageLoad(patchPoints, i * 4 + 1).xyz;
    vec3 C = imageLoad(patchPoints, i * 4 + 3).xyz;

    imageStore(indirect, i, vec4(0));

    float lightNormalAngle = dot(-lightDirection, imageLoad(normals, i).xyz);

    if (lightNormalAngle <= 0) {
        imageStore(excident, i, vec4(0));
        return;
    }

    float resultEmission = 0;
    vec3 AB = (B - A);
    vec3 AC = (C - A);

    for (int j = 0; j < imageSize(samples); ++j) {
        vec2 quadPoint = imageLoad(samples, j).xy;
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
    resultEmission /= imageSize(samples);
    imageStore(excident, i, resultEmission * imageLoad(colors, i) * lightNormalAngle);
}
