#version 330

in vec4 points;
in vec4 colors;

out vec4 vertColor;

uniform mat4 camera;

void main() {
	gl_Position = camera * points;
	vertColor = colors;
}
