#include "Spotlight.h"

void Spotlight::Update(float delta_time)
{
	life_span -= delta_time;

	if (life_span > 2.0f)
	{
		if (alpha < 1.0f)
			alpha += delta_time * 2.0f;
	}
	else
	{
		if (alpha > 0.0f)
			alpha -= delta_time * 0.5f;
	}
}

void Spotlight::Draw(int map_width, int map_height)
{
	DrawCircleV(position, radius, Fade(color, alpha));

	DrawCircleV({ position.x - map_width, position.y }, radius, Fade(color, alpha));
	DrawCircleV({ position.x + map_width, position.y }, radius, Fade(color, alpha));

	DrawCircleV({ position.x, position.y - map_height }, radius, Fade(color, alpha));
	DrawCircleV({ position.x, position.y + map_height }, radius, Fade(color, alpha));

	DrawCircleV({ position.x - map_width, position.y - map_height }, radius, Fade(color, alpha));
	DrawCircleV({ position.x + map_width, position.y - map_height }, radius, Fade(color, alpha));
	DrawCircleV({ position.x - map_width, position.y + map_height }, radius, Fade(color, alpha));
	DrawCircleV({ position.x + map_width, position.y + map_height }, radius, Fade(color, alpha));
}
