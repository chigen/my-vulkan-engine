#include "game_object.hpp"

namespace engine {
    glm::mat3 Transform2dComponent::mat3() {
        const float c = cos(rotation);
        const float s = sin(rotation);
        glm::mat3 rotateMat = {
            c, s, 0.0f,
            -s, c, 0.0f,
            0.0f, 0.0f, 1.0f
        };
        glm::mat3 scaleMat = {
            scale.x, 0.0f, 0.0f,
            0.0f, scale.y, 0.0f,
            0.0f, 0.0f, 1.0f
        };
        glm::mat3 translateMat = {
            1.0f, 0.0f, translation.x,
            0.0f, 1.0f, translation.y,
            0.0f, 0.0f, 1.0f
        };
        // return the transformation matrix
        return translateMat * rotateMat * scaleMat;
    }

    glm::mat4 Transform3dComponent::mat4() {
        // hand-coded transformation matrix, Translation * Rotation(yxz) * Scale
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f}};
    }

    glm::mat3 Transform3dComponent::normalMatrix() {
        // compute (modelMatrix^-1)^T
        // = ((rotation * scale)^-1)^T
        // = rotation * scale^-1
        // because rotation matrix is orthogonal and scale matrix is diagonal
        return glm::mat3(glm::transpose(glm::inverse(mat4())));
    }

    GameObject GameObject::makePointLight(float intensity, float radius, glm::vec3 color) {
        
        GameObject obj = GameObject::createGameObject();
        obj.color = color;
        obj.transform3d.scale.x = radius;
        obj.pointLight = std::make_unique<PointLightComponent>();
        obj.pointLight->intensity = intensity;
        return obj;
    }
}