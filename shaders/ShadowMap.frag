#version 330

out vec4 outColor;

void main() {
	outColor = vec4(vec3(gl_FragCoord.z), 1);
}
