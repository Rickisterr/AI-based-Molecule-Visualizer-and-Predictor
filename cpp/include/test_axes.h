#ifndef TEST_AXES_H
#define TEST_AXES_H
#include <glad/glad.h>
#include <vector>

class Axes {
public:
    GLuint VAO, VBO;
    std::vector<float> vertices;
    GLuint shader;

    Axes(GLuint shader);
    void create_axes();
    void draw_axes();
};

#endif