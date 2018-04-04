#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

out vec3 vertex_normal;
out vec3 vertex_pos;
out vec2 vertex_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normal_matrix;

void main() {
    gl_Position = proj * view * model * vec4(pos, 1.0);
    vertex_normal = normal_matrix * normal;
    vertex_pos = vec3(view * model * vec4(pos, 1.0));
    vertex_tex_coord = tex_coord;
}

