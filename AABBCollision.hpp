#pragma once

#include <glm/glm.hpp>

inline bool aabbCollision(glm::vec2 pos1, glm::vec2 size1, glm::vec2 pos2, glm::vec2 size2) {
    if (pos1.x >= pos2.x + size2.x) return false;
    if (pos1.x + size1.x <= pos2.x) return false;
    if (pos1.y >= pos2.y + size2.y) return false;
    if (pos1.y + size1.y <= pos2.y) return false;
    return true;
}