#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <string>
#include <ctime>

std::mt19937 mt_rand((unsigned int) time(NULL));

// Because I modeled each object in its own file I have separate pnct/scene files for each object.
// I don't think this is ideal, but it works :P

GLuint car_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > car_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("car.pnct"));
	car_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint road_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > road_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("road.pnct"));
	road_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint block_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > block_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("block.pnct"));
	block_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint bike_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > bike_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("unicycle_rider.pnct"));
	bike_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint pizza_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > pizza_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("pizza.pnct"));
	pizza_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint pizza_block_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > pizza_block_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("pizza_block.pnct"));
	pizza_block_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

PlayMode::PlayMode() {

	// load drawables
	for (int x = 0; x < (BLOCK_LENGTH + 1) * MAP_SIZE + 1; x++) {
		for (int y = 0; y < (BLOCK_LENGTH + 1) * MAP_SIZE + 1; y++) {
			if (x % (BLOCK_LENGTH + 1) != 0 && y % (BLOCK_LENGTH + 1) != 0) {
				scene.load(data_path("block.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
					Mesh const &mesh = block_meshes->lookup(mesh_name);

					if (mesh_name == "block") {
						transform->position = transform->position + glm::vec3(x, y, 0.0f) * ROAD_WIDTH + glm::vec3(1.0f, 1.0f, 0.0f) * 0.5f * ROAD_WIDTH;
						transform->rotation = glm::normalize(glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
					}
					scene.drawables.emplace_back(transform);
					Scene::Drawable &drawable = scene.drawables.back();

					drawable.pipeline = lit_color_texture_program_pipeline;

					drawable.pipeline.vao = block_meshes_for_lit_color_texture_program;
					drawable.pipeline.type = mesh.type;
					drawable.pipeline.start = mesh.start;
					drawable.pipeline.count = mesh.count;
				});
			}
			else {
				scene.load(data_path("road.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
					Mesh const &mesh = road_meshes->lookup(mesh_name);
					
					if (mesh_name == "road" || x % (BLOCK_LENGTH + 1) != 0 || y % (BLOCK_LENGTH + 1) != 0) {
						if (mesh_name == "road") {
							transform->position = transform->position + glm::vec3(x, y, 0.0f) * ROAD_WIDTH + glm::vec3(1.0f, 1.0f, 0.0f) * 0.5f * ROAD_WIDTH;
							if (y % (BLOCK_LENGTH + 1) != 0)
								transform->rotation = glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
						}
						scene.drawables.emplace_back(transform);
						Scene::Drawable &drawable = scene.drawables.back();

						drawable.pipeline = lit_color_texture_program_pipeline;

						drawable.pipeline.vao = road_meshes_for_lit_color_texture_program;
						drawable.pipeline.type = mesh.type;
						drawable.pipeline.start = mesh.start;
						drawable.pipeline.count = mesh.count;
					}
				});
			}
		}
	}

	for (int i = 0; i < CARS; i++) {
		scene.load(data_path("car.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
			Mesh const &mesh = car_meshes->lookup(mesh_name);

			if (mesh_name == "body") {
				transform->name = "Car" + std::to_string(i);
			}

			scene.drawables.emplace_back(transform);
			Scene::Drawable &drawable = scene.drawables.back();

			drawable.pipeline = lit_color_texture_program_pipeline;

			drawable.pipeline.vao = car_meshes_for_lit_color_texture_program;
			drawable.pipeline.type = mesh.type;
			drawable.pipeline.start = mesh.start;
			drawable.pipeline.count = mesh.count;
		});
	}

	scene.load(data_path("unicycle_rider.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = bike_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = bike_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
	});

	scene.load(data_path("pizza.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = pizza_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = pizza_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
	});

	scene.load(data_path("pizza_block.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = pizza_block_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = pizza_block_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
	});

	// create street data structure

	intersections.reserve((MAP_SIZE + 1) * (MAP_SIZE + 1));
	street_ends.reserve((MAP_SIZE + 1) * (MAP_SIZE + 1) * 4);
	for (int x = 0; x < MAP_SIZE + 1; x++) {
		for (int y = 0; y < MAP_SIZE + 1; y++) {
			glm::vec2 intersection_pos = glm::vec2(x, y) * (float) (ROAD_WIDTH * (BLOCK_LENGTH + 1));
			glm::vec2 intersection_size(ROAD_WIDTH, ROAD_WIDTH);

			intersections.emplace_back();
			Intersection *intersection = &intersections.back();

			intersection->bb_pos = intersection_pos;
			intersection->bb_size = intersection_size;

			// left street end
			if (x > 0) {
				street_ends.emplace_back();
				StreetEnd *e = &street_ends.back();
				e->stop_bb_pos = intersection_pos + glm::vec2(-STOP_WIDTH, 0.0f);
				e->stop_bb_size = glm::vec2(STOP_WIDTH, 0.5f * ROAD_WIDTH);
				e->go_target = intersection_pos + glm::vec2(-STOP_WIDTH * 0.5f, 0.75f * ROAD_WIDTH);
				e->go_direction = glm::vec2(-1.0f, 0.0f);
				e->intersection = intersection;
				intersection->ends.push_back(e);
			}
			// right street end
			if (x < MAP_SIZE) {
				street_ends.emplace_back();
				StreetEnd *e = &street_ends.back();
				e->stop_bb_pos = intersection_pos + glm::vec2(intersection_size.x, 0.5f * ROAD_WIDTH);
				e->stop_bb_size = glm::vec2(STOP_WIDTH, 0.5f * ROAD_WIDTH);
				e->go_target = intersection_pos + glm::vec2(intersection_size.x + STOP_WIDTH * 0.5f, 0.25f * ROAD_WIDTH);
				e->go_direction = glm::vec2(1.0f, 0.0f);
				e->intersection = intersection;
				intersection->ends.push_back(e);
			}
			// bottom street end
			if (y > 0) {
				street_ends.emplace_back();
				StreetEnd *e = &street_ends.back();
				e->stop_bb_pos = intersection_pos + glm::vec2(0.5f * ROAD_WIDTH, -STOP_WIDTH);
				e->stop_bb_size = glm::vec2(0.5f * ROAD_WIDTH, STOP_WIDTH);
				e->go_target = intersection_pos + glm::vec2(0.25f * ROAD_WIDTH, -STOP_WIDTH * 0.5f);
				e->go_direction = glm::vec2(0.0f, -1.0f);
				e->intersection = intersection;
				intersection->ends.push_back(e);
			}
			// top street end
			if (y < MAP_SIZE) {
				street_ends.emplace_back();
				StreetEnd *e = &street_ends.back();
				e->stop_bb_pos = intersection_pos + glm::vec2(0.0f, intersection_size.y);
				e->stop_bb_size = glm::vec2(0.5f * ROAD_WIDTH, STOP_WIDTH);
				e->go_target = intersection_pos + glm::vec2(0.75f * ROAD_WIDTH, intersection_size.y + STOP_WIDTH * 0.5f);
				e->go_direction = glm::vec2(0.0f, 1.0f);
				e->intersection = intersection;
				intersection->ends.push_back(e);
			}
		}
	}

	// create walls

	// outer map walls
	const float MAP_W = ((MAP_SIZE + 1) * BLOCK_LENGTH + 1) * ROAD_WIDTH;
	const float MAP_H = ((MAP_SIZE + 1) * BLOCK_LENGTH + 1) * ROAD_WIDTH;
	const float WALL_W = 10.0f;
	walls.emplace_back(glm::vec2(0, -WALL_W), glm::vec2(MAP_W, WALL_W)); // bottom wall
	walls.emplace_back(glm::vec2(0, MAP_H), glm::vec2(MAP_W, WALL_W)); // top wall
	walls.emplace_back(glm::vec2(-WALL_W, 0), glm::vec2(WALL_W, MAP_H)); // left wall
	walls.emplace_back(glm::vec2(MAP_W, 0), glm::vec2(WALL_W, MAP_H)); // right wall

	// building/city block walls
	for (int x = 0; x < MAP_SIZE; x++) {
		for (int y = 0; y < MAP_SIZE; y++) {
			const glm::vec2 SIZE(BLOCK_LENGTH * ROAD_WIDTH, BLOCK_LENGTH * ROAD_WIDTH);
			walls.emplace_back(glm::vec2((x * (BLOCK_LENGTH + 1) + 1) * ROAD_WIDTH, (y * (BLOCK_LENGTH + 1) + 1) * ROAD_WIDTH), SIZE);
		}
	}

	// reset_level spawns player, resets score, and spawns cars

	reset_level();

	// link transforms to their objects

	for (auto &transform : scene.transforms) {
		if (transform.name.substr(0, 3) == "Car") {
			int ind = stoi(transform.name.substr(3));
			cars[ind].transform = &transform;
		}
		if (transform.name == "seat") {
			player.transform = &transform;
		}
		if (transform.name == "crust") {
			pizza_transform = &transform;
		}
		if (transform.name == "pizza_block") {
			pizza_block_transform = &transform;
		}
	}
	if (player.transform == nullptr) throw std::runtime_error("couldn't find player transform");
	if (pizza_transform == nullptr) throw std::runtime_error("couldn't find pizza transform");
	if (pizza_block_transform == nullptr) throw std::runtime_error("couldn't find pizza block transform");

	//create camera
	Scene::Transform *t = new Scene::Transform;
	t->position = glm::vec3(0.0f, 0.0f, CAMERA_HEIGHT);
	t->rotation = glm::normalize(glm::angleAxis(glm::radians(CAMERA_ANGLE), glm::vec3(1.0f, 0.0f, 0.0f)));
	scene.cameras.emplace_back(t);
	camera = &scene.cameras.back();

	pick_pizza_delivery_loc();
	pick_pizza_loc();
}

PlayMode::~PlayMode() {
}

void PlayMode::pick_pizza_loc() {
	float y = (float) (mt_rand() % (int) ((BLOCK_LENGTH + 1) * MAP_SIZE + 1)) * ROAD_WIDTH;
	float x = (float) (mt_rand() % (int) (MAP_SIZE + 1)) * (BLOCK_LENGTH + 1) * ROAD_WIDTH + ROAD_WIDTH * 0.5f;

	pizza_loc = glm::vec2(x, y);

	pizza_transform->position = glm::vec3(pizza_loc.x, pizza_loc.y, 1.0f);
}

void PlayMode::pick_pizza_delivery_loc() {
	float big_x = ((mt_rand() % MAP_SIZE) * (BLOCK_LENGTH + 1) + 1) * ROAD_WIDTH;
	float little_x = (mt_rand() % BLOCK_LENGTH) * ROAD_WIDTH;

	float big_y = ((mt_rand() % MAP_SIZE) * (BLOCK_LENGTH + 1) + 1) * ROAD_WIDTH;

	pizza_delivery_loc = glm::vec2(big_x + little_x + 0.5 * ROAD_WIDTH, big_y + 0.5 * ROAD_WIDTH);

	pizza_block_transform->position = glm::vec3(pizza_delivery_loc.x, pizza_delivery_loc.y, 2.0f);
}

void PlayMode::reset_level() {
	player.bb_pos = glm::vec2(0.5f * ROAD_WIDTH * ((BLOCK_LENGTH + 1) * MAP_SIZE + 1), 0.5f * ROAD_WIDTH * ((BLOCK_LENGTH + 1) * MAP_SIZE + 1)) - player.bb_size * 0.5f;
	game_over = false;
	pizzas_delivered = 0;
	has_pizza = true;

	cars.clear();

	for (int i = 0; i < CARS; i++) {
		cars.emplace_back();
		Car *car = &cars.back();

		float y = (float) (mt_rand() % (int) ((BLOCK_LENGTH + 1) * MAP_SIZE + 1)) * ROAD_WIDTH;
		float x = (float) (mt_rand() % (int) (MAP_SIZE + 1)) * (BLOCK_LENGTH + 1) * ROAD_WIDTH;
		car->bb_pos = glm::vec2(ROAD_WIDTH * 0.75f + x, y) - car->bb_size * 0.5f;
		car->vel = glm::vec2(0.0f, car->VELOCITY);
	}

	for (auto &intersection : intersections) {
		intersection.cars.clear();
	}
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_r) {
			if (game_over) reset_level();
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	if (!game_over) {
		for (auto &intersection : intersections) {
			intersection.update(elapsed);
		}

		for (auto &car : cars) {
			car.update(elapsed, street_ends, cars);
		}

		game_over = player.update(elapsed, Player::PlayerInput(up.pressed, down.pressed, left.pressed, right.pressed), cars, walls);
		if (game_over) std::cout << "Game over. Press R to try again" << std::endl;

		if (has_pizza) {
			pizza_transform->position = player.get_center() + glm::vec3(0.0f, 0.0f, 7.2f);

			if (glm::abs(pizza_delivery_loc.x - player.bb_pos.x) <= PIZZA_DELIVERY_RADIUS && glm::abs(pizza_delivery_loc.y - player.bb_pos.y) <= PIZZA_DELIVERY_RADIUS)
			{
				has_pizza = false;
				pick_pizza_loc();
				pick_pizza_delivery_loc();
				pizzas_delivered++;
				std::cout << "Pizzas delivered: " + std::to_string(pizzas_delivered) << std::endl;
			}
		} else if (glm::abs(pizza_loc.x - player.bb_pos.x) <= PIZZA_PICKUP_RADIUS && glm::abs(pizza_loc.y - player.bb_pos.y) <= PIZZA_PICKUP_RADIUS) {
			has_pizza = true;
		}
	}

	camera->transform->position.x = player.get_center().x;
	camera->transform->position.y = player.get_center().y - ROAD_WIDTH * 3.0f;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	GL_ERRORS();
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);
}
