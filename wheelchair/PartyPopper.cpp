#include "PartyPopper.h"

PartyPopper::PartyPopper(Vector2 start_pos, float start_rot, float initial_speed, Texture2D texture)
{
	position = start_pos;
	rotation = start_rot;
	sprite_sheet = texture;
	

	float radians = (rotation - 90.0f) * DEG2RAD;

}

PartyPopper::~PartyPopper()
{
	
}

void PartyPopper::Update(float delta_time, int map_width, int map_height)
{
	if (!active) return;

	frame_timer += delta_time;
	if (frame_timer >= frame_speed)
	{
		frame_timer = 0.0f;
		current_frame++;


		if (current_frame >= max_frames)
		{
			active = false;
		}
	}


	rect = { position.x - (size / 2), position.y - (size / 2), size, size };


	if (position.x < -size || position.x > map_width + size || position.y < -size || position.y > map_height + size)
	{
		active = false;
	}
}

void PartyPopper::Draw()
{
	if (!active) return;

	Rectangle source_rec = {
		(float)current_frame * frame_width,
		0,
		frame_width,
		frame_height
	};
	Rectangle dest_rec = { position.x, position.y, size, size };
	Vector2 origin = { size / 2, size / 2 };
	DrawTexturePro(sprite_sheet, source_rec, dest_rec, origin, 0.0f, WHITE);
	DrawRectangleLinesEx(rect, 10, {255,255,255,50});
}