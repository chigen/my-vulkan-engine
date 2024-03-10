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
    mat4 inverseView;
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
    // blinn-phong equation:
    // I = (kd * Id * cos(theta)) + (ks * Is * cos(alpha)) + (ka * Ia)
    vec3 specularLight = vec3(0.0);
    vec3 diffuseLight = vec3(0.0);
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 surfaceNormal = normalize(worldFragNormal);

    vec3 worldCameraPos = ubo.inverseView[3].xyz;
    vec3 viewDirection = normalize(worldCameraPos - worldFragPos);

    for (int i=0; i<ubo.numLights; ++i) {
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - worldFragPos;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        directionToLight = normalize(directionToLight);
        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0.0);
        vec3 intensity = light.color.xyz * light.color.w * cosAngIncidence * attenuation;

        diffuseLight += intensity;

        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = max(dot(halfAngle, surfaceNormal), 0.0);
        blinnTerm = clamp(blinnTerm, 0.0, 1.0);
        blinnTerm = pow(blinnTerm, 512.0); // higher p => sharper highlight
        
        specularLight += blinnTerm * intensity;
    }

    // r g b a
    // outColor = vec4(inColor, 1.0);
    outColor = vec4((diffuseLight + ambientLight + specularLight) * fragColor, 1.0);
}