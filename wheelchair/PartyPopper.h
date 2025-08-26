#pragma once
#include "raylib.h"
#include "raymath.h"

class PartyPopper
{
public:
	PartyPopper(Vector2 start_pos, float start_rot, float initial_speed, Texture2D texture);
	~PartyPopper();

	void Update(float delta_time, int map_width, int map_height);
	void Draw();
	bool IsActive() const
	{
		return active;
	}
	Rectangle GetHitbox() const
	{
		return rect;
	}
	void Deactivate()
	{
		active = false;
	}
	void SetHit()
	{
		hit_target = true;
	}
	bool DidHit() const
	{
		return hit_target;
	}

private:
	Vector2 position;
	Vector2 velocity;
	Texture2D sprite_sheet;
	float rotation;
	float speed{ 800 };
	float size{ 60 };
	Rectangle rect;
	bool active;
	float frame_timer{ 0.0f };
	int current_frame{ 0 };
	int max_frames{ 6 };
	float frame_speed{ 0.02f };
	float frame_width{ 32.0f };
	float frame_height{ 32.0f };
	float life_span{ 1.0f };
	bool hit_target{ false };
};
