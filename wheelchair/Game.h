#pragma once
#include <algorithm>
#include <iostream>
#include <vector>

#include "Character.h"
#include "Enemy.h"
#include "PartyPopper.h"
#include "Spotlight.h"
#include "raylib.h"
#include "raymath.h"

class Game
{
  public:
    Game();
    ~Game();
    void Run();

  private:
    void Update();
    void Draw();
    void InitGame();

    int screen_width, screen_height;
    int map_width, map_height;

    Character hero;
    std::vector<Enemy> enemies;
    int number_of_enemies;
    std::vector<PartyPopper> partyPoppers;
    std::vector<Spotlight> spotlights;

    Camera2D camera;
    float shake_timer = 0.0f;
    float shake_intensity = 15.0f;

    // --- ¿¡¼Â ---
    Texture2D explosion_texture;
    Music bgm;
    Sound shoot_sfx;
    RenderTexture2D light_texture;
};
