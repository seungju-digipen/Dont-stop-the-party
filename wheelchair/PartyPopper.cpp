#include "PartyPopper.h"

PartyPopper::PartyPopper(Vector2 start_position, float start_rotation, float initial_speed)
{
    position = start_position;
    rotation = start_rotation;
    active = true;

    float radians = rotation * DEG2RAD;
    velocity.x = cos(radians) * speed;
    velocity.y = sin(radians) * speed;
}

PartyPopper::~PartyPopper()
{
}

void PartyPopper::Update(float delta_time, int map_width, int map_height)
{
    if (active)
    {

        rect.x = position.x - (size / 2);
        rect.y = position.y - (size / 2);
        rect.width = size;
        rect.height = size;

        life_span -= delta_time;
        if (life_span <= 0.0f)
        {
            active = false;
            return;
        }
        position.x += velocity.x * delta_time;
        position.y += velocity.y * delta_time;

        if (position.x < 0 || position.x > map_width || position.y < 0 || position.y > map_height)
        {
            active = false;
        }
    }
}

void PartyPopper::Draw()
{
    if (active)
    {
        rect = {position.x, position.y, size, size};
        Vector2 origin = {size / 2, size / 2};
        DrawRectanglePro(rect, origin, rotation, PINK);
    }
}
