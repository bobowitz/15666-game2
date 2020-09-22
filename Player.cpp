#include "Player.hpp"

#include "AABBCollision.hpp"

glm::vec3 Player::get_center() {
    glm::vec2 center_2d = bb_pos + bb_size * 0.5f;
    return glm::vec3(center_2d.x, center_2d.y, 0.0f);
}

bool Player::update(float elapsed, PlayerInput input, std::vector<Car> const &cars, std::vector<Wall> const &walls) {
    if (input.up_pressed) {
        vel += glm::vec2(0.0f, 1.0f) * ACCELERATION * elapsed * elapsed;
    }
    if (input.down_pressed) {
        vel += glm::vec2(0.0f, -1.0f) * ACCELERATION * elapsed * elapsed;
    }
    if (input.left_pressed) {
        vel += glm::vec2(-1.0f, 0.0f) * ACCELERATION * elapsed * elapsed;
    }
    if (input.right_pressed) {
        vel += glm::vec2(1.0f, 0.0f) * ACCELERATION * elapsed * elapsed;
    }

    float mag = glm::length(vel);
    if (mag <= MIN_SPEED)
    {
        if (!input.up_pressed && !input.down_pressed) {
            vel = glm::vec2(0.0f, 0.0f);
        }
    }
    else if (mag > VELOCITY) {
        vel = (vel / mag) * VELOCITY;
    }
    else
        vel += glm::normalize(-vel) * DECELERATION * elapsed * elapsed;

    bb_pos.x += vel.x * elapsed;
    for (auto const &wall : walls) {
        if (aabbCollision(bb_pos, bb_size, wall.pos, wall.size)) {
            if (vel.x > 0.0f) {
                bb_pos.x = wall.pos.x - bb_size.x;
            }
            else if (vel.x < 0.0f) {
                bb_pos.x = wall.pos.x + wall.size.x;
            }
            vel.x = 0.0f;
        }
    }

    bb_pos.y += vel.y * elapsed;
    for (auto const &wall : walls) {
        if (aabbCollision(bb_pos, bb_size, wall.pos, wall.size)) {
            if (vel.y > 0.0f) {
                bb_pos.y = wall.pos.y - bb_size.y;
            }
            else if (vel.y < 0.0f) {
                bb_pos.y = wall.pos.y + wall.size.y;
            }
            vel.y = 0.0f;
        }
    }

    for (auto const &car : cars) {
        if (aabbCollision(bb_pos, bb_size, car.bb_pos, car.bb_size)) {
            return true;
        }
    }

    transform->position.x = get_center().x;
    transform->position.y = get_center().y;

    if (mag > MIN_SPEED)
        shown_rotation += 0.1f * (glm::atan(vel.y, vel.x) - shown_rotation);
    transform->rotation = glm::angleAxis(shown_rotation + glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
    transform->scale = glm::vec3(0.75f, 0.75f, 0.75f);

    return false;
}