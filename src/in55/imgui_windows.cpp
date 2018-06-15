#include <string>

#include <imgui/imgui.h>

#include "imgui_windows.hpp"

void imgui_config_window(int* number_of_faces, float* angle_x, float* angle_z, float* scale,
                         std::vector<glm::vec3>* lamp_colors, glm::vec3* spotlight_color) {
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

	// lamps colors
	for (size_t i = 0; i < lamp_colors->size(); ++i) {
		float col[3] = { (*lamp_colors)[i].x, (*lamp_colors)[i].y, (*lamp_colors)[i].z };
		std::string text = "Light color " + std::to_string(i);
		ImGui::ColorEdit3(text.c_str(), col);
		(*lamp_colors)[i].x = col[0];
		(*lamp_colors)[i].y = col[1];
		(*lamp_colors)[i].z = col[2];
	}

	{ // spotlight colors
		float col[3] = { spotlight_color->x, spotlight_color->y, spotlight_color->z };
		ImGui::ColorEdit3("Spotlight color", col);
		spotlight_color->x = col[0];
		spotlight_color->y = col[1];
		spotlight_color->z = col[2];
	}

	ImGui::End();
}

