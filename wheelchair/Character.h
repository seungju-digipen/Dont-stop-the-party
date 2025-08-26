#pragma once
#include "raylib.h"
#include "raymath.h"
#include "AnimationData.h"

enum class CharacterAnimState {
	IDLE,
	WALK,
	RUN,
	WALK_POPPER
};

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
	Vector2 velocity{ 0, 0 };
	Rectangle hitbox;
	Texture2D sprite_sheet;
	std::map<CharacterAnimState, AnimationData> animations;
	CharacterAnimState current_anim_state;
	CharacterAnimState prev_anim_state;
	float min_speed{ 200 };
	float max_speed{ 500 };
	float acceleration{ 100 };
	float rotation{ 0.0 };
	float rotation_speed{ 200 };
	float size{ 40 };
	float speed{ 200 };
	float frame_timer{ 0.0f };
	int current_frame{ 0 };
	float frame_width{ 64.0f };
	float frame_height{ 64.0f };
	bool popper;
	bool alive;
};
