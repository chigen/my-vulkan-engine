#version 450

// Vertex attribute discription:
// binding, location, format, offset

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 worldFragPos;
layout(location = 2) out vec3 worldFragNormal;

struct PointLight {
    vec4 position;
    vec4 color;
};

// for descriptor set 0 binding 0
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    // this is for parrallel light
    // vec3 directionToLight;

    // for point light
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

// push constant only support 128 bytes => 32 floats => 2 mat4
layout(push_constant) uniform Push{
    mat4 modelMatrix; // projection * view * model
    // for using case 3 below, use normal matrix instead
    // mat4 modelMatrix;
    mat4 normalMatrix;
} push;

// vertex light: compute light in vert shader
void main() {
    vec4 worldPosition = push.modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projection * ubo.view * worldPosition;

    // For normal transformation, 
    // 1. if we only allow uniform scaling, (vec3 scale => float scale) we can use:
    // vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * normal);

    // 2. more computationally, we can compute inverse transpose modelMatrix
    // vec3 normalWorldSpace = normalize(transpose(inverse(mat3(push.modelMatrix))) * normal);

    // 3. pass in pre-computed normal matrix to shaders
    // vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

    worldFragPos = worldPosition.xyz;
    worldFragNormal = normalize(mat3(push.normalMatrix) * normal);
    fragColor = color;
}