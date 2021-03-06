#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <ow/shader_program.hpp>
#include <ow/camera_fps.hpp>
#include <ow/lights_set.hpp>
#include <ow/directional_light.hpp>
#include <ow/point_light.hpp>
#include <ow/texture.hpp>
#include <ow/skybox.hpp>
#include <ow/utils.hpp>
#include <gui/window.hpp>

#include "parametrical_object.hpp"
#include "cube.hpp"
#include "imgui_windows.hpp"

void process_input(gui::window& window, float dt);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

namespace {
	// settings
	constexpr unsigned int SCREEN_WIDTH = 800;
	constexpr unsigned int SCREEN_HEIGHT = 600;

	// initialize camera system
	// ------------------------
	ow::camera_fps camera{glm::vec3(0, 0, 3)};
	auto last_x = static_cast<float>(800) / 2.f;
	auto last_y = static_cast<float>(600) / 2.f;
	bool first_mouse = true;
}

int main() {
	gui::window window{ "IN55", SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr };
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
	auto white_diffuse = std::make_shared<ow::texture>("resources/textures/white.jpg", ow::texture_type::diffuse);
	auto white_spec = std::make_shared<ow::texture>("resources/textures/white.jpg", ow::texture_type::specular);

	// load shaders
	// ------------
	ow::shader_program phong_prog{{
		{GL_VERTEX_SHADER, "phong_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "phong_skybox_frag.glsl"}
	}};

	ow::shader_program lamp_prog{{
		{GL_VERTEX_SHADER, "lamp_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "lamp_frag.glsl"}
	}};

	ow::shader_program skybox_prog{{
		{GL_VERTEX_SHADER, "skybox_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "skybox_frag.glsl"}
	}};

	// lights
	// ------
	ow::lights_set lights;

	// so that the object is never completely black.
	lights.add_directional_light(std::make_shared<ow::directional_light>(
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(1.f),
			glm::vec3(0.0),
			glm::vec3(0.0)
	));

	glm::vec3 spotlight_color{ 0.f, 0.f, 0.f };
	auto spotlight = std::make_shared<ow::spotlight>(
			spotlight_color, spotlight_color,
			camera.get_pos(), camera.get_front(),
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
			1.0, 0.07, 0.017
	);
	lights.add_spotlight(spotlight);

	// add lamps
	std::vector<glm::vec3> lamp_colors;
	std::vector<std::shared_ptr<ow::point_light>> point_lights;
	{
		std::array positions = {
				glm::vec3( 3.f,  3.f,  -1.f),
				glm::vec3( -3.f, 3.f, -1.f),
				glm::vec3(0.f,  3.f, 3.0f),
		};
		lamp_colors.reserve(positions.size());

		for (auto& pos : positions) {
			glm::vec3 color{ 1.0f, 1.0f, 1.0f };
			lamp_colors.push_back(color);
			auto light = std::make_shared<ow::point_light>(color, color, pos, 1.0, 0.14, 0.07);
			lights.add_point_light(light);
			point_lights.push_back(light);
		}
	}

	cube lamp_mesh;

	phong_prog.use();
	phong_prog.set("materials_shininess", 32.f);
	lights.update_all(phong_prog, glm::mat4());

	// Create the parametrical object
	// ------------------------------
	int number_of_faces = 5;
	float angle_x = 0;
	float angle_z = 0;
	float scale = 1.f;
	auto object = std::make_unique<parametrical_object>(number_of_faces);
	object->add_texture(white_diffuse);
	object->add_texture(white_spec);

	// Skybox
	// ------
	// create a skybox texture from several images.
	auto skybox = std::make_shared<ow::skybox>("resources/textures/skybox");
	// cube wrapping the camera with sky texture.
	auto skybox_cube = std::make_unique<cube>();

	// bind the skybox texture in the skybox shader program.
	skybox_prog.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->id);
	skybox_prog.set("skybox", 0);

	// configure phong program to handle reflections on the skybox
	phong_prog.use();
	glActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->id);
	phong_prog.set("skybox", 15);

	// game loop
	// ---------
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

		// activate phong shader program
		phong_prog.use();

		// create transformations
		glm::mat4 view = camera.get_view_matrix();
		glm::mat4 proj = camera.get_proj_matrix(static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT));
		phong_prog.set("view", view);
		phong_prog.set("proj", proj);

		// update lights
		lights.update_all(phong_prog, view);

		{ // draw object
			glm::mat4 model{1.0f};
			model = glm::scale(model, glm::vec3(scale));
			model = glm::rotate(model, angle_x, glm::vec3(1.0, 0.0, 0.0));
			model = glm::rotate(model, angle_z, glm::vec3(0.0, 0.0, 1.0));
			phong_prog.set("model", model);

			glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
			phong_prog.set("normal_matrix", normal_matrix);

			object->draw(phong_prog);
		}

		// draw lamps
		lamp_prog.use();
		for (const auto& pt_light : point_lights) {
			glm::mat4 model{1.0f};
			model = glm::translate(model, pt_light->get_pos());
			model = glm::scale(model, glm::vec3(.2f));
			lamp_prog.set("MVP", proj * view * model);
			lamp_prog.set("color", pt_light->get_diffuse());

			lamp_mesh.draw(lamp_prog);
		}

		{ // draw skybox as last
			skybox_prog.use();
			glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
			glm::mat4 no_translation_view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
			skybox_prog.set("VP", proj * no_translation_view);
			skybox_cube->draw(skybox_prog);
			glDepthFunc(GL_LESS); // set back to default
		}

		// imgui window
		int old_number_of_faces = number_of_faces;
		imgui_config_window(&number_of_faces, &angle_x, &angle_z, &scale, &lamp_colors, &spotlight_color);

		window.render();

		// apply config
		// ------------

		// parametrical object
		if (old_number_of_faces != number_of_faces) {
			object = std::make_unique<parametrical_object>(number_of_faces);
			object->add_texture(white_diffuse);
			object->add_texture(white_spec);
		}

		// light colors
		for (size_t i = 0; i < lamp_colors.size(); ++i) {
			point_lights[i]->set_diffuse(lamp_colors[i]);
			point_lights[i]->set_specular(lamp_colors[i]);
		}
		spotlight->set_diffuse(spotlight_color);
		spotlight->set_specular(spotlight_color);
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		camera.process_mouse_movement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow*, double, double yoffset) {
	camera.process_mouse_scroll(static_cast<float>(yoffset));
}
