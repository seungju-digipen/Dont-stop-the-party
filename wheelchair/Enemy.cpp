#include "Enemy.h"

Enemy::Enemy()
{
	hitbox = { position.x - size / 2, position.y - size / 2, size, size };
}

Enemy::~Enemy()
{
}

void Enemy::SetPosition(Vector2 new_position)
{
	position = new_position;
}

void Enemy::Update(float delta_time, Vector2 player_position)
{
	hitbox.x = position.x - (size / 2);
	hitbox.y = position.y - (size / 2);
	if (state == EnemyState::CHASING)
	{
		/*   Vector2 direction = {player_position.x - position.x, player_position.y - position.y};
		   Vector2 normalized_direction = Vector2Normalize(direction);
		   position.x += normalized_direction.x * speed * delta_time;
		   position.y += normalized_direction.y * speed * delta_time;*/
	}

	else if (state == EnemyState::DANCING)
	{
		rotation += 360.0f * delta_time;
	}
}

void Enemy::Draw()
{
	Rectangle rec = { position.x, position.y, size, size };
	Vector2 origin = { size / 2, size / 2 };

	DrawRectanglePro(rec, origin, rotation, color);
}
