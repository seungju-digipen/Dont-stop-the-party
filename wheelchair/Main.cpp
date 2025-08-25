#include "raylib.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>

class Character
{
public:
	Character();
	~Character();

	Vector2 GetPosition() const { return position; };
	Vector2 GetVelocity() const { return velocity; };
	float GetRotation() const { return rotation; };
	void SetPosition(Vector2 position);
	float GetSpeed() const { return speed; };
	void Draw();
	void Update(float delta_time, int map_width, int map_height);
	bool HasPopper() const { return popper; };
	bool IsAlive() const { return alive; };

private:

	Vector2 position;
	Vector2 velocity{ 0,0 };
	Rectangle hitbox;
	float speed{ 200 };
	float min_speed{ 200 };
	float max_speed{ 500 };
	float acceleration{ 100 };
	float rotation{ 0.0 };
	float rotation_speed{ 200 };
	float size{ 40 };
	bool popper;
	bool alive;
};

Character::Character()
{
	popper = true;
	alive = true;

	hitbox = {
		(position.x - (size / 2)),
		(position.y - (size / 2)),
		size,
		size
	};
}

Character::~Character()
{
}

void Character::SetPosition(Vector2 new_position)
{
	position.x = new_position.x;
	position.y = new_position.y;
}

void Character::Draw()
{
	Rectangle rec = { position.x, position.y, size, size };
	Vector2 origin = { size / 2, size / 2 };
	DrawRectanglePro(rec, origin, rotation, ORANGE);
}

void Character::Update(float delta_time, int map_width, int map_height)
{
	if (alive) {
		if (IsKeyDown(KEY_A)) {
			rotation -= rotation_speed * delta_time;
		}
		if (IsKeyDown(KEY_D)) {
			rotation += rotation_speed * delta_time;
		}
		if (IsKeyDown(KEY_W)) {
			float radians = rotation * DEG2RAD;

			Vector2 forward_vector = {
				(float)cos(radians),
				(float)sin(radians)
			};
			position.x += forward_vector.x * speed * delta_time;
			position.y += forward_vector.y * speed * delta_time;
		}
		if (IsKeyDown(KEY_LEFT_SHIFT))
		{
			if (speed <= max_speed) {
				speed += acceleration * delta_time;
			}
		}
		if (!IsKeyDown(KEY_LEFT_SHIFT)) {
			if (speed > min_speed) {
				speed -= acceleration * delta_time;
			}
		}
		hitbox.x = position.x;
		hitbox.y = position.y;

		
		if (position.y > map_height)
		{
			position.y = 0;
		}
		else if (position.y < 0) {
			position.y = map_height;
		}

		if (position.x > map_width)
		{
			position.x = 0;
		}
		else if (position.x < 0) {
			position.x = map_width;
		}

		//std::cout << position.x << ", " << position.y << std::endl;
	}
}

class Enemy
{
public:
	Enemy();
	~Enemy();

	Vector2 GetPosition() const { return position; };
	void SetPosition(Vector2 new_position);
	void Draw();
	void Update(float delta_time, Vector2 player_position); 
	bool IsAlive() const { return alive; };

private:
	Vector2 position;
	Vector2 velocity{ 0,0 };
	Rectangle hitbox;
	float speed{ 150 }; 
	float size{ 40 };
	bool alive;
};

Enemy::Enemy()
{
	alive = true;
	hitbox = { position.x - size / 2, position.y - size / 2, size, size };
}

Enemy::~Enemy()
{
}

void Enemy::SetPosition(Vector2 new_position)
{
	position = new_position;
}

void Enemy::Draw()
{
	if (alive) {
		
		Rectangle rec = { position.x, position.y, size, size };
		Vector2 origin = { size / 2, size / 2 };
		
		DrawRectanglePro(rec, origin, 0.0f, GREEN);
	}
}

void Enemy::Update(float delta_time, Vector2 player_position)
{
	if (alive) {
		
	}
}

class PartyPopper 
{
public:
	PartyPopper(Vector2 start_position, float start_rotation, float initial_speed);
	~PartyPopper();

	void Update(float delta_time, int map_width, int map_height);
	void Draw();
	bool IsActive() const { return active; };

private:
	Vector2 position;
	Vector2 velocity;
	float rotation;
	float speed{ 800 };
	float size{ 60 };
	Rectangle rect;
	bool active;
	float life_span{ 0.1f };
};

PartyPopper::PartyPopper(Vector2 start_position, float start_rotation, float initial_speed)
{
	position = start_position;
	rotation = start_rotation;
	active = true;

	float radians = rotation * DEG2RAD;
	velocity.x = cos(radians) * speed;
	velocity.y = sin(radians) * speed;
}

PartyPopper::~PartyPopper()
{
}

void PartyPopper::Update(float delta_time, int map_width, int map_height)
{
	if (active)
	{
		life_span -= delta_time;
		if (life_span <= 0.0f)
		{
			active = false;
			return;
		}
		position.x += velocity.x * delta_time;
		position.y += velocity.y * delta_time;

		if (position.x < 0 || position.x > map_width || position.y < 0 || position.y > map_height)
		{
			active = false;
		}
	}
}

void PartyPopper::Draw()
{
	if (active)
	{
		rect = { position.x, position.y, size, size };
		Vector2 origin = { size / 2, size / 2 };
		DrawRectanglePro(rect, origin, rotation, PINK);
	}
}

int main() {
	constexpr int screen_width = 1600;
	constexpr int screen_height = 1000;

	constexpr int map_width = 3200;
	constexpr int map_height = 2000;

	InitWindow(screen_width, screen_height, "Don't Stop the party");
	InitAudioDevice();
	SetTargetFPS(60);

	Character hero;
	hero.SetPosition({ map_width / 2.0f, map_height / 2.0f });

	std::vector<Enemy> enemies;
	int number_of_enemies = 8;
	for (int i = 0; i < number_of_enemies; i++) {
		Enemy new_enemy;
		Vector2 random_position = {
			(float)GetRandomValue(0, map_width),
			(float)GetRandomValue(0, map_height)
		};
		new_enemy.SetPosition(random_position);
		enemies.push_back(new_enemy); 
	}

	std::vector<PartyPopper> partyPoppers;

	Camera2D camera = { 0 };
	camera.target = hero.GetPosition();
	camera.offset = { screen_width / 2.0f, screen_height / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	Music bgm = LoadMusicStream("resources/party2.mp3");
	Sound shoot_sfx = LoadSound("resources/Boom1.wav");
	PlayMusicStream(bgm);

	while (!WindowShouldClose()) {
		float delta_time = GetFrameTime();
		//UpdateMusicStream(bgm);
		hero.Update(delta_time, map_width, map_height);
		for (auto& enemy : enemies) {
			enemy.Update(delta_time, hero.GetPosition());
		}

		if (IsKeyPressed(KEY_ENTER))
		{
			PartyPopper newPopper(hero.GetPosition(), hero.GetRotation(), hero.GetSpeed());
			partyPoppers.push_back(newPopper);
			PlaySound(shoot_sfx);
		}

		for (auto& popper : partyPoppers)
		{
			popper.Update(delta_time, map_width, map_height);
		}

		camera.target = hero.GetPosition();

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode2D(camera);

	
		DrawRectangle(0, 0, map_width, map_height, Fade(LIGHTGRAY, 0.5f));

	
		DrawCircle(map_width/2, map_height/2, 20, BLUE);

		hero.Draw();
		for (auto& enemy : enemies) {
			enemy.Draw();
		}

		for (auto& popper : partyPoppers)
		{
			popper.Draw();
		}
		partyPoppers.erase(
			std::remove_if(partyPoppers.begin(), partyPoppers.end(), [](const PartyPopper& p) {
				return !p.IsActive();
				}),
			partyPoppers.end()
		);
		EndMode2D();

		DrawText("Welcome to Party", 10, 10, 20, BLACK);

		EndDrawing();
	}
	CloseWindow();
	UnloadMusicStream(bgm);
	UnloadSound(shoot_sfx);
	CloseAudioDevice();
	return 0;
}