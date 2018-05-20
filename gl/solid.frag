#version 120

uniform float zoom;
uniform vec4 color;

varying vec3 ec_pos;

void main() {
//    gl_FragColor = color;
    gl_FragColor = vec4(color[0],color[1],color[2],color[3]);
}
