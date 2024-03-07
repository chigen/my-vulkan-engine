#version 450

// for descriptor set 0 binding 0
layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    // this is for parrallel light
    // vec3 directionToLight;

    // for point light
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
} ubo;

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;

void main() {
    float dis = sqrt(dot(fragOffset, fragOffset));
    if(dis >= 1.0) {
        // not render the pixel outside the circle
        discard;
    }
    outColor = vec4(ubo.lightColor.xyz, 1.0);
}