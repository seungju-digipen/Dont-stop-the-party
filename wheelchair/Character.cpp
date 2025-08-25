#include "Character.h"

Character::Character()
{
    popper = true;
    alive = true;

    hitbox = {(position.x - (size / 2)), (position.y - (size / 2)), size, size};
}

Character::~Character()
{
}

void Character::SetPosition(Vector2 new_position)
{
    position.x = new_position.x;
    position.y = new_position.y;
}

void Character::Draw()
{
    Rectangle rec = {position.x, position.y, size, size};
    Vector2 origin = {size / 2, size / 2};
    DrawRectanglePro(rec, origin, rotation, ORANGE);
}

void Character::Update(float delta_time, int map_width, int map_height)
{
    if (alive)
    {
        if (IsKeyDown(KEY_A))
        {
            rotation -= rotation_speed * delta_time;
        }
        if (IsKeyDown(KEY_D))
        {
            rotation += rotation_speed * delta_time;
        }
        if (IsKeyDown(KEY_W))
        {
            float radians = rotation * DEG2RAD;

            Vector2 forward_vector = {(float)cos(radians), (float)sin(radians)};
            position.x += forward_vector.x * speed * delta_time;
            position.y += forward_vector.y * speed * delta_time;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            if (speed <= max_speed)
            {
                speed += acceleration * delta_time;
            }
        }
        if (!IsKeyDown(KEY_LEFT_SHIFT))
        {
            if (speed > min_speed)
            {
                speed -= acceleration * delta_time;
            }
        }
        hitbox.x = position.x;
        hitbox.y = position.y;

        if (position.y > map_height)
        {
            position.y = 0;
        }
        else if (position.y < 0)
        {
            position.y = map_height;
        }

        if (position.x > map_width)
        {
            position.x = 0;
        }
        else if (position.x < 0)
        {
            position.x = map_width;
        }

        // std::cout << position.x << ", " << position.y << std::endl;
    }
}

void Character::Knockback(Vector2 direction, float force)
{
    position.x += direction.x * force;
    position.y += direction.y * force;
}
