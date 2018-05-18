struct GLFWwindow;

namespace gui {

class imgui_impl {
public:
	static void init(GLFWwindow* window);
	static void shutdown();
	static void new_frame();
	static void render();

	static bool is_window_hovered();
	static bool is_window_focused();

	static void on_mouse_button(int button, int action, int mods);
	static void on_scroll(double xoffset, double yoffset);
	static void on_key(int key, int scancode, int action, int mods);
	static void on_char(unsigned int c);

private:
	static void invalidate_device_objects();
	static bool create_device_objects();
	static bool create_fonts_texture();

	static const char* get_clipboard_text(void *window);
	static void set_clipboard_text(void *window, const char *str);
};

}
