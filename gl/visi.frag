#version 120

varying vec3 frag_color;

void main() {
    gl_FragColor = vec4(frag_color[0], frag_color[1], frag_color[2], 0.5);
}
