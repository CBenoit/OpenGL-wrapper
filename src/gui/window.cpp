#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include <gui/imgui_impl.hpp>
#include <gui/window.hpp>

auto init_glfw_and_window(const char* title, int width, int height, GLFWmonitor* monitor, GLFWwindow* share);

auto init_glfw_and_window(const char* title, int width, int height, GLFWmonitor* monitor, GLFWwindow* share) {
	// initialize and configure glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OSX to be happy
	#endif

	return glfwCreateWindow(width, height, title, monitor, share);
}

gui::window::window(const char* title, int width, int height, GLFWmonitor* monitor, GLFWwindow* share) noexcept
		: window(init_glfw_and_window(title, width, height, monitor, share)) {
}

gui::window::window(GLFWwindow* glfw_window) : m_glfw_window(glfw_window) {
	if (invalid()) {
		return; // error!
	}

	// init some glfw parameters
	glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowUserPointer(m_glfw_window, this);

	// add default callback for imgui.
	auto mouseScrollCallback = [](GLFWwindow* w, double xPos, double yPos) {
		if (ImGui::GetIO().WantCaptureMouse) {
			gui::imgui_impl::on_scroll(xPos, yPos);
		} else {
			static_cast<window*>(glfwGetWindowUserPointer(w))->m_user_scroll_callback(w, xPos, yPos);
		}
	};
	glfwSetScrollCallback(m_glfw_window, mouseScrollCallback);

	auto keyCallback = [](GLFWwindow* w, int key, int scancode, int action, int mods) {
		if (ImGui::GetIO().WantCaptureKeyboard) {
			gui::imgui_impl::on_key(key, scancode, action, mods);
		} else {
			static_cast<window *>(glfwGetWindowUserPointer(w))->m_user_key_callback(w, key, scancode, action, mods);
		}
	};
	glfwSetKeyCallback(m_glfw_window, keyCallback);

	auto textCallback = [](GLFWwindow* w, unsigned int character) {
		if (ImGui::GetIO().WantTextInput) {
			gui::imgui_impl::on_char(character);
		} else {
			static_cast<window *>(glfwGetWindowUserPointer(w))->m_user_char_callback(w, character);
		}
	};
	glfwSetCharCallback(m_glfw_window, textCallback);

	auto mouseCallback = [](GLFWwindow* w, int button, int action, int mods) {
		if (!ImGui::GetIO().WantCaptureMouse) {
			gui::imgui_impl::on_mouse_button(button, action, mods);
		} else {
			static_cast<window *>(glfwGetWindowUserPointer(w))->m_user_mouse_callback(w, button, action, mods);
		}
	};
	glfwSetMouseButtonCallback(m_glfw_window, mouseCallback);

	auto cursorPosCallback = [](GLFWwindow* w, double xPos, double yPos) {
		if (!ImGui::GetIO().WantCaptureMouse) {
			static_cast<window *>(glfwGetWindowUserPointer(w))->m_user_cursor_pos_callback(w, xPos, yPos);
		}
	};
	glfwSetCursorPosCallback(m_glfw_window, cursorPosCallback);
}

gui::window::~window() {
	glfwTerminate();
	gui::imgui_impl::shutdown();
}

void gui::window::init_imgui() {
	ImGui::CreateContext();
	gui::imgui_impl::init(m_glfw_window);
	ImGui::GetIO().IniFilename = nullptr;
	gui::imgui_impl::new_frame();
}

void gui::window::render() {
	// imgui
	// -----
	static bool open = true;
	ImGui::ShowDemoWindow(&open);
	gui::imgui_impl::render();
	gui::imgui_impl::new_frame();

	// glfw: swap buffers and poll IO events
	// -------------------------------------
	glfwSwapBuffers(m_glfw_window);
	glfwPollEvents();
}

void gui::window::make_context_current() {
	glfwMakeContextCurrent(m_glfw_window);
}

void gui::window::set_input_mode(int mode, int value) {
	glfwSetInputMode(m_glfw_window, mode, value);
}

// ==== callback setters ====

void  gui::window::set_framebuffer_size_callback(GLFWframebuffersizefun callback) {
	glfwSetFramebufferSizeCallback(m_glfw_window, callback);
}

void  gui::window::set_window_close_callback(GLFWwindowclosefun callback) {
	glfwSetWindowCloseCallback(m_glfw_window, callback);
}

void  gui::window::set_window_focus_callback(GLFWwindowfocusfun callback) {
	glfwSetWindowFocusCallback(m_glfw_window, callback);
}

void gui::window::set_cursor_pos_callback(GLFWcursorposfun callback) {
	m_user_cursor_pos_callback = callback;
}

void gui::window::set_scroll_callback(GLFWscrollfun callback) {
	m_user_scroll_callback = callback;
}

void gui::window::set_key_callback(GLFWkeyfun callback) {
	m_user_key_callback = callback;
}

void gui::window::set_char_callback(GLFWcharfun callback) {
	m_user_char_callback = callback;
}

void gui::window::set_mouse_callback(GLFWmousebuttonfun callback) {
	m_user_mouse_callback = callback;
}
