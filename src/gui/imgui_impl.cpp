#include <cstddef>
#include <bitset>

#include <imgui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

#include <ow/shader_program.hpp>
#include <gui/imgui_impl.hpp>

namespace {
	GLFWwindow* s_window             = nullptr;
	double s_time                    = 0.f;
	std::bitset<3> s_mouse_pressed   = {0};
	float s_mouse_wheel              = 0.f;
	GLuint s_font_texture            = 0;
	unsigned int s_vbo_handle        = 0;
	unsigned int s_vao_handle        = 0;
	unsigned int s_elements_handle   = 0;
	ow::shader_program s_shader_program;
}

void gui::imgui_impl::init(GLFWwindow* window) {
	s_window = window;

	ImGuiIO& io                       = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab]           = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow]     = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow]    = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow]       = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow]     = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp]        = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown]      = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home]          = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End]           = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete]        = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace]     = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter]         = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape]        = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A]             = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C]             = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V]             = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X]             = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y]             = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z]             = GLFW_KEY_Z;

	// Set render callback
	io.SetClipboardTextFn             = imgui_impl::set_clipboard_text;
	io.GetClipboardTextFn             = imgui_impl::get_clipboard_text;
	io.ClipboardUserData              = s_window;
#ifdef _WIN32
	io.ImeWindowHandle                = glfwGetWin32Window(s_window);
#endif

}

void gui::imgui_impl::shutdown() {
	imgui_impl::invalidate_device_objects();
	ImGui::DestroyContext();
}

void gui::imgui_impl::render() {
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	auto framebuffer_width = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	auto framebuffer_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (framebuffer_width == 0 || framebuffer_height == 0)
		return;

	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Save OpenGL state to restore it after drawing imgui.
	GLint last_program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_active_texture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
	GLint last_array_buffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src;
	glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst;
	glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha;
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4];
	glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4];
	glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cullface = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup Imgui Render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup viewport from imgui information.
	glViewport(0, 0, framebuffer_width, framebuffer_height);

	glm::mat4x4 mat4x4 = glm::ortho(0.f, io.DisplaySize.x, io.DisplaySize.y, 0.f);

	// Load imgui program
	s_shader_program.use();
	s_shader_program.set("Texture", 0);
	s_shader_program.set("ProjMtx", mat4x4);

	glBindVertexArray(s_vao_handle);

	for (int n = 0; n < draw_data->CmdListsCount; n++) {
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = nullptr;

		glBindBuffer(GL_ARRAY_BUFFER, s_vbo_handle);
		glBufferData(GL_ARRAY_BUFFER,
		             static_cast<GLsizeiptr>(static_cast<long unsigned int>(cmd_list->VtxBuffer.Size) * sizeof(ImDrawVert)),
		             static_cast<const GLvoid*>(cmd_list->VtxBuffer.Data), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_elements_handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		             static_cast<GLsizeiptr>(static_cast<long unsigned int>(cmd_list->IdxBuffer.Size) * sizeof(ImDrawIdx)),
		             static_cast<const GLvoid*>(cmd_list->IdxBuffer.Data), GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback) {
				pcmd->UserCallback(cmd_list, pcmd);
			} else {
				glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<intptr_t>(pcmd->TextureId)));
				glScissor(static_cast<GLint>(pcmd->ClipRect.x),
				          static_cast<GLint>(static_cast<float>(framebuffer_height)- pcmd->ClipRect.w),
				          static_cast<GLsizei>(pcmd->ClipRect.z - pcmd->ClipRect.x),
				          static_cast<GLsizei>(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount),
				               sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified GL state
	glUseProgram(static_cast<GLuint>(last_program));
	glActiveTexture(static_cast<GLuint>(last_active_texture));
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));
	glBindVertexArray(static_cast<GLuint>(last_vertex_array));
	glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(last_array_buffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(last_element_array_buffer));
	glBlendEquationSeparate(static_cast<GLuint>(last_blend_equation_rgb), static_cast<GLuint>(last_blend_equation_alpha));
	glBlendFunc(static_cast<GLuint>(last_blend_src), static_cast<GLuint>(last_blend_dst));
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cullface) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], last_scissor_box[2], last_scissor_box[3]);
}

const char* gui::imgui_impl::get_clipboard_text(void *window) {
	return glfwGetClipboardString(static_cast<GLFWwindow*>(window));
}

void gui::imgui_impl::set_clipboard_text(void *window, const char *str) {
	glfwSetClipboardString(static_cast<GLFWwindow*>(window), str);
}

bool gui::imgui_impl::is_window_hovered() {
    return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

bool gui::imgui_impl::is_window_focused() {
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

void gui::imgui_impl::on_mouse_button(int button, int action, int /*mods*/) {
	if (action == GLFW_PRESS && button >= 0 && button < 3)
		s_mouse_pressed[static_cast<std::size_t>(button)] = true;
}

void gui::imgui_impl::on_scroll(double /*xoffset*/, double yoffset) {
	s_mouse_wheel += static_cast<float>(yoffset);
}

void gui::imgui_impl::on_key(int key, int /*scancode*/, int action, int mods) {
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void) mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void gui::imgui_impl::on_char(unsigned int c) {
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter(static_cast<ImWchar>(c));
}

bool gui::imgui_impl::create_fonts_texture() {
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width,
	                             &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &s_font_texture);
	glBindTexture(GL_TEXTURE_2D, s_font_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = reinterpret_cast<void*>(static_cast<intptr_t>(s_font_texture));

	// Restore state
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));

	return true;
}

bool gui::imgui_impl::create_device_objects() {
	// Backup OpenGL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	s_shader_program.put({
		{GL_VERTEX_SHADER,   "imgui_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "imgui_frag.glsl"}
	});

	glGenBuffers(1, &s_vbo_handle);
	glGenBuffers(1, &s_elements_handle);
	glGenVertexArrays(1, &s_vao_handle);

	glBindVertexArray(s_vao_handle);
	glBindBuffer(GL_ARRAY_BUFFER, s_vbo_handle);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, pos)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, uv)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, col)));
	glEnableVertexAttribArray(2);

	create_fonts_texture();

	// Restore modified OpenGL state
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));
	glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(last_array_buffer));
	glBindVertexArray(static_cast<GLuint>(last_vertex_array));

	return true;
}

void gui::imgui_impl::invalidate_device_objects() {
	if (s_vao_handle) {
		glDeleteVertexArrays(1, &s_vao_handle);
	}

	if (s_vbo_handle) {
		glDeleteBuffers(1, &s_vbo_handle);
	}

	if (s_elements_handle) {
		glDeleteBuffers(1, &s_elements_handle);
	}

	s_vao_handle = s_vbo_handle = s_elements_handle = 0;

	if (s_font_texture) {
		glDeleteTextures(1, &s_font_texture);
		ImGui::GetIO().Fonts->TexID = nullptr;
		s_font_texture = 0;
	}
}

void gui::imgui_impl::new_frame() {
	if (!s_font_texture)
		imgui_impl::create_device_objects();

	ImGuiIO& io = ImGui::GetIO();

	// Resize Imgui framebuffer.
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(s_window, &w, &h);
	glfwGetFramebufferSize(s_window, &display_w, &display_h);
	io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
	io.DisplayFramebufferScale = ImVec2(w > 0 ? static_cast<float>(display_w) / static_cast<float>(w) : 0.f, h > 0 ? static_cast<float>(display_h) / static_cast<float>(h) : 0.f);

	// get delta
	double current_time = glfwGetTime();
	io.DeltaTime = s_time > 0.0 ? static_cast<float>(current_time - s_time) : 1.0f / 60.0f;
	s_time = current_time;

	if (glfwGetWindowAttrib(s_window, GLFW_FOCUSED)) {
		// If the window is focused, set the mouse cursor position.
		double mouse_x, mouse_y;
		glfwGetCursorPos(s_window, &mouse_x, &mouse_y);
		io.MousePos = ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
	} else {
		// Else set it out of the window.
		io.MousePos = ImVec2(-1, -1);
	}

	// Test for event shorter than 1 frame.
	for (size_t i = 0; i < 3UL; i++) {
		io.MouseDown[i] = s_mouse_pressed[i] || glfwGetMouseButton(s_window, static_cast<int>(i)) != 0;
		s_mouse_pressed[i] = false;
	}

	io.MouseWheel = s_mouse_wheel;
	s_mouse_wheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	glfwSetInputMode(s_window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	// Start the frame
	ImGui::NewFrame();
}
