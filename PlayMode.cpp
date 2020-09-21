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

	// spawn car objects

	for (int i = 0; i < CARS; i++) {
		cars.emplace_back();
		Car *car = &cars.back();

		car->bb_pos = glm::vec2(ROAD_WIDTH * 0.75f, ROAD_WIDTH * 3.5f) - car->bb_size * 0.5f;
		car->vel = glm::vec2(0.0f, car->VELOCITY);
	}
	for (auto &transform : scene.transforms) {
		if (transform.name.substr(0, 3) == "Car") {
			int ind = stoi(transform.name.substr(3));
			cars[ind].transform = &transform;
		}
	}

	//create camera
	Scene::Transform *t = new Scene::Transform;
	t->position = glm::vec3(0.0f, 0.0f, 75.0f);
	t->rotation = glm::normalize(glm::angleAxis(glm::radians(CAMERA_ANGLE), glm::vec3(1.0f, 0.0f, 0.0f)));
	scene.cameras.emplace_back(t);
	camera = &scene.cameras.back();
}

PlayMode::~PlayMode() {
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

	//slowly rotates through [0,1):
	// wobble += elapsed / 10.0f;
	// wobble -= std::floor(wobble);

	// hip->rotation = hip_base_rotation * glm::angleAxis(
	// 	glm::radians(5.0f * std::sin(wobble * 2.0f * float(M_PI))),
	// 	glm::vec3(0.0f, 1.0f, 0.0f)
	// );
	// upper_leg->rotation = upper_leg_base_rotation * glm::angleAxis(
	// 	glm::radians(7.0f * std::sin(wobble * 2.0f * 2.0f * float(M_PI))),
	// 	glm::vec3(0.0f, 0.0f, 1.0f)
	// );
	// lower_leg->rotation = lower_leg_base_rotation * glm::angleAxis(
	// 	glm::radians(10.0f * std::sin(wobble * 3.0f * 2.0f * float(M_PI))),
	// 	glm::vec3(0.0f, 0.0f, 1.0f)
	// );

	for (auto &intersection : intersections) {
		intersection.update(elapsed);
	}

	for (auto &car : cars) {
		car.update(elapsed, street_ends, cars);
	}

	//move camera:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 100.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		//glm::mat4x3 frame = camera->transform->make_local_to_parent();
		//glm::vec3 right = frame[0];
		//glm::vec3 up = frame[1];
		//glm::vec3 forward = frame[1];
		glm::vec3 right(1.0f, 0.0f, 0.0f);
		glm::vec3 forward(0.0f, 1.0f, 0.0f);

		camera->transform->position += move.x * right + move.y * forward;
	}

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
