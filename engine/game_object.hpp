#pragma once

#include "model.hpp"

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace engine {
    struct Transform2dComponent{
        glm::vec2 translation{0.0f, 0.0f};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        glm::mat3 mat3() {
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

    };

    struct Transform3dComponent{
        glm::vec3 translation {};
        glm::vec3 scale {1.0f, 1.0f, 1.0f};
        glm::vec3 rotation {};

        glm::mat4 mat4() {
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

    };
    class GameObject {
        public:
            // every game object has a unique id
            using id_t = unsigned int;

            static GameObject createGameObject() {
                static id_t currentId = 0;
                return GameObject{currentId++};
            }

            // delete copy constructor and operator to avoid copying the game object
            GameObject(const GameObject&) = delete;
            GameObject& operator=(const GameObject&) = delete;
            // use move constructor to transfer the ownership of the model
            GameObject(GameObject&&) = default;
            GameObject& operator=(GameObject&&) = default;

            id_t getId() { return id; }

            // Transform2dComponent transform2d{};
            Transform3dComponent transform3d{};
            std::shared_ptr<Model> model;
            glm::vec3 color{};

        private:
            GameObject(id_t objId) : id{objId} {}
            id_t id;            
    };
}