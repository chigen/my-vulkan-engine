#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

// for push constant
layout(push_constant) uniform Push{
    mat4 transform;
    vec3 color;
} push;

void main() {
  // r g b a
  // outColor = vec4(inColor, 1.0);
  outColor = vec4(fragColor, 1.0);
}