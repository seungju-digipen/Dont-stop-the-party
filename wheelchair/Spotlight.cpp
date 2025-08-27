#include "Spotlight.h"

Spotlight::Spotlight(int map_width, int map_height)
{
	position = { (float)GetRandomValue(0, map_width), (float)GetRandomValue(0, map_height) };

	radius = (float)GetRandomValue(50, 100);
	life_span = (float)GetRandomValue(10, 50) / 10.0f;
	float hue = (float)GetRandomValue(0, 255);
	float saturation = (float)GetRandomValue(30, 90) / 100.0f;
	float value = 0.95f;

	color = ColorFromHSV(hue, saturation, value);

	alpha = 0.0f;
}

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
