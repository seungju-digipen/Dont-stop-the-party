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
#include "Particle.h" 

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
    void ResetGame();
    void SpawnConfetti(Vector2 position);
    const int grid_spacing = 100;
    const Color grid_color = {50, 50, 50, 255};

    int screen_width, screen_height;
    int map_width, map_height;

    int score;
    const float gravity = 400.0f;
    Character hero;
    std::vector<Enemy> enemies;
    int number_of_enemies;
    std::vector<PartyPopper> partyPoppers;
    std::vector<Spotlight> spotlights;
    std::vector<ConfettiParticle> confettiParticles;

    Camera2D camera;
    float shake_timer = 0.0f;
    float shake_intensity = 15.0f;
    //Rectangle camera_rect;

    Texture2D explosion_texture;
    Music bgm;
    Sound shoot_sfx;
    RenderTexture2D light_texture;
};
