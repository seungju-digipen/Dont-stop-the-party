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
	Enemy();
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
	void Draw();
	void Update(float delta_time, int map_width, int map_height);
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
	Rectangle screen;
	float speed{ 100 };
	float size{ 40 };
	int gen_range{ 400 };
	Color color{ GREEN };
	EnemyState state{ EnemyState::ACTIVE };
	float rotation{ 0.0f };
	float respawn_timer{ 0.0f };
	Vector2 start_pos;
    Vector2 end_pos;
    bool moving_to_end{true};
};
