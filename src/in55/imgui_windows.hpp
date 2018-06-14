#pragma once

#include <vector>

#include <glm/glm.hpp>

void imgui_config_window(int* number_of_faces, float* angle_x, float* angle_z, float* scale, std::vector<glm::vec3>& light_colors);
