#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

// push constant only support 128 bytes => 32 floats => 2 mat4
layout(push_constant) uniform Push{
    mat4 transform; // projection * view * model
    // for using case 3 below, use normal matrix instead
    // mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
  // r g b a
  // outColor = vec4(inColor, 1.0);
  outColor = vec4(fragColor, 1.0);
}