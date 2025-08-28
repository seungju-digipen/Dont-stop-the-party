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
	start_pos = new_position;
	end_pos = { start_pos.x + GetRandomValue(-gen_range, gen_range), start_pos.y + GetRandomValue(-gen_range, gen_range) };
}

void Enemy::Update(float delta_time, int map_width, int map_height)
{
	if (state == EnemyState::ACTIVE)
	{
		Vector2 target = moving_to_end ? end_pos : start_pos;
		Vector2 direction = Vector2Normalize({ target.x - position.x, target.y - position.y });
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
		respawn_timer -= delta_time;
		if (respawn_timer <= 0.0f)
		{
			Reset(map_width, map_height);
		}
	}
	hitbox.x = position.x - (size / 2);
	hitbox.y = position.y - (size / 2);
}

void Enemy::Reset(int map_width, int map_height)
{
	state = EnemyState::ACTIVE;
	color = GREEN;
	rotation = 0.0f;
	Vector2 random_position = { (float)GetRandomValue(0, map_width), (float)GetRandomValue(0, map_height) };
	SetPosition(random_position);
}

void Enemy::GotHit()
{
	state = EnemyState::DANCING;
	color = RED;
	respawn_timer = 5.0f;
}

void Enemy::Draw()
{
	Vector2 origin = { size / 2, size / 2 };
	DrawRectanglePro(hitbox, origin, rotation, color);
}
