#pragma once
#include "raylib.h"
struct ConfettiParticle {
	Vector2 position;
	Vector2 velocity;
	float rotation;
	float angular_velocity; 
	float size;
	float life_span;       
	Color color;
};