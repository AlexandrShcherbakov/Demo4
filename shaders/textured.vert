#version 430

in vec4 points;
in vec2 texCoord;
in vec4 normal;
in vec4 indirect;

/*layout(binding = 3) buffer output1 {
    vec4 pointsIndirect[];
};*/

out vec2 vertTexCoord;
out vec4 vertPos;
out vec4 vertNormal;
out vec4 vertLightPos;
out vec4 vertIndirect;

uniform mat4 camera;
uniform mat4 light;

void main() {
	gl_Position = camera * points;
	vertTexCoord = texCoord;
	vertPos = points;
	vertNormal = normal;
	vertLightPos = light * points;
	vertIndirect = indirect; //pointsIndirect[gl_VertexID];
}
