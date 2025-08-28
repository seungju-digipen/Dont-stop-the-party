#pragma once
#include "raylib.h"
#include "raymath.h"
#include "AnimationData.h"

enum class EnemyState
{
	ACTIVE,
	DANCING
};

class Enemy
{
public:
	Enemy(Texture2D texture);
	~Enemy();
	Vector2 GetPosition() const
	{
		return position;
	};
	Rectangle GetHitbox() const
	{
		return hitbox;
	}
	void SetPosition(Vector2 new_position);
	void SetTexture(Texture2D texture);
	void Update(float delta_time, int map_width, int map_height);
	void Draw();
	void Reset(int map_width, int map_height);
	bool IsActive() const
	{
		return state == EnemyState::ACTIVE;
	}
	void GotHit();
	int GetGenRagne() const
	{
		return gen_range;
	}
private:
	Vector2 position;
	Vector2 velocity{ 0, 0 };
	Rectangle hitbox;
	std::map<EnemyState, AnimationData> animations;
	EnemyState current_anim_state;
	EnemyState prev_anim_state;
	Rectangle screen;
	float speed{ 100 };
	float size{ 40 };
	int gen_range{ 400 };
	Color color{ WHITE };
	EnemyState state{ EnemyState::ACTIVE };
	float rotation{ 0.0f };
	Texture2D sprite_sheet;
	int current_frame{ 0 };
	int max_frames{ 6 }; 
	float frame_timer{ 0.0f };
	float frame_speed{ 1.0f / 8.0f };
	float frame_width{ 64.0f };  
	float frame_height{ 64.0f };
	float respawn_timer{ 0.0f };
	Vector2 start_pos;
    Vector2 end_pos;
    bool moving_to_end{true};
};
