#include "Game.h"

Game::Game()
	: screen_width(800), screen_height(600), map_width(2400), map_height(1800), number_of_enemies(50),
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
		Vector2 random_position = { (float)GetRandomValue(new_enemy.GetGenRagne(), map_width - new_enemy.GetGenRagne()), (float)GetRandomValue(new_enemy.GetGenRagne(), map_height - new_enemy.GetGenRagne()) };
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
	confettiParticles.clear();
	hero.SetAlive(true);
	hero.GetPopper();


	SeekMusicStream(bgm, 0.0f);

	InitGame();
}

void Game::SpawnConfetti(Vector2 position)
{
	for (int i = 0; i < 10; i++)
	{
		float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
		float speed = (float)GetRandomValue(50, 300);

		ConfettiParticle p;
		p.position = position;
		p.velocity = { cosf(angle) * speed, sinf(angle) * speed };
		p.rotation = (float)GetRandomValue(0, 360);
		p.angular_velocity = (float)GetRandomValue(-300, 300);
		p.size = (float)GetRandomValue(5, 15);
		p.life_span = (float)GetRandomValue(5, 20) / 10.0f;
		p.color = ColorFromHSV((float)GetRandomValue(0, 360), 0.8f, 0.95f);

		confettiParticles.push_back(p);
	}
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

	if (IsKeyPressed(KEY_R)) {
		ResetGame();
	}

	if (hero.IsAlive())
	{
		if (IsKeyPressed(KEY_ENTER) && hero.HasPopper())
		{
			hero.UsePopper();
			PartyPopper newPopper(hero.GetPosition(), hero.GetRotation(), hero.GetSpeed(), explosion_texture);
			partyPoppers.push_back(newPopper);
			PlaySound(shoot_sfx);
			SpawnConfetti(hero.GetPosition());
			float radians = (hero.GetRotation() - 90.0f) * DEG2RAD;
			Vector2 backward_vector = { -cosf(radians), -sinf(radians) };
			hero.Knockback(backward_vector, 60.0f);

			shake_timer = 0.2f;
		}
	}

	if (GetRandomValue(0, 30) == 0)
	{
		spotlights.emplace_back(map_width, map_height);
	}

	hero.Update(delta_time, map_width, map_height);
	for (auto& light : spotlights) { light.Update(delta_time); }
	for (auto& enemy : enemies) { enemy.Update(delta_time, map_width, map_height); }
	for (auto& popper : partyPoppers) { popper.Update(delta_time, map_width, map_height); }
	for (auto& p : confettiParticles)
	{

		p.position.x += p.velocity.x * delta_time;
		p.position.y += p.velocity.y * delta_time;


		p.velocity.y += gravity * delta_time;


		p.rotation += p.angular_velocity * delta_time;


		p.life_span -= delta_time;
	}


	confettiParticles.erase(
		std::remove_if(confettiParticles.begin(), confettiParticles.end(), [](const ConfettiParticle& p) {
			return p.life_span <= 0.0f;
			}),
		confettiParticles.end()
	);



	if (hero.IsAlive())
	{
		for (auto& popper : partyPoppers)
		{
			if (!popper.IsActive()) continue;

			for (auto& enemy : enemies)
			{
				if (!enemy.IsActive()) continue;

				if (CheckCollisionRecs(popper.GetHitbox(), enemy.GetHitbox()))
				{
					popper.SetHit();
					enemy.GotHit();
					hero.GetPopper();
					score += (hero.GetSpeed() > hero.GetMinSpeed()) ? (1000 + (((int)hero.GetSpeed() / 10) * 10)) : 500;
					break;
				}
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

	confettiParticles.erase(
		std::remove_if(confettiParticles.begin(), confettiParticles.end(), [](const ConfettiParticle& p) {
			return p.life_span <= 0.0f;
			}),
		confettiParticles.end()
	);

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
	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode2D(camera);


	for (int x = 0; x <= map_width; x += grid_spacing)
	{
		DrawLineEx({ (float)x, 0 }, { (float)x, (float)map_height }, 2, grid_color);
	}
	for (int y = 0; y <= map_height; y += grid_spacing)
	{
		DrawLineEx({ 0, (float)y }, { (float)map_width, (float)y }, 2, grid_color);
	}

	for (auto& enemy : enemies) { enemy.Draw(); }
	if (hero.IsAlive()) { hero.Draw(); }
	for (auto& popper : partyPoppers) { popper.Draw(); }

	for (const auto& p : confettiParticles)
	{
		Rectangle particle_rec = { p.position.x, p.position.y, p.size, p.size };
		DrawRectanglePro(particle_rec, { p.size / 2, p.size / 2 }, p.rotation, p.color);
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

	for (const auto& p : confettiParticles)
	{
		Rectangle particle_rec = { p.position.x, p.position.y, p.size, p.size };
		DrawRectanglePro(particle_rec, { p.size / 2, p.size / 2 }, p.rotation, p.color);
	}

	EndBlendMode();
	EndMode2D();

	DrawText(TextFormat("Score: %d", score), 10, 10, 40, WHITE);
	if (!hero.IsAlive())
	{
		DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 40) / 2, GetScreenHeight() / 2 - 20, 40, RED);
		DrawText("Press [R] to Restart", GetScreenWidth() / 2 - MeasureText("Press [R] to Restart", 20) / 2, GetScreenHeight() / 2 + 30, 20, GRAY);
	}

	EndDrawing();
}