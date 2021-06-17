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
#include <iostream>
using namespace std;

SDL_Window* displayWindow;
bool gameIsRunning = true;

//Variable initialization
ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;
glm::mat4 modelMatrix, ballMatrix, leftPaddleMatrix, rightPaddleMatrix;

//Initialize left and right player's speed and size
glm::vec3 leftPosition, rightPosition = glm::vec3(0, 0, 0);
glm::vec3 leftMoving, rightMoving = glm::vec3(0, 0, 0);
glm::vec3 pSize = glm::vec3(0.30f, 1.2f, 1.0f);
float playerSpeed = 4.0f;
float pHeight = pSize.y;
float pWidth = pSize.x;

//Initialize ball position, speed, and size
glm::vec3 ballPosition = glm::vec3(0, 0, 0);
glm::vec3 ballMoving = glm::vec3(0, 0, 0);
glm::vec3 ballSize = glm::vec3(0.30f, 0.30f, 1.0f);
float ballSpeed = 4.0f;
float bHeight = ballSize.y;
float bWidth = ballSize.x;

GLuint ballTextureID, paddleTextureID, keyInstructionsTextureID;

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
    
    stbi_image_free(image);
    
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //texture paths
    ballTextureID = LoadTexture("./textures/ball.png");
    paddleTextureID = LoadTexture("./textures/paddle.jpg");
}

void ProcessInput() {

    leftMoving = glm::vec3(0);
    rightMoving = glm::vec3(0);

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
   
    //right player
    if (keys[SDL_SCANCODE_UP] && (rightPosition.y + (pHeight / 2.0f) <= 3.75f)) {
        rightMoving.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && (rightPosition.y - (pHeight / 2.0f)) >= -3.75f) {
        rightMoving.y = -1.0f;
    }

    //left player 
    if (keys[SDL_SCANCODE_W] && (leftPosition.y + (pHeight / 2.0f) <= 3.75f)) {
        leftMoving.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S] && (leftPosition.y - (pHeight / 2.0f)) >= -3.75f) {
        leftMoving.y = -1.0f;
    }

    if (keys[SDL_SCANCODE_SPACE]) {
        ballMoving = glm::vec3(1, 0.5, 0);

    }

    if (glm::length(ballMoving) > 1.0f) {
        ballMoving = glm::normalize(ballMoving);
    }
}

void updateLeft(float deltaTime) {
    leftPosition += leftMoving * playerSpeed * deltaTime;
    leftPaddleMatrix = glm::translate(modelMatrix, glm::vec3(-4.8f, 0.0f, 0.0f));
    leftPaddleMatrix = glm::translate(leftPaddleMatrix, leftPosition);
    leftPaddleMatrix = glm::scale(leftPaddleMatrix, pSize);
}

void updateRight(float deltaTime) {
    rightPosition += rightMoving * playerSpeed * deltaTime;
    rightPaddleMatrix = glm::translate(modelMatrix, glm::vec3(4.8f, 0.0f, 0.0f));
    rightPaddleMatrix = glm::translate(rightPaddleMatrix, rightPosition);
    rightPaddleMatrix = glm::scale(rightPaddleMatrix, pSize);
}


void collisions() {
    float x1, y1, x2, y2, ball_width, ball_height;

    ball_width = (bWidth + pWidth + 0.2f) / 2.0f;
    ball_height = (bHeight + 0.2f + pHeight) / 2.0f;

    x1 = fabs(ballPosition.x - rightPosition.x - 4.8f) - ball_width;
    x2 = fabs(ballPosition.x - leftPosition.x + 4.8f) - ball_width;
    y1 = fabs(ballPosition.y - rightPosition.y) - ball_height;
    y2 = fabs(ballPosition.y - leftPosition.y) - ball_height;

    if (x1 < 0 && y1 < 0) {
        if (ballPosition.x > 4.8f) {
            ballMoving.x = fabs(ballMoving.x);
        }
        else {
            ballMoving.x = -1.0f * fabs(ballMoving.x);
        }
    }
    if (x2 < 0 && y2 < 0) {
        if (ballPosition.x < -4.8f) {
            ballMoving.x = -1.0f * fabs(ballMoving.x);
        }
        else {
            ballMoving.x = -1.0f  * ballMoving.x;
        }
    }
 
}


float lastTicks = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    updateLeft(deltaTime);
    updateRight(deltaTime);

    //ball movement
    if (glm::length(ballMoving) > 1.0f) {
        ballMoving = glm::normalize(ballMoving);
    }
    ballPosition += ballMoving * ballSpeed * deltaTime;
    ballMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    ballMatrix = glm::translate(ballMatrix, ballPosition);
    ballMatrix = glm::scale(ballMatrix, ballSize);

    //if ball is passed paddles 
    if (fabs(ballPosition.x) > 4.8f) {
        leftMoving = glm::vec3(0.0f, 0.0f, 0.0f);
        rightMoving = glm::vec3(0.0f, 0.0f, 0.0f);
        ballMoving = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    //if ball hits ceiling reflect
    if (ballPosition.y + bHeight / 2.0f > 3.75f) {
        ballMoving.y = ballMoving.y * -1.0f;
    }
    
    //if ball hits floor absolute
    if (ballPosition.y - bHeight / 2.0f < -3.75f) {
        ballMoving.y = fabs(ballMoving.y);
    }

    //if ball collisions with paddle
    collisions();
}

void Render() {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(ballMatrix);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(rightPaddleMatrix);
    glBindTexture(GL_TEXTURE_2D, paddleTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(leftPaddleMatrix);
    glBindTexture(GL_TEXTURE_2D, paddleTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    cout << "GAME OVER";
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