#include "Enemy.h"

Enemy::Enemy(Texture2D texture)
{
	sprite_sheet = texture;
	animations[EnemyState::ACTIVE] = { 0, 6, 1.0f / 12.0f };
	animations[EnemyState::DANCING] = { 0,1,1.0f / 12.0f };
	current_anim_state = EnemyState::ACTIVE;
	prev_anim_state = EnemyState::ACTIVE;
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
		float angle_rad = atan2f(direction.y, direction.x);

		rotation = angle_rad * RAD2DEG + 90.0f;
		if (current_anim_state != prev_anim_state)
		{
			current_frame = 0;
			frame_timer = 0.0f;
		}
		prev_anim_state = current_anim_state;
		AnimationData& current_anim = animations[current_anim_state];
		frame_timer += delta_time;
		if (frame_timer >= current_anim.frame_speed)
		{
			frame_timer = 0.0f;
			current_frame++;
			if (current_frame >= current_anim.num_frames)
			{
				current_frame = 0;
			}
		}
		if (Vector2Distance(position, target) < 10.0f)
		{
			moving_to_end = !moving_to_end;
		}
	}

	else if (state == EnemyState::DANCING)
	{
		current_anim_state = EnemyState::DANCING;
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
	color = WHITE;
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
	AnimationData& current_anim = animations[current_anim_state];
	Rectangle source_rec = { (float)current_frame * frame_width, (float)current_anim.row_index * frame_height,
							frame_width, frame_height };
	Rectangle dest_rec = { position.x, position.y, size, size };
	Vector2 origin = { size / 2, size / 2 };

	DrawTexturePro(sprite_sheet, source_rec, dest_rec, origin, rotation, color);
}

void Enemy::SetTexture(Texture2D texture)
{
	sprite_sheet = texture;
	frame_width = 64.0f;
	frame_height = 64.0f;
}
