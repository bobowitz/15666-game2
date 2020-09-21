#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct Car;
struct StreetEnd;

struct Intersection {
    glm::vec2 bb_pos;
    glm::vec2 bb_size;

    std::vector<StreetEnd *> ends;

    std::vector<Car *> cars;
    bool car_in_intersection = false;
    bool entered_intersection = false;
    Car *in_intersection = nullptr;

    Intersection();
    void enqueue(Car *car);
    void update(float elapsed);
};