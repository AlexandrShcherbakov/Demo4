#version 330

in vec2 vertTexCoord;
in vec4 vertPos;
in vec4 vertNormal;
in vec4 vertLightPos;
in vec4 vertIndirect;

out vec4 outColor;

uniform sampler2D shadowMap;
uniform vec4 material_color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDir;

const float outterCos = 0.3;
const float innerCos = 0.4;

vec4 gamma(vec4 v) {
    return pow(v, vec4(1 / 2.2));
}

void main() {
	outColor = vec4(0.0);

	vec3 lightProj = vertLightPos.xyz / vertLightPos.w / 2 + vec3(0.5f);
    float shadowCoef = 0.0f;
    for (float x_off = -0.75f; x_off <= 0.75f; x_off += 0.5f) {
        for (float y_off = -0.75f; y_off <= 0.75f; y_off += 0.5f) {
            if (texture(shadowMap, lightProj.xy + vec2(x_off, y_off) / 2048).x >= lightProj.z - 0.0001f) {
                shadowCoef += 1.0f;
            }
        }
    }
    shadowCoef /= 16.0f;


	vec3 L = normalize(vertPos.xyz - lightPos);
	vec3 D = normalize(lightDir);

	float current_angle = dot(L, D);

	float spot = clamp((outterCos - current_angle) / (outterCos - innerCos), 0.0f, 1.0f);
	vec3 N = normalize(vertNormal.xyz);
	float lambertTerm = dot(N, -L);
	lambertTerm = max(0.0, lambertTerm);
	outColor += lambertTerm * spot * vec4(lightColor, 0) * material_color * shadowCoef;
	outColor *= 4;

	outColor += vertIndirect * material_color / 8;

	outColor = gamma(outColor);
}
