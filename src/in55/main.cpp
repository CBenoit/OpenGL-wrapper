#include <iostream>
#include <fstream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ow/shader_program.hpp>
#include <ow/camera_fps.hpp>
#include <ow/vertex.hpp>
#include <ow/lights_set.hpp>
#include <ow/directional_light.hpp>
#include <ow/point_light.hpp>
#include <ow/spotlight.hpp>
#include <ow/mesh.hpp>
#include <ow/texture.hpp>
#include <ow/model.hpp>


void process_input(GLFWwindow* window, float dt);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


namespace {

    // settings
    constexpr unsigned int SCR_WIDTH = 800;
    constexpr unsigned int SCR_HEIGHT = 600;

    // initialize camera system
    // ------------------------
    ow::camera_fps camera{glm::vec3(0, 0, 3)};
    auto last_x = static_cast<float>(SCR_WIDTH) / 2.f;
    auto last_y = static_cast<float>(SCR_HEIGHT) / 2.f;
    bool first_mouse = true;
}

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OSX to be happy
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IN55", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW winow" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return EXIT_FAILURE;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // game loop
    // -----------
    float delta_time = 0.0f;	// time between current frame and last frame
    float last_frame = 0.0f; // time of last frame
    while (!glfwWindowShouldClose(window)) {
        auto current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        // -----
        process_input(window, delta_time);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ...

        // glfw: swap buffers and poll IO events
        // -------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();

    return EXIT_SUCCESS;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
void process_input(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.process_movement(ow::FORWARD, dt);
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.process_movement(ow::BACKWARD, dt);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.process_movement(ow::LEFT, dt);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.process_movement(ow::RIGHT, dt);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.process_movement(ow::UP, dt);
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.process_movement(ow::DOWN, dt);
    }
}

// glfw: whenever the window size changed (by OS or user resize)
// this callback function executes
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    auto xposf = static_cast<float>(xpos);
    auto yposf = static_cast<float>(ypos);

    if (first_mouse) { // this bool variable is initially set to true
        last_x = xposf;
        last_y = yposf;
        first_mouse = false;
    }

    auto xoffset = xposf - last_x;
    auto yoffset = last_y - yposf; // y is reversed
    last_x = xposf;
    last_y = yposf;

    camera.process_mouse_movement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow*, double, double yoffset) {
    camera.process_mouse_scroll(static_cast<float>(yoffset));
}

