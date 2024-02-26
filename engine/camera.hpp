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

            const glm::mat4& getProjection() const { return projection; }

        private:
            glm::mat4 projection{1.f};
    };
    
} // namespace engin
