#version 450

// Vertex attribute discription:
// binding, location, format, offset

layout(location = 0) in vec2 positions;
layout(location = 1) in vec3 colors;
// corresponding to the in vec3 inColor in frag
layout(location = 0) out vec3 outColor;

void main() {
    gl_Position = vec4(positions, 0.0, 1.0);
    outColor = colors;
}