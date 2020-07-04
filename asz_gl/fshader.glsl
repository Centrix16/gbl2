#version 330 core

in vec3 shape_color;
in vec2 texture_coordinates;

out vec4 color;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
	color = mix(texture(texture1, texture_coordinates), texture(texture2, texture_coordinates), 0.5);
}
