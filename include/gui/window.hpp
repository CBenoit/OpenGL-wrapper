#pragma once

#include <functional>

struct GLFWwindow;
struct GLFWmonitor;

namespace gui {

class window {
public:
	// initialize glfw with good defaults and create the glfw window.
	explicit window(const char* title, int width = 800, int height = 600,
					GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr) noexcept;

	// assume glfw has been initialized and take a pre-created glfw window.
	explicit window(GLFWwindow* glfw_window);

	window(const window&) = delete;
	window& operator=(const window&) = delete;
	~window();

	void init_imgui();

	bool valid() { return m_glfw_window != nullptr; }

	bool invalid() { return m_glfw_window == nullptr; }

	bool should_close() { return glfwWindowShouldClose(m_glfw_window) != 0; }

	void set_should_close(bool value) { return glfwSetWindowShouldClose(m_glfw_window, value); }

	int get_key(int key) { return glfwGetKey(m_glfw_window, key); }

	void render();

	void make_context_current();

	void set_input_mode(int mode, int value);

	void set_framebuffer_size_callback(GLFWframebuffersizefun callback);

	void set_window_close_callback(GLFWwindowclosefun callback);

	void set_window_focus_callback(GLFWwindowfocusfun callback);

	void set_cursor_pos_callback(GLFWcursorposfun callback);

	void set_scroll_callback(GLFWscrollfun callback);

	void set_key_callback(GLFWkeyfun callback);

	void set_char_callback(GLFWcharfun callback);

	void set_mouse_callback(GLFWmousebuttonfun callback);

private:
	GLFWwindow* m_glfw_window{nullptr};

	GLFWcursorposfun m_user_cursor_pos_callback = [](auto...){};

	GLFWscrollfun m_user_scroll_callback = [](auto...){};

	GLFWkeyfun m_user_key_callback = [](auto...){};

	GLFWcharfun m_user_char_callback = [](auto...){};

	GLFWmousebuttonfun m_user_mouse_callback = [](auto...){};
};

}
