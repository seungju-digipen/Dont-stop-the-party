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

	bool IsActive() const { return active; }
	Rectangle GetHitbox() const { return rect; }
	void SetHit() { hit_target = true; }
	bool DidHit() const { return hit_target; }
	Vector2 GetPosition() const { return position; }

private:
	Vector2 position;
	Vector2 velocity;
	Texture2D sprite_sheet;
	float rotation;
	float speed{ 800 };
	float size{ 60 };
	Rectangle rect;
	bool active{ true };
	bool hit_target{ false };

	// --- Animation ---
	float frame_timer{ 0.0f };
	int current_frame{ 0 };
	int max_frames{ 6 };
	float frame_speed{ 0.05f }; // 속도를 조금 조절했습니다.
	float frame_width{ 32.0f };
	float frame_height{ 32.0f };
	// float life_span{ 1.0f }; // <<-- 삭제: 애니메이션이 수명을 대체합니다.
};