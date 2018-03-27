#include <iostream>
#include <cstddef> // offsetof
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ow/shader_program.hpp>
#include <ow/camera_fps.hpp>
#include <ow/vertex.hpp>
#include <ow/texture.hpp>

void process_input(GLFWwindow* window, float dt);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// initialize camera system
// ------------------------
ow::camera_fps camera{glm::vec3(0, 0, 3)};
float last_x = SCR_WIDTH / 2;
float last_y = SCR_HEIGHT / 2;
bool first_mouse = true;

int main(int argc, char* argv[]) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OSX to be happy
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Getting started", nullptr, nullptr);
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

    // call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // load the texture
    // ----------------
    auto wooden_texture = std::make_shared<ow::texture>("ressources/textures/wooden_container.jpg");

    // load shaders
    // ------------
    ow::shader_program shader_program{"shaders/basic.vs", "shaders/basic.fs"};

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    ow::vertex vertices[] = {
        // front
        {glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0)},
        {glm::vec3(-0.5,  0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1)},
        {glm::vec3(0.5,  -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0)},
        {glm::vec3(0.5,   0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1)},

        // back
        {glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0)},
        {glm::vec3(-0.5,  0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1)},
        {glm::vec3(0.5,  -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0)},
        {glm::vec3(0.5,   0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1)},

        // left
        {glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0)},
        {glm::vec3(-0.5,  0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1)},
        {glm::vec3(-0.5, -0.5,  0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0)},
        {glm::vec3(-0.5,  0.5,  0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1)},

        // right
        {glm::vec3(0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0)},
        {glm::vec3(0.5,  0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1)},
        {glm::vec3(0.5, -0.5,  0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0)},
        {glm::vec3(0.5,  0.5,  0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1)},

        // top
        {glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(0, 0)},
        {glm::vec3(-0.5, 0.5,  0.5), glm::vec3(0, 1, 0), glm::vec2(0, 1)},
        {glm::vec3( 0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
        {glm::vec3( 0.5, 0.5,  0.5), glm::vec3(0, 1, 0), glm::vec2(1, 1)},

        // bottom
        {glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 0)},
        {glm::vec3(-0.5, -0.5,  0.5), glm::vec3(0, -1, 0), glm::vec2(0, 1)},
        {glm::vec3( 0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(1, 0)},
        {glm::vec3( 0.5, -0.5,  0.5), glm::vec3(0, -1, 0), glm::vec2(1, 1)},
    };

    GLuint indices[] = {
        // front face
        0, 1, 2, // first triangle
        1, 2, 3,  // second triangle
        // back face
        4, 5, 6,
        5, 6, 7,
        // left face
        8, 9, 10,
        9, 10, 11,
        // right face
        12, 13, 14,
        13, 14, 15,
        // top face
        16, 17, 18,
        17, 18, 19,
        // bottom face
        20, 21, 22,
        21, 22, 23,
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ow::vertex), (void*) 0);
    glEnableVertexAttribArray(0);
    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ow::vertex), (void*) offsetof(ow::vertex, tex_coords));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards
    // we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't
    // unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // create cubes positions
    // ----------------------
    glm::vec3 cube_positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // tell opengl for each sampler to which texture unit it belongs
    // to (only has to be done once)
    // -------------------------------------------------------------
    shader_program.use();
    shader_program.set_int("tex", 0);

    // game loop
    // -----------
    float delta_time = 0.0f;	// time between current frame and last frame
    float last_frame = 0.0f; // time of last frame
    while (!glfwWindowShouldClose(window)) {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        // -----
        process_input(window, delta_time);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wooden_texture->id);

        // activate shader program
        shader_program.use();

        // create transformations
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 proj = camera.get_proj_matrix(static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT));

        // Seeing as we only have a single  VAO there's
        // no need to bind it every time, but we'll do
        // so to keep things a bit more organized
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model{1.0f};
            model = glm::translate(model, cube_positions[i]);
            model = glm::rotate(model, static_cast<float>(0.2 * i), glm::vec3(1.0f, 0.3f, 0.5f));
            glm::mat4 MVP = proj * view * model;
            shader_program.set_mat("MVP", MVP);

            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
        }

        // glfw: swap buffers and poll IO events
        // -------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();

    return EXIT_SUCCESS;
}

// glfw: whenever the window size changed (by OS or user resize)
// this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (first_mouse) { // this bool variable is initially set to true
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // y is reversed
    last_x = xpos;
    last_y = ypos;

    camera.process_mouse_movement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.process_mouse_scroll(yoffset);
}

