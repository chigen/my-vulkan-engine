#version 450

// Vertex attribute discription:
// binding, location, format, offset

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 colors;
// corresponding to the in vec3 inColor in frag
// layout(location = 0) out vec3 outColor;

// for push constant
layout(push_constant) uniform Push{
    mat3 transform;
    vec3 color;
} push;

void main() {
    // position = transform * vec3(positions, 1.0), then take the xy
    vec3 transformedPosition = push.transform * positions;
    gl_Position = vec4(transformedPosition.x, transformedPosition.y, 0.0, 1.0);
    // outColor = colors;
}