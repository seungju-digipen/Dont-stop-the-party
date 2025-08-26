#include "Enemy.h"

Enemy::Enemy()
{
    hitbox = {position.x - size / 2, position.y - size / 2, size, size};
}

Enemy::~Enemy()
{
}

void Enemy::SetPosition(Vector2 new_position)
{
    position = new_position;
    start_pos = new_position;
    end_pos = {start_pos.x + GetRandomValue(-400, 400), start_pos.y + GetRandomValue(-200, 200)};
}

void Enemy::Update(float delta_time, Vector2 player_position)
{
    if (state == EnemyState::ACTIVE)
    {
        Vector2 target = moving_to_end ? end_pos : start_pos;
        Vector2 direction = Vector2Normalize({target.x - position.x, target.y - position.y});
        position.x += direction.x * speed * delta_time;
        position.y += direction.y * speed * delta_time;

        if (Vector2Distance(position, target) < 10.0f)
        {
            moving_to_end = !moving_to_end;
        }
    }

    else if (state == EnemyState::DANCING)
    {
        rotation += 360.0f * delta_time;
    }
    hitbox.x = position.x - (size / 2);
    hitbox.y = position.y - (size / 2);
}

void Enemy::Draw()
{
    Rectangle rec = {position.x, position.y, size, size};
    Vector2 origin = {size / 2, size / 2};

    DrawRectanglePro(rec, origin, rotation, color);
}
