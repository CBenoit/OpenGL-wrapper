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
#include "ImguiImpl.hpp"

namespace {
	GLFWwindow* s_window             = nullptr;
	double s_time                    = 0.f;
	std::bitset<3> s_mousePressed    = {0};
	float s_mouseWheel               = 0.f;
	GLuint s_fontTexture             = 0;
	ow::shader_program s_shader_program;
	unsigned int s_vboHandle         = 0;
	unsigned int s_vaoHandle         = 0;
	unsigned int s_elementsHandle    = 0;
}

void gui::ImguiImpl::init(GLFWwindow* window) {
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
	io.SetClipboardTextFn             = ImguiImpl::setClipboardText;
	io.GetClipboardTextFn             = ImguiImpl::getClipboardText;
	io.ClipboardUserData              = s_window;
#ifdef _WIN32
	io.ImeWindowHandle                = glfwGetWin32Window(s_window);
#endif

}

void gui::ImguiImpl::shutdown() {
	ImguiImpl::invalidateDeviceObjects();
	ImGui::DestroyContext();
}

void gui::ImguiImpl::render() {
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	auto framebufferWidth = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	auto framebufferHeight = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (framebufferWidth == 0 || framebufferHeight == 0)
		return;

	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	// Save OpenGL state to restore it after drawing imgui.
	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
	GLint lastTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	GLint lastActiveTexture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &lastActiveTexture);
	GLint lastArrayBuffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	GLint lastElementArrayBuffer;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &lastElementArrayBuffer);
	GLint lastVertexArray;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);
	GLint lastBlendSrc;
	glGetIntegerv(GL_BLEND_SRC, &lastBlendSrc);
	GLint lastBlendDst;
	glGetIntegerv(GL_BLEND_DST, &lastBlendDst);
	GLint lastBlendEquationRgb;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &lastBlendEquationRgb);
	GLint lastBlendEquationAlpha;
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &lastBlendEquationAlpha);
	GLint lastViewport[4];
	glGetIntegerv(GL_VIEWPORT, lastViewport);
	GLint lastScissorBox[4];
	glGetIntegerv(GL_SCISSOR_BOX, lastScissorBox);
	GLboolean lastEnableBlend = glIsEnabled(GL_BLEND);
	GLboolean lastEnableCullFace = glIsEnabled(GL_CULL_FACE);
	GLboolean lastEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
	GLboolean lastEnableScissorTest = glIsEnabled(GL_SCISSOR_TEST);

	// Setup Imgui Render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup viewport from imgui information.
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glm::mat4x4 mat4x4 = glm::ortho(0.f, io.DisplaySize.x, io.DisplaySize.y, 0.f);

	// Load imgui program
	s_shader_program.use();
	s_shader_program.set("Texture", 0);
	s_shader_program.set("ProjMtx", mat4x4);

	glBindVertexArray(s_vaoHandle);

	for (int n = 0; n < drawData->CmdListsCount; n++) {
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = nullptr;

		glBindBuffer(GL_ARRAY_BUFFER, s_vboHandle);
		glBufferData(GL_ARRAY_BUFFER,
		             static_cast<GLsizeiptr>(static_cast<long unsigned int>(cmd_list->VtxBuffer.Size) * sizeof(ImDrawVert)),
		             static_cast<const GLvoid*>(cmd_list->VtxBuffer.Data), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_elementsHandle);
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
				          static_cast<GLint>(static_cast<float>(framebufferHeight)- pcmd->ClipRect.w),
				          static_cast<GLsizei>(pcmd->ClipRect.z - pcmd->ClipRect.x),
				          static_cast<GLsizei>(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount),
				               sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified GL state
	glUseProgram(static_cast<GLuint>(lastProgram));
	glActiveTexture(static_cast<GLuint>(lastActiveTexture));
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(lastTexture));
	glBindVertexArray(static_cast<GLuint>(lastVertexArray));
	glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(lastArrayBuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(lastElementArrayBuffer));
	glBlendEquationSeparate(static_cast<GLuint>(lastBlendEquationRgb), static_cast<GLuint>(lastBlendEquationAlpha));
	glBlendFunc(static_cast<GLuint>(lastBlendSrc), static_cast<GLuint>(lastBlendDst));
	if (lastEnableBlend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (lastEnableCullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (lastEnableDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (lastEnableScissorTest) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(lastViewport[0], lastViewport[1], lastViewport[2], lastViewport[3]);
	glScissor(lastScissorBox[0], lastScissorBox[1], lastScissorBox[2], lastScissorBox[3]);
}

const char* gui::ImguiImpl::getClipboardText(void* window) {
	return glfwGetClipboardString(static_cast<GLFWwindow*>(window));
}

void gui::ImguiImpl::setClipboardText(void* window, const char* str) {
	glfwSetClipboardString(static_cast<GLFWwindow*>(window), str);
}

bool gui::ImguiImpl::isWindowHovered() {
    return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

bool gui::ImguiImpl::isWindowFocused() {
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

void gui::ImguiImpl::onMouseButton(int button, int action, int /*mods*/) {
	if (action == GLFW_PRESS && button >= 0 && button < 3)
		s_mousePressed[static_cast<std::size_t>(button)] = true;
}

void gui::ImguiImpl::onScroll(double /*xoffset*/, double yoffset) {
	s_mouseWheel += static_cast<float>(yoffset);
}

void gui::ImguiImpl::onKey(int key, int /*scancode*/, int action, int mods) {
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

void gui::ImguiImpl::onChar(unsigned int c) {
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter(static_cast<ImWchar>(c));
}

bool gui::ImguiImpl::createFontsTexture() {
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width,
	                             &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &s_fontTexture);
	glBindTexture(GL_TEXTURE_2D, s_fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = reinterpret_cast<void*>(static_cast<intptr_t>(s_fontTexture));

	// Restore state
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));

	return true;
}

bool gui::ImguiImpl::createDeviceObjects() { // TODO
	// Backup OpenGL state
	GLint lastTexture, lastArrayBuffer, lastVertexArray;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);

	s_shader_program.put({
		{GL_VERTEX_SHADER,   "imgui_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "imgui_frag.glsl"}
	});

	glGenBuffers(1, &s_vboHandle);
	glGenBuffers(1, &s_elementsHandle);
	glGenVertexArrays(1, &s_vaoHandle);

	glBindVertexArray(s_vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, s_vboHandle);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, pos)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, uv)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, col)));
	glEnableVertexAttribArray(2);

	createFontsTexture();

	// Restore modified OpenGL state
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(lastTexture));
	glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(lastArrayBuffer));
	glBindVertexArray(static_cast<GLuint>(lastVertexArray));

	return true;
}

void gui::ImguiImpl::invalidateDeviceObjects() {
	if (s_vaoHandle) {
		glDeleteVertexArrays(1, &s_vaoHandle);
	}

	if (s_vboHandle) {
		glDeleteBuffers(1, &s_vboHandle);
	}

	if (s_elementsHandle) {
		glDeleteBuffers(1, &s_elementsHandle);
	}

	s_vaoHandle = s_vboHandle = s_elementsHandle = 0;

	if (s_fontTexture) {
		glDeleteTextures(1, &s_fontTexture);
		ImGui::GetIO().Fonts->TexID = nullptr;
		s_fontTexture = 0;
	}
}

void gui::ImguiImpl::newFrame() {
	if (!s_fontTexture)
		ImguiImpl::createDeviceObjects();

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
		io.MouseDown[i] = s_mousePressed[i] || glfwGetMouseButton(s_window, static_cast<int>(i)) != 0;
		s_mousePressed[i] = false;
	}

	io.MouseWheel = s_mouseWheel;
	s_mouseWheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	glfwSetInputMode(s_window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	// Start the frame
	ImGui::NewFrame();
}
