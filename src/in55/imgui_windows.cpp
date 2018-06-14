#include <cstdio>

#include <imgui/imgui.h>

#include "imgui_windows.hpp"

void imgui_config_window(int* number_of_faces, float* angle_x, float* angle_z, float* scale, std::vector<glm::vec3>& light_colors) {
	ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_FirstUseEver);
	static bool open = true;
	if (!ImGui::Begin("Configuraton", &open)) {
		// optimization: if the window is collapsed
		ImGui::End();
		return;
	}

	ImGui::PushItemWidth(-120); // Right align, keep 140 pixels for labels

	ImGui::Text("Object");
	ImGui::SliderInt("Number of faces", number_of_faces, 3, 30);

	ImGui::Separator();
	ImGui::Text("Model");
	ImGui::SliderFloat("Angle around X", angle_x, -2.f, 2.f);
	ImGui::SliderFloat("Angle around Z", angle_z, -2.f, 2.f);
	ImGui::SliderFloat("Scale", scale, 0.5f, 4.f);

	ImGui::Separator();
	ImGui::Text("Lights");

	for (size_t i = 0; i < light_colors.size(); ++i) {
		float col[3] = { light_colors[i].x, light_colors[i].y, light_colors[i].z };
		char buffer [50];
		sprintf(buffer, "Light color %ld", i);
		ImGui::ColorEdit3(buffer, col);
		light_colors[i].x = col[0];
		light_colors[i].y = col[1];
		light_colors[i].z = col[2];
	}

	ImGui::End();
}

