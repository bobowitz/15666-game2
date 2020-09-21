#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

struct Intersection;

struct StreetEnd {
    glm::vec2 stop_bb_pos;
    glm::vec2 stop_bb_size;
    glm::vec2 go_target;
    glm::vec2 go_direction;

    Intersection *intersection;

    StreetEnd();
};