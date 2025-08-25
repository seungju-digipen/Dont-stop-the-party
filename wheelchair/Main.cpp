#include <algorithm>
#include <iostream>
#include <vector>

#include "Character.h"
#include "Enemy.h"
#include "PartyPopper.h"
#include "Spotlight.h"
#include "raylib.h"
#include "raymath.h"

int main()
{
    constexpr int screen_width = 800;
    constexpr int screen_height = 600;

    constexpr int map_width = 2400;
    constexpr int map_height = 1800;

    InitWindow(screen_width, screen_height, "Don't Stop the party");
    InitAudioDevice();
    SetTargetFPS(60);

    std::vector<Spotlight> spotlights;
    RenderTexture2D light_texture = LoadRenderTexture(map_width, map_height);

    Character hero;
    hero.SetPosition({map_width / 2.0f, map_height / 2.0f});

    std::vector<Enemy> enemies;
    int number_of_enemies = 8;
    for (int i = 0; i < number_of_enemies; i++)
    {
        Enemy new_enemy;
        Vector2 random_position = {(float)GetRandomValue(0, map_width), (float)GetRandomValue(0, map_height)};
        new_enemy.SetPosition(random_position);
        enemies.push_back(new_enemy);
    }

    std::vector<PartyPopper> partyPoppers;

    Camera2D camera = {0};
    camera.target = hero.GetPosition();
    camera.offset = {screen_width / 2.0f, screen_height / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    float shake_timer = 0.0f;
    float shake_intensity = 5.0f;

    Music bgm = LoadMusicStream("resources/party2.mp3");
    Sound shoot_sfx = LoadSound("resources/Boom1.wav");
    PlayMusicStream(bgm);

    while (!WindowShouldClose())
    {
        float delta_time = GetFrameTime();

        if (GetRandomValue(0, 30) == 0)
        {
            spotlights.emplace_back(map_width, map_height);
        }

        for (auto &light : spotlights)
        {
            light.Update(delta_time);
        }

        spotlights.erase(
            std::remove_if(spotlights.begin(), spotlights.end(), [](const Spotlight &s) { return !s.IsActive(); }),
            spotlights.end());

        UpdateMusicStream(bgm);
        hero.Update(delta_time, map_width, map_height);
        for (auto &enemy : enemies)
        {
            enemy.Update(delta_time, hero.GetPosition());
        }

        if (IsKeyPressed(KEY_ENTER) && hero.HasPopper())
        {
            hero.UsePopper();
            PartyPopper newPopper(hero.GetPosition(), hero.GetRotation(), hero.GetSpeed());
            partyPoppers.push_back(newPopper);
            PlaySound(shoot_sfx);

            float radians = hero.GetRotation() * DEG2RAD;
            Vector2 backward_vector = {-cosf(radians), -sinf(radians)};
            hero.Knockback(backward_vector, 60.0f);

            shake_timer = 0.2f;
        }
        if (shake_timer > 0.0f)
        {
            camera.offset.x = screen_width / 2.0f + GetRandomValue(-shake_intensity, shake_intensity);
            camera.offset.y = screen_height / 2.0f + GetRandomValue(-shake_intensity, shake_intensity);

            shake_timer -= delta_time;
        }
        else
        {
            camera.offset.x = screen_width / 2.0f;
            camera.offset.y = screen_height / 2.0f;
        }

        camera.target = hero.GetPosition();
        for (auto &popper : partyPoppers)
        {
            popper.Update(delta_time, map_width, map_height);
        }

        for (auto &popper : partyPoppers)
        {
            if (!popper.IsActive())
                continue;

            for (auto &enemy : enemies)
            {
                if (!enemy.IsChasing())
                    continue;
                if (CheckCollisionRecs(popper.GetHitbox(), enemy.GetHitbox()))
                {
                    enemy.GotHit();
                    popper.Deactivate();
                    hero.GetPopper();
                    break;
                }
            }
        }

        BeginTextureMode(light_texture);
        ClearBackground(BLANK);
        for (auto &light : spotlights)
        {
            light.Draw(map_width, map_height);
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
        DrawRectangle(0, 0, map_width, map_height, Fade(BLACK, 0.5f));
        DrawCircle(map_width / 2, map_height / 2, 20, BLUE);

        hero.Draw();
        for (auto &enemy : enemies)
        {
            enemy.Draw();
        }

        for (auto &popper : partyPoppers)
        {
            popper.Draw();
        }
        partyPoppers.erase(std::remove_if(partyPoppers.begin(), partyPoppers.end(),
                                          [](const PartyPopper &p) { return !p.IsActive(); }),
                           partyPoppers.end());
        BeginBlendMode(BLEND_ADDITIVE);

        Rectangle sourceRec = {0, 0, (float)light_texture.texture.width, (float)-light_texture.texture.height};

        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                Vector2 position = {(float)(i * map_width), (float)(j * map_height)};
                DrawTextureRec(light_texture.texture, sourceRec, position, WHITE);
            }
        }

        EndBlendMode();
        EndMode2D();

        DrawText("Welcome to Party", 10, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    UnloadMusicStream(bgm);
    UnloadSound(shoot_sfx);
    CloseAudioDevice();

    return 0;
}
