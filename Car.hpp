#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include "Scene.hpp"

struct StreetEnd;

struct Car {
    enum State {
        STRAIGHT_DRIVING,
        IN_INTERSECTION_STRAIGHT,
        IN_INTERSECTION_TURNING,
        WAITING_FOR_INTERSECTION,
        WAITING_FOR_CAR
    } state = STRAIGHT_DRIVING;

    const float VELOCITY = 17.5f;
    const float BUMPER_DISTANCE = 2.5f;

    glm::vec2 bb_pos;
    glm::vec2 bb_size = glm::vec2(2.0f, 3.7f);
    glm::vec2 vel;
    float rotation = 0.0f;
    glm::vec2 bumper_bb_pos;
    glm::vec2 bumper_bb_size = glm::vec2(1.0f, 1.0f);

    float total_time = 0.0f;
    float turn_start_time = 0.0f;
    float turn_duration = 0.0f;
    bool turn_ytox = false;
    float start_rotation = 0.0f;
    float target_rotation = 0.0f;
    // center points
    glm::vec2 start_pos;
    glm::vec2 target_pos;

    StreetEnd *stopped_at = nullptr;

    Scene::Transform *transform;

    Car();
    glm::vec3 get_position();
    glm::quat get_rotation();

    void update(float elapsed, std::vector<StreetEnd> &street_ends, std::vector<Car> &cars);
    void enter_intersection(); // callback called by Intersection

private:
    bool collision(StreetEnd const &street_end);
    bool bumper_collision(Car const &other);
};