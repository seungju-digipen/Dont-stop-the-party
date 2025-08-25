#pragma once
#include "raylib.h"
#include "raymath.h"

class PartyPopper
{
  public:
    PartyPopper(Vector2 start_position, float start_rotation, float initial_speed);
    ~PartyPopper();

    void Update(float delta_time, int map_width, int map_height);
    void Draw();
    bool IsActive() const
    {
        return active;
    }
    Rectangle GetHitbox() const
    {
        return rect;
    }
    void Deactivate()
    {
        active = false;
    }

  private:
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float speed{800};
    float size{60};
    Rectangle rect;
    bool active;
    float life_span{0.1f};
};
