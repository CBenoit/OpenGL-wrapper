struct GLFWwindow;

namespace gui {
	class ImguiImpl {
	public:
		static void init(GLFWwindow* window);
		static void shutdown();
		static void newFrame();
		static void render();

		static bool isWindowHovered();
		static bool isWindowFocused();

		static void onMouseButton(int button, int action, int mods);
		static void onScroll(double xoffset, double yoffset);
		static void onKey(int key, int scancode, int action, int mods);
		static void onChar(unsigned int c);

	private:
		static void invalidateDeviceObjects();
		static bool createDeviceObjects();
		static bool createFontsTexture();

		static const char* getClipboardText(void* window);
		static void setClipboardText(void* window, const char* str);

	};
}
