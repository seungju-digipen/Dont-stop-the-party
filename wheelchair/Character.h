#pragma once
#include "raylib.h"
#include "raymath.h"

class Character
{
  public:
    Character();
    ~Character();

    Vector2 GetPosition() const
    {
        return position;
    }
    Vector2 GetVelocity() const
    {
        return velocity;
    }
    float GetRotation() const
    {
        return rotation;
    }
    void SetPosition(Vector2 position);
    float GetSpeed() const
    {
        return speed;
    }
    void Draw();
    void Update(float delta_time, int map_width, int map_height);

    bool HasPopper() const
    {
        return popper;
    }
    void UsePopper()
    {
        popper = false;
    }
    void GetPopper()
    {
        popper = true;
    }
    void Knockback(Vector2 direction, float force);
    bool IsAlive() const
    {
        return alive;
    }

  private:
    Vector2 position;
    Vector2 velocity{0, 0};
    Rectangle hitbox;
    float speed{200};
    float min_speed{200};
    float max_speed{500};
    float acceleration{100};
    float rotation{0.0};
    float rotation_speed{200};
    float size{40};
    bool popper;
    bool alive;
};
