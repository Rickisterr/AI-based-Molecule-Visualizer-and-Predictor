// For linux, run run_simulator.sh and for windows powershell, run run_simulator.ps1
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/test_axes.h"

struct Configurations {
    float FoV = 40;
    float nearClip = 0.1;
    float farClip = 100;
    float deltaAngle = 0.05;
    float zoom_dist = 0.5;
} config;

struct Environment {
    glm::vec3 cameraPosn = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraFocus = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
} rend_env;

struct Initializations {
    const char* windowName = "Molecule Visualizer with Predictor";
    int windowWidth = 1920;
    int windowHeight = 1080;
    float phi = 0.0;
    float theta = 0.0;
    float radius = glm::length(rend_env.cameraPosn);
} init;

const char* vertexShaderScript = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 Posn;
        uniform mat4 Model;
        uniform mat4 View;
        uniform mat4 Perspective;
        void main() {
            gl_Position = Perspective * View * Model * vec4(Posn, 1.0);
        }
    )glsl";

const char* fragmentShaderScript = R"glsl(
        #version 330 core
        out vec4 fragColor;
        uniform vec4 currentColor;
        void main() {
            fragColor = currentColor;
        }
    )glsl";

void processWindowCloseInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

void processWindowCloseInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    return;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    float aspectRatio = (float)width / (float)height;

    // Perspective matrix being reset on changing window size
    glm::mat4 Perspective = glm::perspective(glm::radians(config.FoV), aspectRatio, config.nearClip, config.farClip);
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // Rotating about origin
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        init.theta -= config.deltaAngle;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        init.theta += config.deltaAngle;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        init.phi += config.deltaAngle;
        init.phi = glm::clamp(init.phi, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f); // prevent flipping
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        init.phi -= config.deltaAngle;
        init.phi = glm::clamp(init.phi, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f); // prevent flipping
    }

    // Zooming in and out
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        init.radius -= config.zoom_dist;
        init.radius = glm::max(init.radius, 0.1f); // prevent negative radius
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        init.radius += config.zoom_dist;
    }

    // Convert spherical coordinates to Cartesian coordinates
    float x = init.radius * cos(init.phi) * sin(init.theta);
    float y = init.radius * sin(init.phi);
    float z = init.radius * cos(init.phi) * cos(init.theta);

    rend_env.cameraPosn = glm::vec3(x, y, z);
}


// Main Program
int main() {
    // Beginning OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Initializing window
    GLFWwindow* window = glfwCreateWindow(init.windowWidth, init.windowHeight, init.windowName, NULL, NULL);
    float aspectRatio = (float)init.windowWidth / (float)init.windowHeight;

    if (window == NULL) {
        printf("Failed to create window\n");
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // If window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Loading and linking shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderScript, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex Shader Compilation Failed: %s \n", infoLog);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderScript, NULL);
    glCompileShader(fragmentShader);

    int success2;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success2);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment Shader Compilation Failed: %s \n", infoLog);
    }

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    // Deleting shader programs after linking together
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Using shaders
    glUseProgram(shader);
    glEnable(GL_DEPTH_TEST);

    // Transformation matrices intialized
    glm::mat4 View = glm::lookAt(rend_env.cameraPosn, rend_env.cameraFocus, rend_env.upVector);
    glm::mat4 Perspective = glm::perspective(glm::radians(config.FoV), aspectRatio, config.nearClip, config.farClip);

    // Linking matrices to equivalent variables in shader script
    glUniformMatrix4fv(glGetUniformLocation(shader, "View"), 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix4fv(glGetUniformLocation(shader, "Perspective"), 1, GL_FALSE, glm::value_ptr(Perspective));

    glfwSetKeyCallback(window, keyCallBack);

    // Initialize models
    Axes axes(shader);

    // Main Render Loop
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processWindowCloseInput(window);

        // Keep using shader in case extra shaders for different models developed later
        // Not much overhead if there are no other shaders so does not matter otherwise
        glUseProgram(shader);

        glm::mat4 View = glm::lookAt(rend_env.cameraPosn, rend_env.cameraFocus, rend_env.upVector);
        glUniformMatrix4fv(glGetUniformLocation(shader, "View"), 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(glGetUniformLocation(shader, "Perspective"), 1, GL_FALSE, glm::value_ptr(Perspective));

        // Draw Models
        // Draw Axes
        axes.draw_axes();

        // Checks for any remaining events before loop iteration ends
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // End programs
    glfwTerminate();
    return 0;
}