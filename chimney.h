#ifndef CHIMNEY_H
#define CHIMNEY_H

#include <glad/glad.h>
#include "shader.h"

// Hollow chimney with inner and outer walls, no top.
// Vertex layout: location 0 = vec3 position, location 1 = vec3 normal, location 2 = vec2 uv

class Chimney {
public:
    Chimney() {
        // Outer and inner walls of chimney
        float vertices[] = {
            // ===== Outer walls =====
            // Front outer wall
            -0.25f, 0.0f, -0.25f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
             0.25f, 0.0f, -0.25f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
             0.25f, 0.6f, -0.25f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
            -0.25f, 0.6f, -0.25f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,

            // Right outer wall
             0.25f, 0.0f, -0.25f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
             0.25f, 0.0f,  0.25f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
             0.25f, 0.6f,  0.25f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
             0.25f, 0.6f, -0.25f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

             // Back outer wall
              0.25f, 0.0f,  0.25f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
             -0.25f, 0.0f,  0.25f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
             -0.25f, 0.6f,  0.25f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
              0.25f, 0.6f,  0.25f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,

              // Left outer wall
              -0.25f, 0.0f,  0.25f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
              -0.25f, 0.0f, -0.25f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
              -0.25f, 0.6f, -0.25f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
              -0.25f, 0.6f,  0.25f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

              // ===== Inner walls (inverted normals) =====
              // Front inner wall
              -0.15f, 0.0f, -0.15f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
               0.15f, 0.0f, -0.15f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
               0.15f, 0.6f, -0.15f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
              -0.15f, 0.6f, -0.15f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,

              // Right inner wall
               0.15f, 0.0f, -0.15f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
               0.15f, 0.0f,  0.15f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
               0.15f, 0.6f,  0.15f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
               0.15f, 0.6f, -0.15f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

               // Back inner wall
                0.15f, 0.0f,  0.15f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
               -0.15f, 0.0f,  0.15f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
               -0.15f, 0.6f,  0.15f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                0.15f, 0.6f,  0.15f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

                // Left inner wall
                -0.15f, 0.0f,  0.15f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
                -0.15f, 0.0f, -0.15f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
                -0.15f, 0.6f, -0.15f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
                -0.15f, 0.6f,  0.15f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

                // ===== Bottom face =====
                -0.25f, 0.0f,  0.25f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
                 0.25f, 0.0f,  0.25f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
                 0.25f, 0.0f, -0.25f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
                -0.25f, 0.0f, -0.25f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
        };

        unsigned int indices[] = {
            // Outer walls
            0,1,2, 2,3,0,
            4,5,6, 6,7,4,
            8,9,10, 10,11,8,
            12,13,14, 14,15,12,

            // Inner walls
            16,17,18, 18,19,16,
            20,21,22, 22,23,20,
            24,25,26, 26,27,24,
            28,29,30, 30,31,28,

            // Bottom
            32,33,34, 34,35,32
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));

        // normal (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        // uv (location = 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        glBindVertexArray(0);
    }

    ~Chimney() {
        if (EBO) glDeleteBuffers(1, &EBO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (VAO) glDeleteVertexArrays(1, &VAO);
    }

    void Draw(const Shader& shader, const glm::mat4& model) const {
        shader.use();
        shader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 90, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


private:
    GLuint VAO = 0, VBO = 0, EBO = 0;
};

#endif // CHIMNEY_H
