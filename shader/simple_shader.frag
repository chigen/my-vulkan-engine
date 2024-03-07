#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 worldFragPos;
layout(location = 2) in vec3 worldFragNormal;

layout(location = 0) out vec4 outColor;

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

// fragment lighting: compute light in fragment shader
void main() {
    vec3 diffuseLight = vec3(0.0);
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 surfaceNormal = normalize(worldFragNormal);

    for (int i=0; i<ubo.numLights; ++i) {
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - worldFragPos;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        float cosAngIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0.0);
        vec3 intensity = light.color.xyz * light.color.w * cosAngIncidence * attenuation;

        diffuseLight += intensity;
    }

    // r g b a
    // outColor = vec4(inColor, 1.0);
    outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}