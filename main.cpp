// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "shader.h"
#include "chimney.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ---------- Perlin noise ----------
float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
float grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
int p[512];
void initNoise() {
    int permutation[] = { 151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,
        69,142,8,99,37,240,21,10,23,190,6,148,247,120,
        234,75,0,26,197,62,94,252,219,203,117,35,11,32,
        57,177,33,88,237,149,56,87,174,20,125,136,171,
        168,68,175,74,165,71,134,139,48,27,166,77,146,
        158,231,83,111,229,122,60,211,133,230,220,105,
        92,41,55,46,245,40,244,102,143,54,65,25,63,161,
        1,216,80,73,209,76,132,187,208,89,18,169,200,
        196,135,130,116,188,159,86,164,100,109,198,173,
        186,3,64,52,217,226,250,124,123,5,202,38,147,
        118,126,255,82,85,212,207,206,59,227,47,16,58,
        17,182,189,28,42,223,183,170,213,119,248,152,
        2,44,154,163,70,221,153,101,155,167,43,172,9,
        129,22,39,253,19,98,108,110,79,113,224,232,178,
        185,112,104,218,246,97,228,251,34,242,193,238,
        210,144,12,191,179,162,241,81,51,145,235,249,
        14,239,107,49,192,214,31,181,199,106,157,184,
        84,204,176,115,121,50,45,127,4,150,254,138,236,
        205,93,222,114,67,29,24,72,243,141,128,195,78,
        66,215,61,156,180 };
    for (int i = 0; i < 256; i++) p[256 + i] = p[i] = permutation[i];
}
float perlin(float x, float y, float z) {
    int X = (int)floor(x) & 255, Y = (int)floor(y) & 255, Z = (int)floor(z) & 255;
    x -= floor(x); y -= floor(y); z -= floor(z);
    float u = fade(x), v = fade(y), w = fade(z);
    int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
    int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
    float res = glm::mix(glm::mix(glm::mix(grad(p[AA], x, y, z),
        grad(p[BA], x - 1, y, z), u),
        glm::mix(grad(p[AB], x, y - 1, z),
            grad(p[BB], x - 1, y - 1, z), u), v),
        glm::mix(glm::mix(grad(p[AA + 1], x, y, z - 1),
            grad(p[BA + 1], x - 1, y, z - 1), u),
            glm::mix(grad(p[AB + 1], x, y - 1, z - 1),
                grad(p[BB + 1], x - 1, y - 1, z - 1), u), v), w);
    return (res + 1.0f) / 2.0f;
}
glm::vec3 noiseVelocity(glm::vec3 pos, float time) {
    float scale = 0.8f;
    float nX = perlin(pos.x * scale + time * 0.2f, pos.y * scale, pos.z * scale) - 0.5f;
    float nZ = perlin(pos.z * scale, pos.x * scale + time * 0.3f, pos.y * scale) - 0.5f;
    return glm::vec3(nX, 0.0f, nZ) * 1.2f;
}

// ---------- Particle ----------
struct Particle {
    glm::vec3 pos, vel;
    float life, size;
};

// ---------- Tuning ----------
const int MAX_PARTICLES = 1000;
const float CHIMNEY_Y = 0.5f;
const float SPAWN_RADIUS_BASE = 0.03f;
const float SPAWN_RADIUS_MAX = 0.25f;
const float UPWARD_SPEED_MIN = 1.0f;
const float UPWARD_SPEED_MAX = 1.8f;
const float HORIZONTAL_SPREAD = 0.3f;
const float LIFE_SPAN = 22.0f;
const float SIZE_BASE = 0.15f;
const float SIZE_GROWTH = 1.7f;
bool smokeActive = true;

// spread heights
const float SPREAD_START_HEIGHT = 2.0f;
const float SPREAD_FULL_HEIGHT = 6.0f;

// ---------- Decl ----------
void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);
void respawnParticle(Particle&);

// ---------- Main ----------
int main() {
    initNoise();
    srand((unsigned)time(nullptr));

    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Chimney Smoke", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // load GL functions AFTER context is current
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cerr << "Failed to init GLAD\n"; return -1; }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    // optional: glEnable(GL_CULL_FACE);

    // ---------- SHADERS ----------
    // particle shader (existing)
    Shader particleShader("billboard.vert", "billboard.frag", "billboard.geom");
    // chimney shader (new)
    Shader chimneyShader("chimney.vert", "chimney.frag");

    // Now it's safe to create GL objects that depend on a valid GL context
    Chimney chimney;

    // ---------- load smoke texture ----------
    int tw, th, tc;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("smoke.png", &tw, &th, &tc, 4);
    if (!data) { std::cerr << "Failed to load smoke.png\n"; return -1; }
    GLuint texSmoke;
    glGenTextures(1, &texSmoke);
    glBindTexture(GL_TEXTURE_2D, texSmoke);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ---------- load chimney texture (brick) ----------
    int cw, ch, cc;
    unsigned char* cdata = stbi_load("brick_diffuse.jpg", &cw, &ch, &cc, 0);
    if (!cdata) {
        std::cerr << "Warning: Failed to load chimney texture 'brick_diffuse.jpg'. Using solid color.\n";
    }
    GLuint texChimney = 0;
    if (cdata) {
        glGenTextures(1, &texChimney);
        glBindTexture(GL_TEXTURE_2D, texChimney);
        GLenum format = (cc == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, cw, ch, 0, format, GL_UNSIGNED_BYTE, cdata);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(cdata);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    // ---------- particle GPU buffer ----------
    struct GPUParticle { float px, py, pz, size, life; };
    std::vector<Particle> particles;
    std::vector<GPUParticle> gpuData(MAX_PARTICLES);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GPUParticle) * MAX_PARTICLES, nullptr, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(4 * sizeof(float)));
    glBindVertexArray(0);

    glm::mat4 modelIdentity = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.f / 720.f, 0.1f, 100.0f);
    glm::vec3 camPos(0.0f, 2.0f, 6.0f), target(0.0f, 1.0f, 0.0f), worldUp(0.0f, 1.0f, 0.0f);

    float lastTime = (float)glfwGetTime();
    float spawnTimer = 0.0f;

    // set chimney shader texture unit once (static)
    chimneyShader.use();
    chimneyShader.setInt("tex", 0);

    // particle shader: set smokeTex unit
    particleShader.use();
    particleShader.setInt("smokeTex", 0);

    while (!glfwWindowShouldClose(window)) {
        float now = (float)glfwGetTime();
        float dt = now - lastTime;
        lastTime = now;
        processInput(window);

        // --- spawn in puffs/trains ---
        if (smokeActive) {
            spawnTimer += dt;
            if (spawnTimer >= 0.12f) {
                spawnTimer = 0.0f;
                int burst = 4; // particles per puff
                for (int i = 0; i < burst && (int)particles.size() < MAX_PARTICLES; ++i) {
                    particles.emplace_back();
                    respawnParticle(particles.back());
                }
            }
        }

        // --- update particles (flow logic preserved) ---
        for (auto& p : particles) {
            p.life += dt / LIFE_SPAN;
            if (p.life >= 1.0f) {
                respawnParticle(p);
                continue;
            }

            // Natural upward drift
            p.vel += glm::vec3(0.0f, 0.5f * dt, 0.0f);

            // Smooth turbulent motion using Perlin flow
            glm::vec3 flow = noiseVelocity(p.pos, now);
            float height = p.pos.y - CHIMNEY_Y;
            float hFactor = glm::clamp(height / 4.0f, 0.0f, 1.0f);

            // Flow intensity grows with height
            p.vel += flow * dt * (0.4f + 1.6f * hFactor);

            // Gradual horizontal expansion based on height
            glm::vec2 radialDir = glm::normalize(glm::vec2(p.pos.x + 1e-6f, p.pos.z + 1e-6f));
            glm::vec3 radialPush = glm::vec3(radialDir.x, 0.0f, radialDir.y) * HORIZONTAL_SPREAD * hFactor * dt;
            p.vel += radialPush;

            // Damping and motion integration
            p.vel *= 0.995f;
            p.pos += p.vel * dt;

            // Particle size grows with height
            p.size = SIZE_BASE * (1.0f + (SIZE_GROWTH - 1.0f) * hFactor);
        }


        // prepare GPU data
        int activeCount = (int)particles.size();
        for (int i = 0; i < activeCount; ++i) {
            const auto& p = particles[i];
            gpuData[i].px = p.pos.x;
            gpuData[i].py = p.pos.y;
            gpuData[i].pz = p.pos.z;
            gpuData[i].size = p.size;
            gpuData[i].life = p.life;
        }

        // upload GPU data
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if (activeCount > 0)
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GPUParticle) * activeCount, gpuData.data());

        // camera basis
        glm::mat4 view = glm::lookAt(camPos, target, worldUp);
        glm::vec3 forward = glm::normalize(target - camPos);
        glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
        glm::vec3 up = glm::cross(right, forward);

        // --- render: sky, chimney (opaque), smoke (transparent) ---
        glClearColor(0.55f, 0.75f, 0.95f, 1.0f); // sky color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- draw chimney (opaque) ---
        chimneyShader.use();
        glm::mat4 chimneyModel = glm::mat4(1.0f);
        chimneyModel = glm::translate(chimneyModel, glm::vec3(0.0f, CHIMNEY_Y - 0.5f, 0.0f)); // align top to CHIMNEY_Y
        chimneyShader.setMat4("model", chimneyModel);
        chimneyShader.setMat4("view", view);
        chimneyShader.setMat4("proj", projection);

        // lighting uniforms
        glm::vec3 lightDir = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.25f));
        glm::vec3 lightColor = glm::vec3(1.0f, 0.95f, 0.9f);
        glm::vec3 ambient = glm::vec3(0.35f, 0.35f, 0.4f);
        chimneyShader.setVec3("lightDir", lightDir);
        chimneyShader.setVec3("lightColor", lightColor);
        chimneyShader.setVec3("ambient", ambient);

        if (texChimney != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texChimney);
        }
        else {
            // no texture loaded: bind smoke texture as placeholder (optional)
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texSmoke);
        }

        chimney.Draw();

        // --- draw smoke (transparent) ---
        particleShader.use();
        particleShader.setMat4("model", modelIdentity);
        particleShader.setMat4("view", view);
        particleShader.setMat4("proj", projection);
        particleShader.setVec3("camRight", right);
        particleShader.setVec3("camUp", up);
        particleShader.setVec3("tint", glm::vec3(0.95f, 0.95f, 1.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texSmoke);

        glDepthMask(GL_FALSE);
        glBindVertexArray(vao);
        if (activeCount > 0)
            glDrawArrays(GL_POINTS, 0, activeCount);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup (textures + buffers)
    if (texChimney) glDeleteTextures(1, &texChimney);
    glDeleteTextures(1, &texSmoke);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();
    return 0;
}

// ---------- Respawn ----------
void respawnParticle(Particle& p) {
    float angle = (rand() % 1000) / 1000.0f * 2.0f * 3.14159265359f;
    float r = ((rand() % 1000) / 1000.0f) * SPAWN_RADIUS_BASE;
    p.pos = glm::vec3(cos(angle) * r, CHIMNEY_Y, sin(angle) * r);
    float up = UPWARD_SPEED_MIN + ((rand() % 1000) / 1000.0f) * (UPWARD_SPEED_MAX - UPWARD_SPEED_MIN);
    p.vel = glm::vec3(((rand() % 1000) / 1000.0f - 0.5f) * 0.02f, up, ((rand() % 1000) / 1000.0f - 0.5f) * 0.02f);
    p.life = 0.0f;
    p.size = SIZE_BASE * (0.8f + (rand() % 1000) / 1000.0f * 0.4f);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool gPressedLast = false;
    static bool hPressedLast = false;

    bool gPressedNow = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
    bool hPressedNow = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS;

    // Toggle smoke on 'G' key press
    if (gPressedNow && !gPressedLast)
        smokeActive = true;

    // Stop smoke on 'H' key press
    if (hPressedNow && !hPressedLast)
        smokeActive = false;

    gPressedLast = gPressedNow;
    hPressedLast = hPressedNow;
}


void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}
