#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include "Intersection.hpp"
#include "StreetEnd.hpp"
#include "Car.hpp"
#include "Player.hpp"

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	void pick_pizza_loc();
	void pick_pizza_delivery_loc();
	void reset_level();

	//----- constants -----
	const float CAMERA_ANGLE = 30.0f;
	const float CAMERA_HEIGHT = 40.0f;
	const int BLOCK_LENGTH = 4; // each block is 4 street tiles long
	const int MAP_SIZE = 4; // 4 blocks
	const float ROAD_WIDTH = 7.0f;
	const float STOP_WIDTH = 0.5f;
	const int CARS = 50;
	const float PIZZA_DELIVERY_RADIUS = 7.0f;
	const float PIZZA_PICKUP_RADIUS = 2.0f;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//map:
	std::vector<Intersection> intersections;
	std::vector<StreetEnd> street_ends;
	std::vector<Car> cars;
	std::vector<Wall> walls;

	Player player;

	Scene::Transform *pizza_transform = nullptr;
	Scene::Transform *pizza_block_transform = nullptr;

	bool has_pizza = true;
	glm::vec2 pizza_loc;
	glm::vec2 pizza_delivery_loc;
	int pizzas_delivered = 0;

	bool game_over = false;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:

	//Scene::Transform *upper_leg = nullptr;
	//Scene::Transform *lower_leg = nullptr;
	//glm::quat hip_base_rotation;
	//glm::quat upper_leg_base_rotation;
	//glm::quat lower_leg_base_rotation;
	//float wobble = 0.0f;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
