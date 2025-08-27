#include "Game.h"

Game::Game()
	: screen_width(800), screen_height(600), map_width(4800), map_height(3600), number_of_enemies(50),
	shake_timer(0.0f), shake_intensity(15.0f)
{
	InitWindow(screen_width, screen_height, "Don't Stop the party");
	InitAudioDevice();
	SetTargetFPS(60);

	explosion_texture = LoadTexture("resources/Explosion.png");
	light_texture = LoadRenderTexture(map_width, map_height);
	bgm = LoadMusicStream("resources/party2.mp3");
	shoot_sfx = LoadSound("resources/Boom1.wav");
	PlayMusicStream(bgm);
	hero.LoadResources();

	camera.target = hero.GetPosition();
	camera.offset = { (float)screen_width / 2.0f, (float)screen_height / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 2.0f;
	InitGame();
}

Game::~Game()
{
	UnloadTexture(explosion_texture);
	UnloadRenderTexture(light_texture);
	UnloadMusicStream(bgm);
	UnloadSound(shoot_sfx);
	hero.UnloadResources();
	CloseAudioDevice();
	CloseWindow();
}

void Game::InitGame()
{
	hero.SetPosition({ map_width / 2.0f, map_height / 2.0f });
	for (int i = 0; i < number_of_enemies; i++)
	{
		Enemy new_enemy;
		Vector2 random_position = { (float)GetRandomValue(0, map_width), (float)GetRandomValue(0, map_height) };
		new_enemy.SetPosition(random_position);
		enemies.push_back(new_enemy);
	}
	score = 0;
}

void Game::ResetGame()
{
	enemies.clear();
	partyPoppers.clear();
	spotlights.clear();
	hero.SetAlive(true);
	hero.GetPopper();
	InitGame();
}

void Game::Run()
{
	while (!WindowShouldClose())
	{
		Update();
		Draw();
	}
}

void Game::Update()
{
	float delta_time = GetFrameTime();
	UpdateMusicStream(bgm);

	if (GetRandomValue(0, 1) == 0)
	{
		spotlights.emplace_back(map_width, map_height);
	}

	if (IsKeyPressed(KEY_ENTER) && hero.HasPopper())
	{
		hero.UsePopper();
		PartyPopper newPopper(hero.GetPosition(), hero.GetRotation(), hero.GetSpeed(), explosion_texture);
		partyPoppers.push_back(newPopper);
		PlaySound(shoot_sfx);

		float radians = (hero.GetRotation() - 90.0f) * DEG2RAD;
		Vector2 backward_vector = { -cosf(radians), -sinf(radians) };
		hero.Knockback(backward_vector, 60.0f);

		shake_timer = 0.2f;
	}

	if (IsKeyPressed(KEY_R)) {
		ResetGame();
	}

	hero.Update(delta_time, map_width, map_height);
	for (auto& light : spotlights)
	{
		light.Update(delta_time);
	}
	for (auto& enemy : enemies)
	{

		enemy.Update(delta_time);
	}
	for (auto& popper : partyPoppers)
	{
		popper.Update(delta_time, map_width, map_height);
	}

	for (auto& popper : partyPoppers)
	{
		if (!popper.IsActive())
			continue;

		for (auto& enemy : enemies)
		{
			if (!enemy.IsActive())
				continue;
			if (CheckCollisionRecs(popper.GetHitbox(), enemy.GetHitbox()))
			{
				popper.SetHit();
				enemy.GotHit();
				popper.Deactivate();
				hero.GetPopper();
				score += 1000;
				break;
			}
		}
	}

	spotlights.erase(
		std::remove_if(spotlights.begin(), spotlights.end(), [](const Spotlight& s) { return !s.IsActive(); }),
		spotlights.end());

	for (int i = partyPoppers.size() - 1; i >= 0; i--)
	{
		if (!partyPoppers[i].IsActive())
		{
			if (!partyPoppers[i].DidHit())
			{
				hero.SetAlive(false);
			}
			partyPoppers.erase(partyPoppers.begin() + i);
		}
	}

	camera.target = hero.GetPosition();
	if (shake_timer > 0.0f)
	{
		camera.offset.x = screen_width / 2.0f + GetRandomValue(-shake_intensity, shake_intensity);
		camera.offset.y = screen_height / 2.0f + GetRandomValue(-shake_intensity, shake_intensity);
		shake_timer -= delta_time;
	}
	else
	{
		camera.offset.x = screen_width / 2.0f;
		camera.offset.y = screen_height / 2.0f;
	}
}

void Game::Draw()
{

	BeginTextureMode(light_texture);
	ClearBackground(BLANK);
	for (auto& light : spotlights)
	{
		light.Draw(map_width, map_height);
	}
	EndTextureMode();

	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode2D(camera);

	for (int x = 0; x <= map_width; x += grid_spacing)
	{
		DrawLine(x, 0, x, map_height, grid_color);
	}
	for (int y = 0; y <= map_height; y += grid_spacing)
	{
		DrawLine(0, y, map_width, y, grid_color);
	}

	for (auto& enemy : enemies)
	{
		enemy.Draw();
	}
	hero.Draw();

	for (auto& popper : partyPoppers)
	{
		popper.Draw();
	}

	BeginBlendMode(BLEND_ADDITIVE);

	Rectangle sourceRec = { 0, 0, (float)light_texture.texture.width, (float)-light_texture.texture.height };

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			Vector2 position = { (float)(i * map_width), (float)(j * map_height) };
			DrawTextureRec(light_texture.texture, sourceRec, position, WHITE);
		}
	}

	EndBlendMode();
	EndMode2D();

	DrawText(TextFormat("Score: %d", score), 10, 10, 40, WHITE);

	EndDrawing();
}
