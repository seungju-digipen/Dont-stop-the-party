#pragma once
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "Character.h"
#include "Enemy.h"
#include "PartyPopper.h"
#include "Spotlight.h"
#include "raylib.h"
#include "raymath.h"
#include "Particle.h" 
#include "GameScreen.h"

struct HighScore {
	std::string name;
	int score;
};

struct GameSettings {
	float bgm_volume{ 0.5f };     
	bool sfx_enabled{ true };     
	int current_bgm_index{ 0 }; 
};

class Game
{
public:
	Game();
	~Game();
	void Run();

private:
	void LoadRanking(); 
	void SaveRanking(); 
	void AddScoreToRanking(HighScore new_hs);

	void LoadSettings();
	void SaveSettings();
	void ApplySettings();

	void UpdateMainMenu();
	void UpdateGameplay();
	void UpdateRanking();
	void UpdateRankingScreen();
	void UpdateNameEntry();
	void UpdateOptions();
	void UpdateHowToPlay();
	void UpdateGameOver();
	void Update();

	void DrawMainMenu();
	void DrawGameplay();
	void DrawRankingScreen(); 
	void DrawNameEntry();
	void DrawOptions();
	void DrawHowToPlay();
	void DrawGameOver();
	void Draw();

	void InitGame();
	void ResetGame();

	bool IsHighScore(int score);
	void SpawnConfetti(Vector2 position);

	const int grid_spacing = 100;
	const Color grid_color = { 50, 50, 50, 255 };

	int screen_width, screen_height;
	int map_width, map_height;

	GameScreen current_screen;
	int menu_index;

	char player_name[4];
	int selected_letter_index{ 0 };

	int score;
	std::vector<HighScore> high_scores;

	GameSettings settings;
	int options_menu_index{ 0 };

	const float gravity = 400.0f;

	Texture2D player_texture;
	Texture2D enemy_texture;
	Texture2D explosion_texture;

	Character hero;
	std::vector<Enemy> enemies;
	int number_of_enemies;
	std::vector<PartyPopper> partyPoppers;
	std::vector<Spotlight> spotlights;
	std::vector<ConfettiParticle> confettiParticles;

	Camera2D camera;
	float shake_timer = 0.0f;
	float shake_intensity = 15.0f;


	Sound shoot_sfx;
	//Sound people_sound;
	std::vector<Music> bgm_tracks;
	std::vector<Sound> score_sfxs;
	std::vector<const char*> bgm_track_names;
	RenderTexture2D light_texture;
};
