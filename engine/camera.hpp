#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <cassert>
#include <limits>

namespace engine{
    class Camera{
        public:
            void setOrthographicProjection(
                float left, float right,
                float bottom, float top,
                float near, float far
            );
            void setPerspectiveProjection(
                float fovy, float aspect, float near, float far
            );

            void setViewDirection(const glm::vec3 position, const glm::vec3 direction, 
                const glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});

            void setViewTarget(const glm::vec3 position, const glm::vec3 target, 
                const glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});

            void setViewYXZ(const glm::vec3 position, const glm::vec3 rotation);

            const glm::mat4& getProjection() const { return projection; }
            const glm::mat4& getView() const { return view; }
            const glm::mat4& getInverseView() const { return inverseView; }

        private:
            glm::mat4 projection{1.f};
            glm::mat4 view{1.f};
            glm::mat4 inverseView{1.f};
    };
    
} // namespace engin
