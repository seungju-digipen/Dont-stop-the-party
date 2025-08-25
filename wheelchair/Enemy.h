#pragma once
#include "raylib.h"
#include "raymath.h"

enum class EnemyState
{
    CHASING,
    DANCING
};

class Enemy
{
  public:
    Enemy();
    ~Enemy();

    Vector2 GetPosition() const
    {
        return position;
    };
    Rectangle GetHitbox() const
    {
        return hitbox;
    }
    void SetPosition(Vector2 new_position);
    void Draw();
    void Update(float delta_time, Vector2 player_position);

    bool IsChasing() const
    {
        return state == EnemyState::CHASING;
    }
    void GotHit()
    {
        state = EnemyState::DANCING;
        color = RED;
    }

  private:
    Vector2 position;
    Vector2 velocity{0, 0};
    Rectangle hitbox;
    float speed{100};
    float size{40};
    Color color{GREEN};
    EnemyState state{EnemyState::CHASING};
    float rotation{0.0f};
};
