#include "camera.hpp"

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
}