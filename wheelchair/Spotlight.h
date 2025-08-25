#pragma once
#include "raylib.h"

class Spotlight
{
  public:
    Spotlight(int map_width, int map_height)
    {
        position = {(float)GetRandomValue(0, map_width), (float)GetRandomValue(0, map_height)};

        radius = (float)GetRandomValue(50, 250);
        life_span = (float)GetRandomValue(10, 50) / 10.0f;
        float hue = (float)GetRandomValue(50, 65);
        float saturation = (float)GetRandomValue(5, 70) / 100.0f;
        float value = 0.95f;

        color = ColorFromHSV(hue, saturation, value);

        alpha = 0.0f;
    }

    void Update(float delta_time);

    void Draw(int map_width, int map_height);

    bool IsActive() const
    {
        return life_span > 0.0f;
    }

  private:
    Vector2 position;
    float radius;
    float life_span;
    Color color;
    float alpha;
};
