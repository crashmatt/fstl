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
    mat4 matrix = view_matrix * transform_matrix;
    gl_Position = matrix * vec4(vertex_position, 1.0);
    vec4 vert = matrix * vec4(vertical, 1.0);
    vec4 horiz = matrix * vec4(vertical, 1.0);
    float vert_color    = abs( (vert.y+horiz.y) * zoom * 2.0);
    float horiz_color   = abs( (vert.x+horiz.x) * zoom * 2.0);
    frag_color = vec3(vert_color, horiz_color, 0.0);

//    frag_color = abs(vertex_color * vert.y * zoom * 2.0);
}
