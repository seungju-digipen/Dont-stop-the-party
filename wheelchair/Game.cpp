#include "Game.h"

Game::Game()
    : screen_width(800), screen_height(600), map_width(2400), map_height(1800), hero(player_texture),
      number_of_enemies(50), shake_timer(0.0f), shake_intensity(15.0f)
{
    InitWindow(screen_width, screen_height, "Don't Stop the party");
    InitAudioDevice();
    LoadSettings();
    bgm_track_names = {"resources/party1.mp3", "resources/party2.mp3", "resources/party3.mp3"};

    for (const char *path : bgm_track_names)
    {
        Music track = LoadMusicStream(path);

        bgm_tracks.push_back(track);
    }
    SetTargetFPS(60);
    LoadRanking();

    player_texture = LoadTexture("resources/PlayerSheet.png");
    enemy_texture = LoadTexture("resources/People.png");
    explosion_texture = LoadTexture("resources/Explosion.png");
    hero.SetTexture(player_texture);
    light_texture = LoadRenderTexture(map_width, map_height);
    shoot_sfx = LoadSound("resources/Boom1.wav");
    score_sfxs.push_back(LoadSound("resources/thankyou1.mp3"));
    score_sfxs.push_back(LoadSound("resources/thankyou2.mp3"));
    score_sfxs.push_back(LoadSound("resources/thankyou3.mp3"));
    score_sfxs.push_back(LoadSound("resources/thankyou4.mp3"));
    score_sfxs.push_back(LoadSound("resources/thankyou5.mp3"));

    ApplySettings();

    current_screen = GameScreen::MAIN_MENU;
    menu_index = 0;

    camera.target = hero.GetPosition();
    camera.offset = {(float)screen_width / 2.0f, (float)screen_height / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;
    InitGame();
}

Game::~Game()
{
    SaveRanking();
    SaveSettings();
    UnloadTexture(player_texture);
    UnloadTexture(enemy_texture);
    UnloadTexture(explosion_texture);
    UnloadRenderTexture(light_texture);
    for (Music &bgm : bgm_tracks)
    {
        UnloadMusicStream(bgm);
    }
    UnloadSound(shoot_sfx);
    for (Sound &sfx : score_sfxs)
    {
        UnloadSound(sfx);
    }
    CloseAudioDevice();
    CloseWindow();
}

void Game::InitGame()
{
    hero.SetPosition({map_width / 2.0f, map_height / 2.0f});
    for (int i = 0; i < number_of_enemies; i++)
    {
        Enemy new_enemy(enemy_texture);
        new_enemy.SetTexture(enemy_texture);
        Vector2 random_position = {
            (float)GetRandomValue(new_enemy.GetGenRagne(), map_width - new_enemy.GetGenRagne()),
            (float)GetRandomValue(new_enemy.GetGenRagne(), map_height - new_enemy.GetGenRagne())};
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

    SeekMusicStream(bgm_tracks[settings.current_bgm_index], 0.0f);

    InitGame();
}

bool Game::IsHighScore(int score)
{
    if (high_scores.size() < 5)
    {
        return true;
    }
    return score > high_scores.back().score;
}

void Game::LoadSettings()
{
    if (FileExists("setting.txt"))
    {
        const char *file_data = LoadFileText("setting.txt");
        std::stringstream ss(file_data);
        std::string line;

        while (std::getline(ss, line))
        {
            if (line.empty())
                continue;

            std::stringstream line_ss(line);
            std::string key, value;

            if (std::getline(line_ss, key, ':') && std::getline(line_ss, value))
            {
                if (key == "bgm_volume")
                {
                    settings.bgm_volume = std::stof(value);
                }
                else if (key == "sfx_enabled")
                {
                    settings.sfx_enabled = std::stoi(value);
                }
                else if (key == "current_bgm_index")
                {
                    settings.current_bgm_index = std::stoi(value);
                }
            }
        }
        UnloadFileText((char *)file_data);
    }
}

void Game::SaveSettings()
{
    std::string text_data = "";
    text_data += "bgm_volume:" + std::to_string(settings.bgm_volume) + "\n";
    text_data += "sfx_enabled:" + std::to_string(settings.sfx_enabled) + "\n";
    text_data += "current_bgm_index:" + std::to_string(settings.current_bgm_index) + "\n";

    SaveFileText("setting.txt", (char *)text_data.c_str());
}

void Game::ApplySettings()
{
    SetMusicVolume(bgm_tracks[settings.current_bgm_index], settings.bgm_volume);
    PlayMusicStream(bgm_tracks[settings.current_bgm_index]);
}

void Game::SpawnConfetti(Vector2 position)
{
    for (int i = 0; i < 10; i++)
    {
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue(50, 300);

        ConfettiParticle p;
        p.position = position;
        p.velocity = {cosf(angle) * speed, sinf(angle) * speed};
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
    while (!WindowShouldClose() && current_screen != GameScreen::EXIT)
    {
        Update();
        Draw();
    }
}
void Game::LoadRanking()
{
    high_scores.clear();
    const char *file_data = LoadFileText("ranking.txt");

    if (file_data != NULL)
    {
        std::stringstream ss(file_data);
        std::string line;

        while (std::getline(ss, line))
        {
            if (line.empty())
                continue;

            std::stringstream line_ss(line);
            std::string score_str, name_str;

            if (std::getline(line_ss, score_str, ',') && std::getline(line_ss, name_str))
            {
                high_scores.push_back({name_str, std::stoi(score_str)});
            }
        }
        UnloadFileText((char *)file_data);
    }
}

void Game::SaveRanking()
{
    std::string text_data = "";
    for (const auto &hs : high_scores)
    {
        text_data += std::to_string(hs.score) + "," + hs.name + "\n";
    }

    SaveFileText("ranking.txt", (char *)text_data.c_str());
}
void Game::AddScoreToRanking(HighScore new_hs)
{
    high_scores.push_back(new_hs);

    std::sort(high_scores.begin(), high_scores.end(),
              [](const HighScore &a, const HighScore &b) { return a.score > b.score; });

    if (high_scores.size() > 5)
    {
        high_scores.resize(5);
    }
}

void Game::UpdateMainMenu()
{
    if (IsKeyPressed(KEY_S))
    {
        menu_index = (menu_index + 1) % 5;
    }

    if (IsKeyPressed(KEY_W))
    {
        menu_index = (menu_index - 1 + 5) % 5;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        switch (menu_index)
        {
        case 0:
            ResetGame();
            current_screen = GameScreen::GAMEPLAY;
            break;
        case 1:
            current_screen = GameScreen::RANKING_SCREEN;
            break;
        case 2:
            current_screen = GameScreen::OPTIONS;
            break;
        case 3:
            current_screen = GameScreen::HOW_TO_PLAY;
            break;
        case 4:
            current_screen = GameScreen::EXIT;
            break;
        }
    }
}
void Game::UpdateGameplay()
{

    float delta_time = GetFrameTime();
    UpdateMusicStream(bgm_tracks[settings.current_bgm_index]);

    if (IsKeyPressed(KEY_R))
    {
        ResetGame();
    }

    if (hero.IsAlive())
    {
        if (IsKeyPressed(KEY_ENTER) && hero.HasPopper())
        {
            hero.UsePopper();
            PartyPopper newPopper(hero.GetPosition(), hero.GetRotation(), hero.GetSpeed(), explosion_texture);
            partyPoppers.push_back(newPopper);
            if (settings.sfx_enabled)
            {
                PlaySound(shoot_sfx);
            }
            SpawnConfetti(hero.GetPosition());
            float radians = (hero.GetRotation() - 90.0f) * DEG2RAD;
            Vector2 backward_vector = {-cosf(radians), -sinf(radians)};
            hero.Knockback(backward_vector, 60.0f);

            shake_timer = 0.2f;
        }

        for (auto &popper : partyPoppers)
        {
            if (!popper.IsActive())
                continue;

            for (auto &enemy : enemies)
            {
                if (!enemy.IsActive())
                    continue;

                if (CheckCollisionRecs(popper.GetHitbox(), enemy.GetHitbox()))
                {
                    popper.SetHit();
                    enemy.GotHit();
                    hero.GetPopper();
                    score += (hero.GetSpeed() > hero.GetMinSpeed()) ? (1000 + (((int)hero.GetSpeed() / 10) * 10)) : 500;
                    if (settings.sfx_enabled && !score_sfxs.empty())
                    {
                        int random_index = GetRandomValue(0, score_sfxs.size() - 1);
                        PlaySound(score_sfxs[random_index]);
                    }
                    break;
                }
            }
        }
    }

    if (GetRandomValue(0, 30) == 0)
    {
        spotlights.emplace_back(map_width, map_height);
    }

    hero.Update(delta_time, map_width, map_height);
    for (auto &light : spotlights)
    {
        light.Update(delta_time);
    }
    for (auto &enemy : enemies)
    {
        enemy.Update(delta_time, map_width, map_height);
    }
    for (auto &popper : partyPoppers)
    {
        popper.Update(delta_time, map_width, map_height);
    }
    for (auto &p : confettiParticles)
    {
        p.position.x += p.velocity.x * delta_time;
        p.position.y += p.velocity.y * delta_time;
        p.velocity.y += gravity * delta_time;
        p.rotation += p.angular_velocity * delta_time;
        p.life_span -= delta_time;
    }

    confettiParticles.erase(std::remove_if(confettiParticles.begin(), confettiParticles.end(),
                                           [](const ConfettiParticle &p) { return p.life_span <= 0.0f; }),
                            confettiParticles.end());

    spotlights.erase(
        std::remove_if(spotlights.begin(), spotlights.end(), [](const Spotlight &s) { return !s.IsActive(); }),
        spotlights.end());

    for (int i = partyPoppers.size() - 1; i >= 0; i--)
    {
        if (!partyPoppers[i].IsActive())
        {
            if (!partyPoppers[i].DidHit())
            {
                hero.SetAlive(false);
                UpdateRanking();
            }
            partyPoppers.erase(partyPoppers.begin() + i);
        }
    }

    confettiParticles.erase(std::remove_if(confettiParticles.begin(), confettiParticles.end(),
                                           [](const ConfettiParticle &p) { return p.life_span <= 0.0f; }),
                            confettiParticles.end());
    if (!hero.IsAlive())
    {
        current_screen = GameScreen::GAME_OVER;
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
void Game::UpdateRanking()
{

    if (IsHighScore(score))
    {

        current_screen = GameScreen::NAME_ENTRY;
        selected_letter_index = 0;
        player_name[0] = 'A';
        player_name[1] = 'A';
        player_name[2] = 'A';
        player_name[3] = '\0';
    }
    else
    {
        current_screen = GameScreen::MAIN_MENU;
    }
}
void Game::UpdateRankingScreen()
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        current_screen = GameScreen::MAIN_MENU;
    }
}
void Game::UpdateNameEntry()
{
    if (IsKeyPressed(KEY_D))
    {
        selected_letter_index = (selected_letter_index + 1) % 3;
    }
    if (IsKeyPressed(KEY_A))
    {
        selected_letter_index = (selected_letter_index - 1 + 3) % 3;
    }

    if (IsKeyPressed(KEY_W))
    {
        player_name[selected_letter_index]++;
        if (player_name[selected_letter_index] > 'Z')
        {
            player_name[selected_letter_index] = 'A';
        }
    }
    if (IsKeyPressed(KEY_S))
    {
        player_name[selected_letter_index]--;
        if (player_name[selected_letter_index] < 'A')
        {
            player_name[selected_letter_index] = 'Z';
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        AddScoreToRanking({std::string(player_name), score});
        current_screen = GameScreen::MAIN_MENU;
    }
}
void Game::Update()
{
    switch (current_screen)
    {
    case GameScreen::MAIN_MENU:
        UpdateMainMenu();
        break;
    case GameScreen::GAMEPLAY:
        UpdateGameplay();
        break;
    case GameScreen::NAME_ENTRY:
        UpdateNameEntry();
        break;
    case GameScreen::RANKING_SCREEN:
        UpdateRankingScreen();
        break;
    case GameScreen::OPTIONS:
        UpdateOptions();
        break;
    case GameScreen::HOW_TO_PLAY:
        UpdateHowToPlay();
        break;
    case GameScreen::GAME_OVER:
        UpdateGameOver();
        break;
    default:
        break;
    }
}

void Game::UpdateOptions()
{

    if (IsKeyPressed(KEY_S))
    {
        options_menu_index = (options_menu_index + 1) % 3;
    }
    if (IsKeyPressed(KEY_W))
    {
        options_menu_index = (options_menu_index - 1 + 3) % 3;
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
    {
        float direction = IsKeyDown(KEY_D) ? 1.0f : -1.0f;

        switch (options_menu_index)
        {
        case 0:
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D))
            {
                StopMusicStream(bgm_tracks[settings.current_bgm_index]);
                settings.current_bgm_index =
                    (settings.current_bgm_index + (int)direction + bgm_tracks.size()) % bgm_tracks.size();
                PlayMusicStream(bgm_tracks[settings.current_bgm_index]);
            }
            break;
        case 1:
            settings.bgm_volume += 0.05f * direction;
            if (settings.bgm_volume > 1.0f)
                settings.bgm_volume = 1.0f;
            if (settings.bgm_volume < 0.0f)
                settings.bgm_volume = 0.0f;
            SetMusicVolume(bgm_tracks[settings.current_bgm_index], settings.bgm_volume);
            break;
        case 2:
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D))
            {
                settings.sfx_enabled = !settings.sfx_enabled;
            }
            break;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        current_screen = GameScreen::MAIN_MENU;
    }
}

void Game::UpdateHowToPlay()
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        current_screen = GameScreen::MAIN_MENU;
    }
}

void Game::UpdateGameOver()
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        current_screen = GameScreen::MAIN_MENU;
    }
}

void Game::DrawGameplay()
{
    BeginMode2D(camera);

    for (int x = 0; x <= map_width; x += grid_spacing)
    {
        DrawLineEx({(float)x, 0}, {(float)x, (float)map_height}, 2, grid_color);
    }
    for (int y = 0; y <= map_height; y += grid_spacing)
    {
        DrawLineEx({0, (float)y}, {(float)map_width, (float)y}, 2, grid_color);
    }

    for (auto &enemy : enemies)
    {
        enemy.Draw();
    }
    if (hero.IsAlive())
    {
        hero.Draw();
    }
    for (auto &popper : partyPoppers)
    {
        popper.Draw();
    }

    BeginBlendMode(BLEND_ADDITIVE);

    Rectangle sourceRec = {0, 0, (float)light_texture.texture.width, (float)-light_texture.texture.height};

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            Vector2 position = {(float)(i * map_width), (float)(j * map_height)};
            DrawTextureRec(light_texture.texture, sourceRec, position, WHITE);
        }
    }

    for (const auto &p : confettiParticles)
    {
        Rectangle particle_rec = {p.position.x, p.position.y, p.size, p.size};
        DrawRectanglePro(particle_rec, {p.size / 2, p.size / 2}, p.rotation, p.color);
    }

    EndBlendMode();
    EndMode2D();

    DrawText(TextFormat("Score: %d", score), 10, 10, 40, WHITE);
}
void Game::DrawMainMenu()
{
    const char *menu_items[] = {"START GAME", "RANKING", "OPTION", "HOW TO PLAY", "EXIT"};
    int text_size = 40;
    int start_y = screen_height / 2 - 100;

    DrawText("Don't Stop the Party", screen_width / 2 - MeasureText("Don't Stop the Party", 60) / 2, start_y - 80, 60,
             WHITE);

    for (int i = 0; i < 5; i++)
    {
        Color color = (i == menu_index) ? YELLOW : GRAY;
        DrawText(menu_items[i], screen_width / 2 - MeasureText(menu_items[i], text_size) / 2, start_y + i * 50,
                 text_size, color);
    }
}
void Game::DrawNameEntry()
{
    DrawText("NEW HIGH SCORE!", screen_width / 2 - MeasureText("NEW HIGH SCORE!", 40) / 2, 150, 40, GOLD);
    DrawText(TextFormat("YOUR SCORE: %i", score),
             screen_width / 2 - MeasureText(TextFormat("YOUR SCORE: %i", score), 20) / 2, 210, 20, LIGHTGRAY);
    DrawText("ENTER YOUR NAME (3 LETTERS)", screen_width / 2 - MeasureText("ENTER YOUR NAME (3 LETTERS)", 20) / 2, 300,
             20, GRAY);

    int text_size = 50;
    int start_x = screen_width / 2 - MeasureText("A B C", text_size) / 2;
    int y_pos = 350;

    DrawText(TextFormat("%c %c %c", player_name[0], player_name[1], player_name[2]), start_x, y_pos, text_size, WHITE);

    if ((int)(GetTime() * 2.0f) % 2 == 0)
    {
        int cursor_x = start_x + (MeasureText("A ", text_size) * selected_letter_index);
        DrawText("_", cursor_x, y_pos + 10, text_size, YELLOW);
    }
}
void Game::DrawRankingScreen()
{
    int start_y = 150;
    DrawText("HALL OF FAME", screen_width / 2 - MeasureText("HALL OF FAME", 60) / 2, start_y - 80, 60, GOLD);

    if (high_scores.empty())
    {
        DrawText("NO SCORES YET", screen_width / 2 - MeasureText("NO SCORES YET", 30) / 2, start_y + 50, 30, GRAY);
    }
    else
    {
        for (int i = 0; i < high_scores.size(); i++)
        {
            Color color = (i == 0) ? YELLOW : WHITE;
            const char *text =
                TextFormat("%d. %s ............ %d", i + 1, high_scores[i].name.c_str(), high_scores[i].score);
            DrawText(text, screen_width / 2 - MeasureText(text, 40) / 2, start_y + i * 50, 40, color);
        }
    }

    DrawText("Press ENTER to return to Menu", screen_width / 2 - MeasureText("Press ENTER to return to Menu", 20) / 2,
             screen_height - 50, 20, GRAY);
}

void Game::DrawOptions()
{
    int start_y = 150;
    DrawText("OPTIONS", screen_width / 2 - MeasureText("OPTIONS", 60) / 2, start_y - 80, 60, WHITE);

    DrawText("BGM", 150, start_y, 40, (options_menu_index == 0) ? YELLOW : GRAY);
    DrawText(TextFormat("< %s >", GetFileNameWithoutExt(bgm_track_names[settings.current_bgm_index])), 450, start_y, 40,
             (options_menu_index == 0) ? YELLOW : GRAY);

    DrawText("VOLUME", 150, start_y + 60, 40, (options_menu_index == 1) ? YELLOW : GRAY);
    DrawRectangle(450, start_y + 60, 200, 40, LIGHTGRAY);
    DrawRectangle(450, start_y + 60, (int)(settings.bgm_volume * 200.0f), 40,
                  (options_menu_index == 1) ? YELLOW : WHITE);

    DrawText("SFX", 150, start_y + 120, 40, (options_menu_index == 2) ? YELLOW : GRAY);
    DrawText(settings.sfx_enabled ? "ON" : "OFF", 450, start_y + 120, 40, (options_menu_index == 2) ? YELLOW : GRAY);

    DrawText("Press ENTER to return to Menu", screen_width / 2 - MeasureText("Press ENTER to return to Menu", 20) / 2,
             screen_height - 50, 20, GRAY);
}
void Game::DrawHowToPlay()
{

    int start_y = 30;
    int start_x = 30;
    int line_spacing_body = 30;
    int line_spacing_header = 40;
    int section_spacing = 20;

    int current_y = start_y;

    // --- Objective ---
    DrawText("Objective", start_x, current_y, 30, GOLD);
    current_y += line_spacing_header;
    DrawText("The goal is to celebrate the party participants by hitting them", start_x, current_y, 20, LIGHTGRAY);
    current_y += line_spacing_body;
    DrawText("with a Party Popper to score points.", start_x, current_y, 20, LIGHTGRAY);
    current_y += line_spacing_body;
    DrawText("You only have one shot. You must hit a participant to get another popper.", start_x, current_y, 20,
             WHITE);
    current_y += line_spacing_body;
    DrawText("If you miss and the popper hits nothing, the game is over.", start_x, current_y, 20, RED);
    current_y += line_spacing_header + section_spacing;

    // --- Controls ---
    DrawText("Controls", start_x, current_y, 30, GOLD);
    current_y += line_spacing_header;
    DrawText("- A / D: Rotate Left / Right", start_x, current_y, 20, LIGHTGRAY);
    current_y += line_spacing_body;
    DrawText("- Spacebar: Brake", start_x, current_y, 20, LIGHTGRAY);
    current_y += line_spacing_body;
    DrawText("- Enter: Fire Party Popper", start_x, current_y, 20, LIGHTGRAY);
    current_y += line_spacing_body;
    DrawText("- R: Restart", start_x, current_y, 20, LIGHTGRAY);
    current_y += line_spacing_header + section_spacing;

    // --- Score ---
    DrawText("Score", start_x, current_y, 30, GOLD);
    current_y += line_spacing_header;
    DrawText("Hitting a participant earns a base score.", start_x, current_y, 20, LIGHTGRAY);
    current_y += line_spacing_body;
    DrawText("The faster you are moving, the higher the bonus score.", start_x, current_y, 20, LIGHTGRAY);

    // --- Return to Menu ---
    DrawText("Press ENTER to return to Menu", screen_width / 2 - MeasureText("Press ENTER to return to Menu", 20) / 2,
             screen_height - 40, 20, GRAY);
}

void Game::DrawGameOver()
{
    DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 40) / 2, GetScreenHeight() / 2 - 20, 40, RED);
    DrawText("Press [SPACEBAR] to return to the Main Menu",
             GetScreenWidth() / 2 - MeasureText("Press [SPACEBAR] to return to the Main Menu", 20) / 2,
             GetScreenHeight() / 2 + 30, 20, GRAY);
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    switch (current_screen)
    {
    case GameScreen::MAIN_MENU:
        DrawMainMenu();
        break;
    case GameScreen::GAMEPLAY:
        DrawGameplay();
        break;
    case GameScreen::NAME_ENTRY:
        DrawNameEntry();
        break;
    case GameScreen::RANKING_SCREEN:
        DrawRankingScreen();
        break;
    case GameScreen::OPTIONS:
        DrawOptions();
        break;
    case GameScreen::HOW_TO_PLAY:
        DrawHowToPlay();
        break;
    case GameScreen::GAME_OVER:
        DrawGameOver();
        break;
    }
    EndDrawing();
}
