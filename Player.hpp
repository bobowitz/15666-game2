#pragma once

#include <glm/glm.hpp>

#include "Scene.hpp"
#include "Car.hpp"
#include "Wall.hpp"

struct Player {
    struct PlayerInput {
        bool up_pressed = false;
        bool down_pressed = false;
        bool left_pressed = false;
        bool right_pressed = false;

        PlayerInput(bool u, bool d, bool l, bool r): up_pressed(u), down_pressed(d), left_pressed(l), right_pressed(r) {}
    };

    const float DECELERATION = 5000.0f;
    const float ACCELERATION = 10000.0f + DECELERATION;
    const float MIN_SPEED = 1.0f;
    const float VELOCITY = 20.0f;

    glm::vec2 bb_pos = glm::vec2(0.0f, 0.0f);
    glm::vec2 bb_size = glm::vec2(1.0f, 1.0f);
    glm::vec2 vel = glm::vec2(0.0f, 0.0f);

    // radians
    float rotation = 0.0f;
    float shown_rotation = 0.0f;

    Scene::Transform *transform = nullptr;

    bool update(float elapsed, PlayerInput input, std::vector<Car> const &cars, std::vector<Wall> const &walls);
    glm::vec3 get_center();
};