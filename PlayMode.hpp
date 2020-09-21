#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include "Intersection.hpp"
#include "StreetEnd.hpp"
#include "Car.hpp"

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- constants -----
	const float CAMERA_ANGLE = 35.0f;
	const int BLOCK_LENGTH = 4; // each block is 4 street tiles long
	const int MAP_SIZE = 4; // 4 blocks
	const float ROAD_WIDTH = 10.0f;
	const float STOP_WIDTH = 0.5f;
	const int CARS = 100;

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
