#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 vertex_tex_coord;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1.0);
	vertex_tex_coord = tex_coord;
}

