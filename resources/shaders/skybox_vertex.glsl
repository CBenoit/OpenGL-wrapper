#version 330 core

layout (location = 0) in vec3 pos;

out vec3 tex_coord;

uniform mat4 VP; // MVP matrix but without model.

void main() {
	tex_coord = pos;
	tex_coord.y = -tex_coord.y;
    vec4 p = VP * vec4(pos, 1.0);
    gl_Position = p.xyww; // trick the depth buffer (make the skybox always behind everything)
}
