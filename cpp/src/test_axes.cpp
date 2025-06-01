#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../include/test_axes.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Axes::Axes(GLuint shader) {
    this->shader = shader;

    this->vertices = {
        // X axis
        -100.0f, 0.0f, 0.0f,
        100.0f, 0.0f, 0.0f,

        // Y axis
        0.0f, -100.0f, 0.0f,
        0.0f, 100.0f, 0.0f,

        // Z axis
        0.0f, 0.0f, -100.0f,
        0.0f, 0.0f, 100.0f
    };

    this->create_axes();
}

void Axes::create_axes() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Axes::draw_axes() {
    glBindVertexArray(this->VAO);

    glm::mat4 Model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "Model"), 1, GL_FALSE, glm::value_ptr(Model));

    // Draw X axis in Red
    glUniform4f(glGetUniformLocation(this->shader, "currentColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 0, 2);

    // Draw Y axis in Green
    glUniform4f(glGetUniformLocation(this->shader, "currentColor"), 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 2, 2);

    // Draw Z axis in Blue
    glUniform4f(glGetUniformLocation(this->shader, "currentColor"), 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);

    glBindVertexArray(0);
}
