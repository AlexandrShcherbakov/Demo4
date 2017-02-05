#version 330

in vec4 vertColor;

out vec4 outColor;

void main() {
	outColor = vertColor;
	if (length(vertColor) == 0) {
        //outColor = vec4(0.3);
	}
}
