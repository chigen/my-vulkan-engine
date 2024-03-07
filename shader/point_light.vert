#version 450

// create the billboard object by offsetting each vertex position
// from the point light position
const vec2 offsets[6] = vec2[6](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;

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

const float LIGHT_RADIUS = 0.05;

void main() {
    fragOffset = offsets[gl_VertexIndex];
    vec3 worldCameraRight = {ubo.view[0].x, ubo.view[1].x, ubo.view[2].x};
    vec3 worldCameraUp = {ubo.view[0].y, ubo.view[1].y, ubo.view[2].y};

    vec3 worldPosition = ubo.lightPosition.xyz 
        + worldCameraRight * fragOffset.x * LIGHT_RADIUS
        + worldCameraUp * fragOffset.y * LIGHT_RADIUS;

    gl_Position = ubo.projection * ubo.view * vec4(worldPosition, 1.0);
}