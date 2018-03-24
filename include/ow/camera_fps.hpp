#pragma once

#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ow {

// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system
// specific input methods
enum camera_movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
constexpr float DEFAULT_YAW         = -M_PI / 2.0;
constexpr float DEFAULT_PITCH       =  0.0f;
constexpr float DEFAULT_SPEED       =  2.5f;
constexpr float DEFAULT_SENSITIVITY =  0.001f;
constexpr float DEFAULT_FOV         =  M_PI / 4.0;

// An abstract camera class that processes input and calculates
// the corresponding Euler Angles, Vectors and Matrices for use
// in OpenGL
class camera_fps
{
public:
    camera_fps(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 get_view_matrix() const {
        return glm::lookAt(m_pos, m_pos + m_front, m_up);
    }

    // Returns the projection matrix calculated using camera's fov and given aspect
    glm::mat4 get_proj_matrix(float aspect) const {
        return glm::perspective(m_fov, aspect, 0.1f, 100.0f);
    }

    // Processes input received from any keyboard-like input system.
    // Accepts input parameter in the form of camera defined ENUM
    // (to abstract it from windowing systems)
    void process_movement(camera_movement direction, float dt);

    // Processes input received from a mouse input system.
    // Expects the offset value in both the x and y direction.
    void process_mouse_movement(float xoffset, float yoffset, GLboolean constrain_pitch = true);

    // Processes input received from a mouse scroll-wheel event.
    // Only requires input on the vertical wheel-axis
    void process_mouse_scroll(float yoffset);

    glm::vec3 get_pos() const {
        return m_pos;
    }

    glm::vec3 get_front() const {
        return m_front;
    }

    float get_yaw() const {
        return m_yaw;
    }

    float get_pitch() const {
        return m_pitch;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void _update_camera_vectors();

private:
    // Camera Attributes
    glm::vec3 m_pos;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_world_up;

    // Euler Angles
    float m_yaw;
    float m_pitch;

    // Camera options
    float m_mov_speed;
    float m_mouse_sensitivity;
    float m_fov;
};

}
