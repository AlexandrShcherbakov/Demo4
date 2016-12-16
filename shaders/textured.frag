#version 330

in vec2 vertTexCoord;
in vec4 vertPos;
in vec4 vertNormal;
in vec4 vertLightPos;
in vec4 vertIndirect;

out vec4 outColor;

uniform sampler2D material_texture;
uniform sampler2D shadowMap;
uniform vec4 material_color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDir;

uniform float outterCos;
uniform float innerCos;

vec4 gamma(vec4 v) {
    return pow(v, vec4(1 / 2.2));
}

void main() {
    vec4 material = texture(material_texture, vec2(vertTexCoord.x, 1 - vertTexCoord.y)) * material_color;

	outColor = material / 5;
	outColor = vec4(0.0);

	vec3 lightProj = vertLightPos.xyz / vertLightPos.w / 2 + vec3(0.5f);
    float shadowCoef = 0.0f;
    for (float x_off = -0.75f; x_off <= 0.75f; x_off += 0.5f) {
        for (float y_off = -0.75f; y_off <= 0.75f; y_off += 0.5f) {
            if (texture(shadowMap, lightProj.xy + vec2(x_off, y_off) / 2048).x >= lightProj.z - 0.00001f) {
                shadowCoef += 1.0f;
            }
        }
    }
    shadowCoef /= 16.0f;

	vec3 L = normalize(vertPos.xyz - lightPos);
	vec3 D = normalize(lightDir);

	float current_angle = dot(L, D);

	float spot = clamp((current_angle - outterCos) / (innerCos - outterCos), 0.0f, 1.0f);
	vec3 N = normalize(vertNormal.xyz);
	float lambertTerm = max(dot(N, -L), 0.0);
	outColor += lambertTerm * spot * vec4(lightColor, 0) * material * shadowCoef;

    outColor /= 2.0;

    outColor += vertIndirect * material / 10;

	outColor = gamma(outColor);
}
