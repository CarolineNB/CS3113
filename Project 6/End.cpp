#include "End.h"

#define END_WIDTH 14
#define END_HEIGHT 8

#define END_ENEMY_COUNT 0

unsigned int end_data[] =
{
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void End::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("seaset.png");
    state.map = new Map(END_WIDTH, END_HEIGHT, end_data, mapTextureID, 1.0f, 5, 1);

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(7, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 4.0f;
    state.player->textureID = Util::LoadTexture("plant saver.png");
    state.player->height = 0.8f;
    state.player->width = 0.8f;

    state.player->jumpPower = 7.0f;

    state.enemies = new Entity[END_ENEMY_COUNT];
}

void End::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, END_ENEMY_COUNT, state.map);
}

void End::Render(ShaderProgram* program) {

    GLuint fontTextureID = Util::LoadTexture("font.png");

    if (state.lives >= 1) {
        Util::DrawText(program, fontTextureID, "You Win!", 0.34f, 0.05f, glm::vec3(3.2, -2.55, 0));
        Util::DrawText(program, fontTextureID, "Press Enter to play again", 0.2f, 0.05f, glm::vec3(3.2, -3.0, 0));
    }
    else {
        Util::DrawText(program, fontTextureID, "You Lose", 0.34f, 0.05f, glm::vec3(3.2, -2.55, 0));
        Util::DrawText(program, fontTextureID, "Press Enter to play again", 0.2f, 0.05f, glm::vec3(3.2, -3.0, 0));
    }

    state.map->Render(program);
    state.player->Render(program);
}