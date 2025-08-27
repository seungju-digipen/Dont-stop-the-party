#include "Character.h"

Character::Character()
{
	popper = true;
	alive = true;
	hitbox = { (position.x - (size / 2)), (position.y - (size / 2)), size, size };
}
Character::~Character()
{
}

void Character::LoadResources()
{
	sprite_sheet = LoadTexture("resources/PlayerSheet.png");
	animations[CharacterAnimState::IDLE] = { 0, 1, 1.0f / 12.0f };
	animations[CharacterAnimState::WALK] = { 0, 6, 1.0f / 12.0f };
	animations[CharacterAnimState::RUN] = { 2, 6, 1.0f / 16.0f };
	animations[CharacterAnimState::WALK_POPPER] = { 1, 1, 1.0f / 1.0f };
	current_anim_state = CharacterAnimState::IDLE;
	prev_anim_state = CharacterAnimState::IDLE;
}
void Character::UnloadResources()
{
	UnloadTexture(sprite_sheet);
}
void Character::SetPosition(Vector2 new_position)
{
	position.x = new_position.x;
	position.y = new_position.y;
}

void Character::Update(float delta_time, int map_width, int map_height)
{
	if (!alive)
		return;
	if (IsKeyDown(KEY_A))
	{

		rotation -= rotation_speed * delta_time;
	}
	if (IsKeyDown(KEY_D))
	{
		rotation += rotation_speed * delta_time;
	}
	//if (IsKeyDown(KEY_W))
	//{
	//}
	//if (IsKeyDown(KEY_LEFT_SHIFT))
	//{
	//	current_anim_state = CharacterAnimState::WALK_POPPER;
	//}
	//else
	//{
	//	current_anim_state = CharacterAnimState::RUN;
	//	//current_anim_state = HasPopper() ? CharacterAnimState::WALK_POPPER : CharacterAnimState::WALK;
	//}

	float radians = (rotation - 90.0f) * DEG2RAD;

	Vector2 forward_vector = { (float)cos(radians), (float)sin(radians) };
	position.x += forward_vector.x * speed * delta_time;
	position.y += forward_vector.y * speed * delta_time;

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

	if ((IsKeyDown(KEY_SPACE)))
	{
		current_anim_state = CharacterAnimState::WALK_POPPER;
		if (speed > min_speed)
		{
			speed -= acceleration * delta_time;
		}
	}
	if (!(IsKeyDown(KEY_SPACE)))
	{
		current_anim_state = CharacterAnimState::RUN;
		if (speed <= max_speed)
		{
			speed += acceleration * delta_time;
		}
	}
	hitbox.x = position.x;
	hitbox.y = position.y;

	if (position.y > map_height)
	{
		position.y = map_height;
	}
	else if (position.y < 0)
	{
		position.y = 0;
	}

	if (position.x > map_width)
	{
		position.x = map_width;
	}
	else if (position.x < 0)
	{
		position.x = 0;
	}

	// std::cout << position.x << ", " << position.y << std::endl;
}

void Character::Draw()
{
	if (!alive)
		return;

	AnimationData& current_anim = animations[current_anim_state];

	Rectangle source_rec = { (float)current_frame * frame_width, (float)current_anim.row_index * frame_height,
							frame_width, frame_height };

	Rectangle dest_rec = { position.x, position.y, size, size };
	Vector2 origin = { size / 2, size / 2 };

	DrawTexturePro(sprite_sheet, source_rec, dest_rec, origin, rotation, WHITE);
}

void Character::Knockback(Vector2 direction, float force)
{
	position.x += direction.x * force;
	position.y += direction.y * force;
}
