#include "camera.hpp"

#include <iostream>

namespace engine{
    void Camera::setOrthographicProjection(
        float left, float right,
        float bottom, float top,
        float near, float far
    ){
        projection = glm::mat4{1.f};
        projection = {
            2 / (right - left), 0, 0, 0,
            0, 2 / (top - bottom), 0, 0,
            0, 0, 1 / (far - near), 0,
            (left + right) / (left - right),
            (bottom + top) / (bottom - top),
            near / (near - far),
            1
        };
    }

    void Camera::setPerspectiveProjection(
        float fovy, float aspect, float near, float far
    ){
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

        const float tanHalfFov = tan(fovy / 2.f);
        projection = glm::mat4{0.f};
        projection = {
            1 / (aspect * tanHalfFov), 0, 0, 0,
            0, 1 / tanHalfFov, 0, 0,
            0, 0, far / (far - near), 1,
            0, 0, -(far * near) / (far - near), 0
        };
    }

    void Camera::setViewDirection(const glm::vec3 position, const glm::vec3 direction, 
        const glm::vec3 up){
        // set the orthonormal basis for the camera
        const glm::vec3 w = glm::normalize(direction);
        const glm::vec3 u = glm::normalize(glm::cross(up, w));
        const glm::vec3 v = glm::cross(w, u);

        view = glm::mat4{1.f};
        view = {
            u.x, v.x, w.x, 0,
            u.y, v.y, w.y, 0,
            u.z, v.z, w.z, 0,
            -glm::dot(u, position),
            -glm::dot(v, position),
            -glm::dot(w, position),
            1
        };

        inverseView = {
            u.x, u.y, u.z, 0,
            v.x, v.y, v.z, 0,
            w.x, w.y, w.z, 0,
            position.x, position.y, position.z, 1
        };
    }

    void Camera::setViewTarget(const glm::vec3 position, const glm::vec3 target, 
        const glm::vec3 up){
        setViewDirection(position, target - position, up);
    }

    void Camera::setViewYXZ(const glm::vec3 position, const glm::vec3 rotation){
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
        const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
        const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
        
        view = glm::mat4{1.f};
        view = {
            u.x, v.x, w.x, 0,
            u.y, v.y, w.y, 0,
            u.z, v.z, w.z, 0,
            -glm::dot(u, position),
            -glm::dot(v, position),
            -glm::dot(w, position),
            1
        };

        inverseView = {
            u.x, u.y, u.z, 0,
            v.x, v.y, v.z, 0,
            w.x, w.y, w.z, 0,
            position.x, position.y, position.z, 1
        };
    }
}