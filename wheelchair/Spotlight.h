#pragma once
#include "raylib.h"

class Spotlight
{
public:
	Spotlight(int map_width, int map_height);

	void Update(float delta_time);

	void Draw(int map_width, int map_height);

	bool IsActive() const
	{
		return life_span > 0.0f;
	}

private:
	Vector2 position;
	float radius;
	float life_span;
	Color color;
	float alpha;
};
