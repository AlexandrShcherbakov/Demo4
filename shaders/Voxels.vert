#version 330

in vec4 points;
in vec4 color;

out vec4 vertColor;

uniform mat4 cam;

void main() {
	gl_Position = cam * points;
	vertColor = color;
	//vertColor = points;
}
