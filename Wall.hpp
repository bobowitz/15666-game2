#pragma once

#include <glm/glm.hpp>

struct Wall {
    glm::vec2 pos;
    glm::vec2 size;

    Wall(glm::vec2 p, glm::vec2 s): pos(p), size(s) { }
};