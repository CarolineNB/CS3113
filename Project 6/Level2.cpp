#include "Level2.h"

#define LEVEL2_WIDTH 22
#define LEVEL2_HEIGHT 15

#define LEVEL2_ENEMY_COUNT 5

unsigned int level2_data[] =
{
    //22x14
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0,
    2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
    2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 0, 0, 2,
    2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 2, 2, 2, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2,
    2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 2, 2, 2, 2, 0, 0, 0, 2,
    3, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 2, 0, 0, 0, 2, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
};

void Level2::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("seaset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 2.5f, 5, 1);

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(2, -3, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 3.5f;
    state.player->textureID = Util::LoadTexture("plant saver.png");
    state.player->height = 0.8f;
    state.player->width = 0.8f;
    state.player->jumpPower = 6.0f;

    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];

    state.player->startPos = glm::vec3(1.5, -3.5, 0);
    state.player->livesLeft = state.lives;


    GLuint crabfTextureID = Util::LoadTexture("trash.png");
    state.enemies[0].textureID = crabfTextureID;
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].speed = .75;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].position = glm::vec3(22.3, -3.0f, 0);
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

    state.enemies[2].textureID = crabfTextureID;
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].speed = 2;
    state.enemies[2].position = glm::vec3(36.0f, -13.0f, 0);
    state.enemies[2].aiType = WAITANDGO;
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].aiState = IDLE;

    state.enemies[3].textureID = crabfTextureID;
    state.enemies[3].entityType = ENEMY;
    state.enemies[3].speed = 2;
    state.enemies[3].position = glm::vec3(38.0f, -13.0f, 0);
    state.enemies[3].aiType = WAITANDGO;
    state.enemies[3].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[3].aiState = IDLE;

    state.enemies[4].textureID = crabfTextureID;
    state.enemies[4].entityType = ENEMY;
    state.enemies[4].speed = 2;
    state.enemies[4].position = glm::vec3(43.0f, -13.0f, 0);
    state.enemies[4].aiType = WAITANDGO;
    state.enemies[4].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[4].aiState = IDLE;
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }
    state.lives = state.player->livesLeft;
    if (state.lives <= 0) {
        state.nextScene = 4;
    }
    if (state.player->position.x >= 57.0) {
        state.nextScene = 3;
    }
}

void Level2::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("font.png");
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) state.enemies[i].Render(program);
    if (state.lives == 5) {
        Util::DrawText(program, fontTextureID, "Lives: 5", .2f, .1f, glm::vec3(2, -3, 0));
    }
    else if (state.lives == 4) {
        Util::DrawText(program, fontTextureID, "Lives: 4", 0.2f, 0.1f, glm::vec3(2, -3, 0));
    }
    else if (state.lives == 3) {
        Util::DrawText(program, fontTextureID, "Lives: 3", 0.2f, 0.1f, glm::vec3(2, -3, 0));
    }
    else if (state.lives == 2) {
        Util::DrawText(program, fontTextureID, "Lives: 2", 0.2f, 0.1f, glm::vec3(2, -3, 0));
    }
    else {
        Util::DrawText(program, fontTextureID, "Lives: 1", 0.2f, 0.1f, glm::vec3(2, -3, 0));
    }
    state.map->Render(program);
    state.player->Render(program);
}