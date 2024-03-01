#version 450

// Vertex attribute discription:
// binding, location, format, offset

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

// for descriptor set 0 binding 0
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec3 directionToLight;
} ubo;

// push constant only support 128 bytes => 32 floats => 2 mat4
layout(push_constant) uniform Push{
    mat4 modelMatrix; // projection * view * model
    // for using case 3 below, use normal matrix instead
    // mat4 modelMatrix;
    mat4 normalMatrix;
} push;

const float ambient = 0.01;

void main() {
    gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

    // For normal transformation, 
    // 1. if we only allow uniform scaling, (vec3 scale => float scale) we can use:
    // vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * normal);

    // 2. more computationally, we can compute inverse transpose modelMatrix
    // vec3 normalWorldSpace = normalize(transpose(inverse(mat3(push.modelMatrix))) * normal);

    // 3. pass in pre-computed normal matrix to shaders
    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

    float lightIntensity = max(dot(normalWorldSpace, ubo.directionToLight), 0.0) + ambient;
    
    fragColor = lightIntensity * color;
}