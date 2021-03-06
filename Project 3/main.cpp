#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <iostream>

#include <vector>

#define PLATFORM_COUNT 27

struct GameState {
    Entity* player;
    Entity* platforms;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Cows Landing on Lollis!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0.0f, 4.0f, 0.0f);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0f, -0.7f, 0.0f);
    state.player->speed = 2.0f;
    state.player->textureID = LoadTexture("textures/cow.png");

    state.player->height = 0.4f;
    state.player->width = 0.4f;

    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("textures/cloud.png");
    GLuint portalTextureID = LoadTexture("textures/lolli.png");

    float wall = 8; 
    float ground= 6;
    
    int start = 0;

    //draw left walls
    float plane = 0;
    for (int i = 0; i < wall; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-4.5, -3.5 - plane, 0);
        plane -= 1;
    }
  
    //draw right walls
    plane = 0;
    start += wall;
    for (int i = start; i < wall + wall; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(4.5, -3.5 - plane, 0);
        plane -= 1;
    }

    //draw ground 
    plane = 0;
    start += wall;
    for (int i = start; i < wall + wall + ground; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-3.5 + plane, -3.5, 0);
        plane += 1;
    }

    //add floating blocks
    plane = 0;
    start += ground;
    for (int i = start; i < wall + wall + ground + 3; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-2.5 + plane, -0.25, 0);
        plane += 2.5;
    }
    
    //add portal
    state.platforms[25].textureID = portalTextureID;
    state.platforms[26].textureID = portalTextureID;

    state.platforms[25].position = glm::vec3(2.5, -3.5, 0);
    state.platforms[26].position = glm::vec3(3.5 , -3.5, 0);
    

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0);
    }

}

void ProcessInput() {

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    //player hits lollipop platform
    if (state.player->enterdPortal(&state.platforms[25])
        || state.player->enterdPortal(&state.platforms[26]))
            state.player->isWinner = true;
    else if (!state.player->collidedLeft && !state.player->collidedRight && !state.player->collidedBottom) {
        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->acceleration.x = -50.0f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->acceleration.x = 50.0f;
        }
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size) {

    std::vector<float> vertices;
    std::vector<float> texCoords; 
    
    int columns = 16;
    int rows = 16;
    float width = 1.0f / columns;
    float height = 1.0f / rows;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = i * (size + -0.2f);

        float u = (float)(index % columns) / columns;
        float v = (float)(index / columns) / rows;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            });

        texCoords.insert(texCoords.end(), {
            u, v, u, v + height,
            u + width, v, u + width, v + height,
            u + width, v, u, v + height,
            });
    }

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.0f, 1.5f, 0));

    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 10));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    state.player->Render(&program);

    if (state.player->isWinner) {
        DrawText(&program, LoadTexture("textures/font1.png"), "MISSION SUCCESSFUL!!! ", 0.5f);
    }
    else if (state.player->collidedLeft || state.player->collidedRight || state.player->collidedBottom) {
        DrawText(&program, LoadTexture("textures/font1.png"), "MISSION FAILED :(   ", 0.5f);
    }


    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}