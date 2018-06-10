#include <iostream>
#include <fstream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h> // TO GET RID OF

#include <ow/shader_program.hpp>
#include <ow/camera_fps.hpp>
#include <ow/vertex.hpp>
#include <ow/lights_set.hpp>
#include <ow/directional_light.hpp>
#include <ow/point_light.hpp>
#include <ow/spotlight.hpp>
#include <ow/mesh.hpp>
#include <ow/texture.hpp>
#include <ow/model.hpp>
#include <ow/utils.hpp>
#include <gui/window.hpp>

#include "parametrical_object.hpp"

void process_input(gui::window& window, float dt);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

namespace {
	// settings
	constexpr unsigned int SCR_WIDTH = 800;
	constexpr unsigned int SCR_HEIGHT = 600;

	// initialize camera system
	// ------------------------
	ow::camera_fps camera{glm::vec3(0, 0, 3)};
	auto last_x = static_cast<float>(800) / 2.f;
	auto last_y = static_cast<float>(600) / 2.f;
	bool first_mouse = true;
}

int main() {
	gui::window window{ "IN55", SCR_WIDTH, SCR_HEIGHT, nullptr, nullptr };
	if (window.invalid()) {
		ow::logger << "Failed to create window" << std::endl;
		return EXIT_FAILURE;
	}

	window.make_context_current();
	window.set_framebuffer_size_callback(framebuffer_size_callback);
	window.set_cursor_pos_callback(mouse_callback);
	window.set_scroll_callback(scroll_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		ow::logger << "Failed to initialize GLAD" << std::endl;
		return EXIT_FAILURE;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	ow::check_errors("Failed to set GL_DEPTH_TEST.");

	// Init ImGui
	// ==========
	window.init_imgui();

	// load the texture
	// ----------------
	auto white_texture = std::make_shared<ow::texture>("resources/textures/white.jpg", ow::texture_type::diffuse);
	auto white_texture_spec = std::make_shared<ow::texture>("resources/textures/white.jpg", ow::texture_type::specular);

	// load shaders
	// ------------
	ow::shader_program prog{{
		{GL_VERTEX_SHADER, "phong_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "phong_frag.glsl"}
	}};

	// lights
	// ------
	ow::lights_set lights;
	lights.add_directional_light(std::make_shared<ow::directional_light>(glm::vec3(1.0f, -1.0f, -1.0f)));

	auto spotlight = std::make_shared<ow::spotlight>(
			camera.get_pos(), camera.get_front(),
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
			1.0, 0.07, 0.017
	);
	lights.add_spotlight(spotlight);

	prog.use();
	prog.set("materials_shininess", 32.f);
	lights.update_all(prog, glm::mat4());

	// Create the parametrical object
	// ------------------------------
	parametrical_object object{5};
	object.add_texture(white_texture);
	object.add_texture(white_texture_spec);

	// game loop
	// -----------
	float delta_time = 0.0f;	// time between current frame and last frame
	float last_frame = 0.0f; // time of last frame
	while (!window.should_close()) {
		auto current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// input
		// -----
		process_input(window, delta_time);

		spotlight->set_pos(camera.get_pos());
		spotlight->set_dir(camera.get_front());

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		ow::check_errors("Failed to set clear color.");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ow::check_errors("Failed to clear scr.");

		// activate shader program
		prog.use();

		// create transformations
		glm::mat4 view = camera.get_view_matrix();
		glm::mat4 proj = camera.get_proj_matrix(static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT));
		prog.set("view", view);
		prog.set("proj", proj);

		// update lights
		lights.update_position_and_direction(prog, view);

		{ // draw object
			glm::mat4 model{1.0f};
			model = glm::translate(model, glm::vec3(.3f));
			model = glm::scale(model, glm::vec3(1.f));
			prog.set("model", model);

			glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
			prog.set("normal_matrix", normal_matrix);

			object.draw(prog);
		}

		window.render();
	}

	return EXIT_SUCCESS;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
void process_input(gui::window& window, float dt) {
	if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		window.set_should_close(true);
	}

	if (window.get_key(GLFW_KEY_W) == GLFW_PRESS) {
		camera.process_movement(ow::FORWARD, dt);
	} else if (window.get_key(GLFW_KEY_S) == GLFW_PRESS) {
		camera.process_movement(ow::BACKWARD, dt);
	}

	if (window.get_key(GLFW_KEY_A) == GLFW_PRESS) {
		camera.process_movement(ow::LEFT, dt);
	} else if (window.get_key(GLFW_KEY_D) == GLFW_PRESS) {
		camera.process_movement(ow::RIGHT, dt);
	}

	if (window.get_key(GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.process_movement(ow::UP, dt);
	} else if (window.get_key(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera.process_movement(ow::DOWN, dt);
	}
}

// glfw: whenever the window size changed (by OS or user resize)
// this callback function executes
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* w, double xpos, double ypos) {
	auto xposf = static_cast<float>(xpos);
	auto yposf = static_cast<float>(ypos);

	if (first_mouse) { // this bool variable is initially set to true
		last_x = xposf;
		last_y = yposf;
		first_mouse = false;
	}

	auto xoffset = xposf - last_x;
	auto yoffset = last_y - yposf; // y is reversed
	last_x = xposf;
	last_y = yposf;

	if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		camera.process_mouse_movement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow*, double, double yoffset) {
	camera.process_mouse_scroll(static_cast<float>(yoffset));
}
