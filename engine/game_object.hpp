#pragma once

#include "model.hpp"

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace engine {
    struct Transform2dComponent{
        glm::vec2 translation{0.0f, 0.0f};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        glm::mat3 mat3();
    };

    struct Transform3dComponent{
        glm::vec3 translation {};
        glm::vec3 scale {1.0f, 1.0f, 1.0f};
        glm::vec3 rotation {};

        glm::mat4 mat4();

        glm::mat3 normalMatrix();
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