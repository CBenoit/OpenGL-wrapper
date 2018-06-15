#version 330 core

in vec3 vertex_position;

out vec4 frag_color;

uniform samplerCube skybox;

void main() {
	frag_color = texture(skybox, vertex_position);
}

