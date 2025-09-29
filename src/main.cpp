#include "Window.h"
#include "Shader.h"
#include "raymath.h"
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <ctime>

struct Vertex
{
    Vector2 pos;   // offset of 0
    Vector3 col;   // offset of 8 (4 bytes for pos.x + 4 bytes for pos.y = 8)
};

// Assignment 1 object 1 -- white triangle (change these vertex colours from red to white)!!!
static const Vertex vertices_white[3] =
{
    { { -0.6f, -0.4f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.6f, -0.4f }, { 1.0f, 1.0f, 1.0f } },
    { {   0.f,  0.6f }, { 1.0f, 1.0f, 1.0f } }
};

// Assignment 1 object 2 -- rainbow triangle (done for you)
static const Vertex vertices_rainbow[3] =
{
    { { -0.6f, -0.4f }, { 1.0f, 0.0f, 0.0f } },
    { {  0.6f, -0.4f }, { 0.0f, 1.0f, 0.0f } },
    { {   0.f,  0.6f }, { 0.0f, 0.0f, 1.0f } }
};

static const Vector2 vertex_positions[3] =
{
    { -0.6f, -0.4f },
    { 0.6f, -0.4f },
    { 0.f,  0.6f }
};

static const Vector3 vertex_colors[3] =
{
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};

int main()
{
    CreateWindow(800, 800, "Graphics 1");
    
    GLuint a1_tri_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a1_triangle.vert");
    GLuint a1_tri_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a1_triangle.frag");
    GLuint a1_tri_shader = CreateProgram(a1_tri_vert, a1_tri_frag);

    GLuint vertex_buffer_rainbow_positions;
    GLuint vertex_buffer_rainbow_colors;
    glGenBuffers(1, &vertex_buffer_rainbow_positions);
    glGenBuffers(1, &vertex_buffer_rainbow_colors);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    // Can only have 1 vertex buffer bound at a time, so must unbind in order to prevent overwriting it

    GLuint vertex_buffer_white;
    glGenBuffers(1, &vertex_buffer_white);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_white);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_white), vertices_white, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    GLuint vertex_array_rainbow;
    glGenVertexArrays(1, &vertex_array_rainbow);
    glBindVertexArray(vertex_array_rainbow);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_positions);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_colors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);

    glBindVertexArray(GL_NONE);

    GLuint vertex_array_white;
    glGenVertexArrays(1, &vertex_array_white);
    glBindVertexArray(vertex_array_white);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_white);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));

    glBindVertexArray(GL_NONE);

    int object_index = 0;

    GLint u_color = glGetUniformLocation(a1_tri_shader, "u_color");
    GLint u_world = glGetUniformLocation(a1_tri_shader, "u_world");

    Matrix world = world = MatrixIdentity();

    // Generally you want to Scale * Rotate * Translate (order matters)!!!
    world = MatrixRotateZ(0.0f * DEG2RAD) * MatrixTranslate(0.0f, 0.0f, 0.0f);

    /* Loop until the user closes the window */
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE))
            SetWindowShouldClose(true);

        // Colors are represented as fractions between 0.0 and 1.0, so convert using a colour-picker tool accordingly!
        float r = 239.0f / 255.0f;
        float g = 136.0f / 255.0f;
        float b = 190.0f / 255.0f;
        float a = 1.0f;

        

        // Time in seconds since GLFW was initialized (use this with functions like sinf and cosf for repeating animations)
        float tt = Time();
        // x_offset is for when we want to oscillate the triangle form -1 to 1 in case 3
        float x_offset = sinf(tt);
        // this is for the angle of which the trangle will turn counter clockwise in case 4
        float rotationAngle = tt;

        int colorIndex = static_cast<int>(tt) % 3;

        /* Render here */
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);

        if (IsKeyPressed(KEY_SPACE))
        {
            ++object_index %= 5;
        }

        switch (object_index)
        {
        case 0:
            glUseProgram(a1_tri_shader);
            glUniform3f(u_color, 1.0f, 1.0f, 1.0f);

            // Must use MatrixToFloat to send mat4 since raylib Matrix memory is in a different layout than glsl mat4
            glUniformMatrix4fv(u_world, 1, GL_FALSE, MatrixToFloat(world));

            glBindVertexArray(vertex_array_white);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;

        case 1:
            glUseProgram(a1_tri_shader);
            glUniform3f(u_color, 0.8, 0.8f, 0.8f);
            glUniformMatrix4fv(u_world, 1, GL_FALSE, MatrixToFloat(world));
            glBindVertexArray(vertex_array_rainbow);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;

        case 2:// I did two methods of changing color of the triangle i like the first method the best
            // change to multiple colors acording to the sin wave 
            r = (sinf(tt * 1.0f) + 1.0f) * 0.5f;
            g = (sinf(tt * 2.0f) + 1.0f) * 0.5f;
            b = (sinf(tt * 3.0f) + 1.0f) * 0.5f;

            // change color every second to just red, green, blue every second 
            //switch (colorIndex)
            //{
            //case 0: r = 1.0f; g = 0.0, b = 0.0f; break; // for red
            //case 1: r = 0.0f; g = 1.0, b = 0.0f; break; // for green 
            //case 2: r = 0.0f; g = 0.0, b = 1.0f; break; // for blue
            //}

            glUseProgram(a1_tri_shader);
            glUniform3f(u_color, r, g, b);
            glUniformMatrix4fv(u_world, 1, GL_FALSE, MatrixToFloat(world));
            glBindVertexArray(vertex_array_white);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;

        case 3:
            
            world = MatrixTranslate(x_offset, 0.0f, 0.0f);

            glUseProgram(a1_tri_shader);
            glUniform3f(u_color, 0.4, 0.4f, 0.4f);
            glUniformMatrix4fv(u_world, 1, GL_FALSE, MatrixToFloat(world));
            glBindVertexArray(vertex_array_rainbow);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;

        case 4:

            world = MatrixRotateZ(rotationAngle);

            glUseProgram(a1_tri_shader);
            glUniform3f(u_color, 0.5, 0.5f, 0.5f);
            glUniformMatrix4fv(u_world, 1, GL_FALSE, MatrixToFloat(world));
            glBindVertexArray(vertex_array_rainbow);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        }

        // Called at end of the frame to swap buffers and update input
        Loop();
    }

    glDeleteVertexArrays(1, &vertex_array_rainbow);
    glDeleteVertexArrays(1, &vertex_array_white);
    glDeleteBuffers(1, &vertex_buffer_rainbow_positions);
    glDeleteBuffers(1, &vertex_buffer_rainbow_colors);
    glDeleteBuffers(1, &vertex_buffer_white);
    glDeleteProgram(a1_tri_shader);
    glDeleteShader(a1_tri_frag);
    glDeleteShader(a1_tri_vert);

    DestroyWindow();
    return 0;
}
