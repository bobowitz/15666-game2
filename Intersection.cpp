#include "Intersection.hpp"

#include <iostream>

#include "AABBCollision.hpp"
#include "Car.hpp"

Intersection::Intersection() : ends(), cars() {

}

void Intersection::enqueue(Car *car) {
    cars.push_back(car);
}

void Intersection::update(float elapsed) {
    if (!car_in_intersection) {
        if (!cars.empty()) {
            in_intersection = cars.front();
            cars.erase(cars.begin());
            in_intersection->enter_intersection();
            car_in_intersection = true;
            entered_intersection = false;
        }
    } else {
        if (!aabbCollision(in_intersection->bb_pos, in_intersection->bb_size, bb_pos, bb_size)) {
            if (entered_intersection)
                car_in_intersection = false;
        } else {
            entered_intersection = true;
        }
    }
}