#include "GameStart.h"

#define MENU_WIDTH 14
#define MENU_HEIGHT 8

#define MENU_ENEMY_COUNT 0

unsigned int menu_data[] =
{
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

void GameStart::Initialize() {

    state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("seaset.png");
	state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 5, 1);
	
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(7, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 4.0f;
    state.player->textureID = Util::LoadTexture("plant saver.png");
    state.player->height = 0.8f;
    state.player->width = 0.8f;
    state.player->jumpPower = 7.0f;

    state.enemies = new Entity[MENU_ENEMY_COUNT];
}

void GameStart::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, MENU_ENEMY_COUNT, state.map);
}

void GameStart::Render(ShaderProgram* program) {
	
    GLuint fontTextureID = Util::LoadTexture("font.png");

    Util::DrawText(program, fontTextureID, "This is: Oceans Rising", 0.34f, 0.05f, glm::vec3(2.7, -2, 0));
    Util::DrawText(program, fontTextureID, "There are 3 levels, you have 5 lives.", 0.2f, 0.05f, glm::vec3(2.0, -2.45, 0));
    Util::DrawText(program, fontTextureID, "Save the world by making it to end of maze", 0.2f, 0.05f, glm::vec3(1.0, -2.85, 0));
    Util::DrawText(program, fontTextureID, "Kill enemies by hitting their head. Press Enter to Play", 0.15f, 0.05f, glm::vec3(1.0, -3.2, 0));

    state.map->Render(program);
	state.player->Render(program);
}