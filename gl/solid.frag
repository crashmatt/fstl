#version 120

uniform float zoom;
uniform vec4 color;

varying vec3 ec_pos;

void main() {
    gl_FragColor = color;
}
