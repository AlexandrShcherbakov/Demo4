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

uniform float outterRadius;
uniform float innerRadius;

vec4 gamma(vec4 v) {
    return pow(v, vec4(1 / 2.2));
}

void main() {
	outColor = vec4(0.0);

	vec3 lightProj = vertLightPos.xyz / vertLightPos.w / 2 + vec3(0.5f);
    vec2 offset = fract(lightProj.xy * 0.5);
	offset.x = float(offset.x > 0.25);
	offset.y = float(offset.y > 0.25);
	offset.y += offset.x;

	if (offset.y > 1.1) {
        offset.y = 0;
	}

    float shadowCoef = 0.0f;
    shadowCoef += float(texture(shadowMap, lightProj.xy + vec2(-1.5,  0.5) / 2048).x >= lightProj.z - 0.00001f);
    shadowCoef += float(texture(shadowMap, lightProj.xy + vec2( 0.5,  0.5) / 2048).x >= lightProj.z - 0.00001f);
    shadowCoef += float(texture(shadowMap, lightProj.xy + vec2(-1.5, -1.5) / 2048).x >= lightProj.z - 0.00001f);
    shadowCoef += float(texture(shadowMap, lightProj.xy + vec2( 0.5, -1.5) / 2048).x >= lightProj.z - 0.00001f);
    shadowCoef /= 4.0;


	vec3 L = vertPos.xyz - lightPos;
	vec3 D = normalize(lightDir);

	float currentRadius = dot(L, D);

	float spot = clamp((outterRadius - currentRadius) / (outterRadius - innerRadius), 0.0f, 1.0f);
	vec3 N = normalize(vertNormal.xyz);
	float lambertTerm = dot(N, -D);
	lambertTerm = max(0.0, lambertTerm);
	outColor += lambertTerm * spot * vec4(lightColor, 0) * material_color * shadowCoef;

	outColor += vertIndirect * material_color;

	outColor = gamma(outColor);
}
