#version 330

in vec4 points;

uniform mat4 camera;

void main() {
	gl_Position = camera * points;
}
