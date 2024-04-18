#include <SDL.h>
#include <iostream>
#include <SDL_opengl.h>

struct Vertex {
    float x, y, z; // Position
    float r, g, b, a; // Color
};

struct Mesh {
    Vertex* vertices;
    int vertexCount;
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Optimized Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Define the cube vertices
    Vertex cubeVertices[] = {
        // Front face
        {-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        // Back face
        {-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        // Top face
        {-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        // Bottom face
        {-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        // Right face
        { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        // Left face
        {-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}
    };

    // Define the cube mesh
    Mesh cubeMesh = {cubeVertices, sizeof(cubeVertices) / sizeof(Vertex)};

    // OpenGL setup
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Render loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the cube
        glBegin(GL_QUADS);
        for (int i = 0; i < cubeMesh.vertexCount; i++) {
            glColor4f(cubeMesh.vertices[i].r, cubeMesh.vertices[i].g, cubeMesh.vertices[i].b, cubeMesh.vertices[i].a);
            glVertex3f(cubeMesh.vertices[i].x, cubeMesh.vertices[i].y, cubeMesh.vertices[i].z);
        }
        glEnd();

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
