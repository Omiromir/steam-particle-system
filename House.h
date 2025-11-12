#ifndef HOUSE_H
#define HOUSE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "stb_image.h"

class House {
public:
    unsigned int VAO_walls, VBO_walls, VAO_roof, VBO_roof;
    unsigned int textureWalls, textureRoof;

    House(const char* wallTexPath, const char* roofTexPath) {
        setupWalls();
        setupRoof();
        loadTexture(wallTexPath, textureWalls);
        loadTexture(roofTexPath, textureRoof);
    }

    void Draw(Shader& shader, glm::mat4 view, glm::mat4 projection) {
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // Draw walls
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureWalls);
        glBindVertexArray(VAO_walls);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 6 faces * 2 triangles * 3 vertices
        glBindVertexArray(0);

        // Draw roof
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureRoof);
        glBindVertexArray(VAO_roof);
        glDrawArrays(GL_TRIANGLES, 0, 18); // 2 front/back triangles + 2 side quads = 6 triangles
        glBindVertexArray(0);
    }

private:
    void setupWalls() {
        float wallVertices[] = {
            // positions          // texcoords
            // Front face
            -1.0f,0.0f, 1.0f,    0.0f,0.0f,
             1.0f,0.0f, 1.0f,    1.0f,0.0f,
             1.0f,1.0f, 1.0f,    1.0f,1.0f,
             1.0f,1.0f, 1.0f,    1.0f,1.0f,
            -1.0f,1.0f, 1.0f,    0.0f,1.0f,
            -1.0f,0.0f, 1.0f,    0.0f,0.0f,
            // Back face
            -1.0f,0.0f,-1.0f,    0.0f,0.0f,
             1.0f,0.0f,-1.0f,    1.0f,0.0f,
             1.0f,1.0f,-1.0f,    1.0f,1.0f,
             1.0f,1.0f,-1.0f,    1.0f,1.0f,
            -1.0f,1.0f,-1.0f,    0.0f,1.0f,
            -1.0f,0.0f,-1.0f,    0.0f,0.0f,
            // Left face
            -1.0f,0.0f,-1.0f,    0.0f,0.0f,
            -1.0f,0.0f, 1.0f,    1.0f,0.0f,
            -1.0f,1.0f, 1.0f,    1.0f,1.0f,
            -1.0f,1.0f, 1.0f,    1.0f,1.0f,
            -1.0f,1.0f,-1.0f,    0.0f,1.0f,
            -1.0f,0.0f,-1.0f,    0.0f,0.0f,
            // Right face
             1.0f,0.0f,-1.0f,    0.0f,0.0f,
             1.0f,0.0f, 1.0f,    1.0f,0.0f,
             1.0f,1.0f, 1.0f,    1.0f,1.0f,
             1.0f,1.0f, 1.0f,    1.0f,1.0f,
             1.0f,1.0f,-1.0f,    0.0f,1.0f,
             1.0f,0.0f,-1.0f,    0.0f,0.0f,
             // Bottom
             -1.0f,0.0f,-1.0f,    0.0f,0.0f,
              1.0f,0.0f,-1.0f,    1.0f,0.0f,
              1.0f,0.0f, 1.0f,    1.0f,1.0f,
              1.0f,0.0f, 1.0f,    1.0f,1.0f,
             -1.0f,0.0f, 1.0f,    0.0f,1.0f,
             -1.0f,0.0f,-1.0f,    0.0f,0.0f,
        };
        glGenVertexArrays(1, &VAO_walls);
        glGenBuffers(1, &VBO_walls);
        glBindVertexArray(VAO_walls);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_walls);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void setupRoof() {
        float roofVertices[] = {
            // Front triangle
            -1.0f,1.0f, 1.0f,    0.0f,0.0f,
             1.0f,1.0f, 1.0f,    1.0f,0.0f,
             0.0f,1.6f, 1.0f,    0.5f,1.0f,
             // Back triangle
             -1.0f,1.0f,-1.0f,    0.0f,0.0f,
              1.0f,1.0f,-1.0f,    1.0f,0.0f,
              0.0f,1.6f,-1.0f,    0.5f,1.0f,
              // Left side quad (2 triangles)
              -1.0f,1.0f, 1.0f,    0.0f,0.0f,
              -1.0f,1.0f,-1.0f,    1.0f,0.0f,
               0.0f,1.6f,-1.0f,    1.0f,1.0f,
               0.0f,1.6f,-1.0f,    1.0f,1.0f,
               0.0f,1.6f, 1.0f,    0.0f,1.0f,
              -1.0f,1.0f, 1.0f,    0.0f,0.0f,
              // Right side quad (2 triangles)
               1.0f,1.0f, 1.0f,    0.0f,0.0f,
               1.0f,1.0f,-1.0f,    1.0f,0.0f,
               0.0f,1.6f,-1.0f,    1.0f,1.0f,
               0.0f,1.6f,-1.0f,    1.0f,1.0f,
               0.0f,1.6f, 1.0f,    0.0f,1.0f,
               1.0f,1.0f, 1.0f,    0.0f,0.0f
        };
        glGenVertexArrays(1, &VAO_roof);
        glGenBuffers(1, &VBO_roof);
        glBindVertexArray(VAO_roof);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_roof);
        glBufferData(GL_ARRAY_BUFFER, sizeof(roofVertices), roofVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void loadTexture(const char* path, GLuint& texID) {
        int w, h, c;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path, &w, &h, &c, 0);
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (c == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
};

#endif
