#version 120
attribute vec3 vertex_position;
attribute vec3 vertex_color;
attribute vec3 vertical;
attribute vec3 horizontal;

uniform mat4 transform_matrix;
uniform mat4 view_matrix;
uniform float zoom;

varying vec3 frag_color;

void main() {
    gl_Position = view_matrix * transform_matrix * vec4(vertex_position, 1.0);
    vec4 vert = view_matrix * transform_matrix * vec4(vertical, 1.0);
    frag_color = abs(vertex_color * vert.y * zoom * 2.0); //vertex_color *  vert.y;
}
