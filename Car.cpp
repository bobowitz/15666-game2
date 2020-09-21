#include "Car.hpp"

#include "gl_errors.hpp"
#include "AABBCollision.hpp"

#include "StreetEnd.hpp"
#include "Intersection.hpp"

#include <random>
#include <ctime>

Car::Car() {
    srand((unsigned int) time(NULL));
}

glm::vec3 Car::get_position() {
    glm::vec2 center = bb_pos + 0.5f * bb_size;
    return glm::vec3(center.x, center.y, 0.0f);
}

glm::quat Car::get_rotation() {
    return glm::angleAxis(glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
}

bool Car::collision(StreetEnd const &street_end) {
    return aabbCollision(bb_pos, bb_size, street_end.stop_bb_pos, street_end.stop_bb_size);
}

bool Car::bumper_collision(Car const &other) {
    return aabbCollision(bumper_bb_pos, bumper_bb_size, other.bb_pos, other.bb_size);
}

void Car::update(float elapsed, std::vector<StreetEnd> &street_ends, std::vector<Car> &cars) {
    total_time += elapsed;

    float t;
    float prevT;
    bool still_touching_end;

    switch (state) {
        case STRAIGHT_DRIVING:
            bb_pos += vel * elapsed;
            bumper_bb_pos.x = (bb_pos.x + 0.5f * bb_size.x) + -glm::sin(glm::radians(rotation)) * BUMPER_DISTANCE - 0.5f * bumper_bb_size.x;
            bumper_bb_pos.y = (bb_pos.y + 0.5f * bb_size.y) + glm::cos(glm::radians(rotation)) * BUMPER_DISTANCE - 0.5f * bumper_bb_size.y;
            for (auto &car : cars) {
                if (bumper_collision(car)) {
                    state = WAITING_FOR_CAR;
                }
            }

            for (auto &street_end : street_ends) {
                if (collision(street_end)) {
                    street_end.intersection->enqueue(this);
                    stopped_at = &street_end;

                    state = WAITING_FOR_INTERSECTION;

                    break;
                }
            }
            break;
        case IN_INTERSECTION_STRAIGHT:
            bb_pos += vel * elapsed;
            still_touching_end = false;
            for (auto &street_end : street_ends) {
                if (collision(street_end)) {
                    still_touching_end = true;
                }
            }
            if (!still_touching_end) state = STRAIGHT_DRIVING; 
            break;
        case IN_INTERSECTION_TURNING:
            t = (total_time - turn_start_time) / turn_duration;
            prevT = t - elapsed / turn_duration;

            if (t >= 1.0f) {
                t = 1.0f;
                state = STRAIGHT_DRIVING;
            }

            if (t >= 0.5f && prevT < 0.5f) {
                bb_size = glm::vec2(bb_size.y, bb_size.x);
                bumper_bb_size = glm::vec2(bumper_bb_size.y, bumper_bb_size.x);
            }

            if (turn_ytox) {
                bb_pos.x = (glm::sin(t * glm::pi<float>() * 0.5f + 1.5f * glm::pi<float>()) + 1) * (target_pos.x - start_pos.x) + start_pos.x - 0.5f * bb_size.x;
                bb_pos.y = glm::cos(t * glm::pi<float>() * 0.5f + 1.5f * glm::pi<float>()) * (target_pos.y - start_pos.y) + start_pos.y - 0.5f * bb_size.y;
            } else {
                bb_pos.x = glm::cos(t * glm::pi<float>() * 0.5f + 1.5f * glm::pi<float>()) * (target_pos.x - start_pos.x) + start_pos.x - 0.5f * bb_size.x;
                bb_pos.y = (glm::sin(t * glm::pi<float>() * 0.5f + 1.5f * glm::pi<float>()) + 1) * (target_pos.y - start_pos.y) + start_pos.y - 0.5f * bb_size.y;
            }

            rotation = t * target_rotation + (1.0f - t) * start_rotation;

            break;
        case WAITING_FOR_INTERSECTION:
            // do nothing
            break;
        case WAITING_FOR_CAR:
            state = STRAIGHT_DRIVING;

            for (auto &car : cars) {
                if (bumper_collision(car)) {
                    state = WAITING_FOR_CAR;
                    break;
                }
            }
            break;
    }

    transform->position = get_position();
    transform->rotation = get_rotation();
}

void Car::enter_intersection() {
    if (state == WAITING_FOR_INTERSECTION) {
        // pick a direction
        int ind = std::rand() % stopped_at->intersection->ends.size();
        while (stopped_at->intersection->ends[ind] == stopped_at) // no u turns
            ind = std::rand() % stopped_at->intersection->ends.size();

        start_pos = bb_pos + 0.5f * bb_size;
        target_pos = stopped_at->intersection->ends[ind]->go_target;

        if (target_pos.x != start_pos.x && target_pos.y != start_pos.y) {
            start_rotation = rotation;
            target_rotation = glm::degrees(glm::atan(-stopped_at->intersection->ends[ind]->go_direction.x, stopped_at->intersection->ends[ind]->go_direction.y));
            while (start_rotation - target_rotation > 180.0f) target_rotation += 360.0f;
            while (start_rotation - target_rotation < -180.0f) target_rotation -= 360.0f;
            vel = stopped_at->intersection->ends[ind]->go_direction * VELOCITY;

            turn_start_time = total_time;

            float rot_positive = rotation;
            while (rot_positive < 0.0f) rot_positive += 360.0f;
            while (rot_positive >= 360.0f) rot_positive -= 360.0f;
            turn_ytox = ((int) rot_positive) % 360 == 0 || ((int) rot_positive) % 360 == 180;

            float turn_length = 0.5f * glm::pi<float>() * glm::abs(target_pos.x - start_pos.x);
            turn_duration = turn_length / VELOCITY;

            state = IN_INTERSECTION_TURNING;
        }
        else {
            state = IN_INTERSECTION_STRAIGHT;
        }
    } else {
        throw std::runtime_error("enter_intersection called on car in state " + state);
    }
}