#version 450

// Vertex attribute discription:
// binding, location, format, offset

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colors;

layout(location = 0) out vec3 fragColor;

// for push constant
layout(push_constant) uniform Push{
    mat4 transform;
    vec3 color;
} push;

void main() {
    gl_Position = push.transform * vec4(position, 1.0);
    fragColor = colors;
}