🏠 Chimney Smoke Simulation 

Project: Real-time Chimney Smoke Simulation
Environment: Windows 10/11 — MSYS2 MinGW64 — OpenGL 4.5 — GLFW 3.3 — GLAD — GLM — stb_image

🎯 Overview

This project simulates realistic chimney smoke using a GPU-accelerated particle system in OpenGL.
The smoke flows naturally upward, expands gradually, and dissipates with time — following Perlin noise to create smooth, organic motion.

A 3D chimney model is also rendered with texture beneath the smoke emitter, providing visual context.

🧠 Features

✅ Smooth, continuous particle-based smoke
✅ Perlin noise-driven motion for natural turbulence
✅ Gradual spread — smoke rises high before dispersing
✅ “Train-like” particle emission — bursts form smoke puffs
✅ Textured chimney model for realism
✅ Adjustable parameters for tuning speed, size, and lifespan

⚙️ Build Environment

OS: Windows 10/11

Compiler: g++ (GCC via MSYS2 MinGW64)

Dependencies:

GLFW
 – window/context management

GLAD
 – OpenGL loader

GLM
 – math library

stb_image.h
 – texture loading

📁 Project Structure
ChimneySmoke/
│
├── main.cpp             # Core smoke logic and rendering loop
├── shader.h             # Shader program utility class
├── chimney.h/.cpp       # Chimney model definition and draw logic
├── billboard.vert       # Vertex shader for smoke billboards
├── billboard.frag       # Fragment shader for smoke transparency
├── billboard.geom       # Geometry shader for billboard orientation
├── smoke.png            # Smoke texture (RGBA)
├── chimney_texture.jpg  # Chimney brick texture
└── README.md            # This file

🔧 Compilation & Execution
Step 1: Clone and Enter Directory
git clone https://github.com/yourusername/ChimneySmoke.git
cd ChimneySmoke

Step 2: Build

Make sure you have GLFW, GLM, and GLAD set up in your include and lib paths.

g++ main.cpp chimney.cpp -o chimney_smoke.exe -lglfw -lGL -ldl -lX11 -pthread

Step 3: Run
./chimney_smoke.exe

🌫️ Smoke Behavior

Each particle:

Spawns near chimney top (CHIMNEY_Y)

Rises with an initial upward velocity

Wobbles due to Perlin noise (noiseVelocity)

Expands gradually as it gains height

Fades & respawns after a fixed lifespan

You can tune the look by editing these constants in main.cpp:

const int MAX_PARTICLES = 1000;
const float LIFE_SPAN = 22.0f;
const float UPWARD_SPEED_MIN = 1.0f;
const float UPWARD_SPEED_MAX = 1.8f;
const float SIZE_GROWTH = 1.7f;
const float HORIZONTAL_SPREAD = 0.3f;

🧱 Chimney Model

The chimney is drawn from Chimney class:

Defined in chimney.h / chimney.cpp

Built from simple rectangular geometry

Uses chimney_texture.jpg as material

Rendered below smoke (translated by CHIMNEY_Y - 0.5f)

Add or modify texture loading inside Chimney::Init().

🧩 Future Improvements

Add dynamic lighting and shadows

Implement wind direction control

Add house model under the chimney

Introduce smoke color fading over lifetime