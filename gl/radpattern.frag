#version 120

varying vec4 frag_color;

void main() {
    gl_FragColor = frag_color; // vec4(frag_color[0], frag_color[1], frag_color[2], 0.5);
}
