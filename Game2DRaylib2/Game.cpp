#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>


Game::Game()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Space Shooter");
    srand((unsigned)time(nullptr));
    InitAudioDevice();
    importAssets();

    player = Player(playerTex, { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f }, 
        [this](Vector2 pos) { shootLaser(pos); });

    meteorTimer = Timer(
        METEOR_TIMER_DURATION, true, true,
        [this]() { spawnMeteor(); }
    );
    

    PlayMusicStream(music);
}
Game::~Game()
{
    UnloadTexture(playerTex);
    UnloadTexture(laserTex);
    UnloadTexture(meteorTex);
    UnloadTexture(starTex);

    for (auto& tex : explosionFrames)
        UnloadTexture(tex);

    UnloadFont(font);

    UnloadSound(laserSound);
    UnloadSound(explodeSound);
    UnloadMusicStream(music);

    CloseAudioDevice();
    CloseWindow();
}
void Game::run()
{
    while (!WindowShouldClose() && !isGameOver)
    {
        update();
        draw();
    }
}
void Game::draw()
{
    BeginDrawing();
    ClearBackground(BG_COLOR);
    drawStars();
    player.draw();

    for (auto& l : lasers) l.draw();
    for (auto& m : meteors) m.draw();

    for (auto& a : explode) a.draw();
 
    drawTime();
    drawScore();
    EndDrawing();
}
void Game::update()
{
    float dt = GetFrameTime();
    meteorTimer.update();

    player.update(dt);
    for (auto& l : lasers) l.update(dt);
    for (auto& m : meteors) m.update(dt);
    checkCollision();
    checkLaserCollision();
    for (auto& a : explode) a.update(dt);

    despawnSprites();
    UpdateMusicStream(music);
}
void Game::drawStars()
{
    for (const auto& star : starData)
    {
        DrawTextureEx(starTex, star.first, 0.0f, star.second, WHITE);
    }
}

void Game::spawnMeteor()
{
    float x = (float)(rand() % WINDOW_WIDTH);
    float y = -100.f;

    float speed = (float)(METEOR_SPEED_MIN + rand() % (METEOR_SPEED_MAX - METEOR_SPEED_MIN));

    Vector2 dir = { ((float)rand() / RAND_MAX) - 0.5f, 1.0f };
    meteors.push_back(Meteor(meteorTex, {x,y}, speed, dir));
}

void Game::shootLaser(Vector2 pos)
{
    lasers.push_back(Laser(laserTex, pos));
      PlaySound(laserSound);
}

void Game::drawTime() const
{
    int time = (int)GetTime();
    std::string timetext = std::to_string(time);

    Vector2 textSize = MeasureTextEx(font, timetext.c_str(), (float)FONT_SIZE, 0.0f);

    DrawTextEx(font, timetext.c_str(), { (WINDOW_WIDTH /2.f - textSize.x / 2.f ), 100.f}, FONT_SIZE, 0.f, WHITE);
}

void Game::drawScore()
{
    scoreString = "Score: " + std::to_string(scoreInt);
    Vector2 textSize = MeasureTextEx(score, scoreString.c_str(), (float)FONT_SCORE_SIZE, 0.0f);
    DrawTextEx(score, scoreString.c_str(), { (WINDOW_WIDTH / 2.f - textSize.x / 2.f + 600), 100.f }, FONT_SCORE_SIZE, 0.f, WHITE);
}

void Game::checkCollision()
{
    for (auto& m : meteors)
    {
        bool collided = CheckCollisionCircles(player.getCenter(), player.collisionRadius, m.getCenter(), m.collisionRadius);
        if (collided)
        {
            std::cout << "collided with meteor!";
            isGameOver = true;
        }
    }
}

void Game::checkLaserCollision()
{
    for (auto& l : lasers)
    {
        for (auto& m : meteors)
        {
            bool collided = CheckCollisionCircleRec(m.getCenter(),m.collisionRadius, l.getRect());
            if (collided)
            {
                PlaySound(explodeSound);

                m.despawn = true;
                l.despawn = true;

                ++scoreInt;
                Vector2 pos = { l.pos.x, l.pos.y};
                explode.push_back(AnimatedExplosion(pos, explosionFrames));
            }
        }
    }
}

void Game::despawnSprites()
{
    for (auto it = lasers.begin(); it != lasers.end();)
    {
        if (it->despawn)
        {
            it = lasers.erase(it);
        }
        else
        {
            ++it;
        }
    }

    meteors.erase(
        std::remove_if(meteors.begin(), meteors.end(),
            [](const Meteor& m) { return m.despawn; }),
        meteors.end()
    );
    
    explode.erase(
    std::remove_if(explode.begin(), explode.end(),
        [](const AnimatedExplosion& e) { return e.despawn; }),
    explode.end()
    );
    

}

void Game::importAssets()
{
    playerTex = LoadTexture("images/spaceship.png");
    meteorTex = LoadTexture("images/meteor.png");
    laserTex = LoadTexture("images/laser.png");
    starTex = LoadTexture("images/star.png");


    font = LoadFontEx("font/Stormfaze.otf", FONT_SIZE, nullptr, 0);
    score = LoadFontEx("font/Stormfaze.otf", FONT_SCORE_SIZE, nullptr, 0);

    music = LoadMusicStream("audio/music.wav");

    laserSound = LoadSound("audio/laser.wav");
    explodeSound = LoadSound("audio/explosion.wav");

    for (int i = 1; i < 29; i++)
    {
        std::string path = "images/explosion/" + std::to_string(i) + ".png";
        explosionFrames.push_back(LoadTexture(path.c_str()));
    }

    starData.clear();
    for (int i = 0; i < 30; i++)
    {
        Vector2 pos = {
            (float)(rand() % (WINDOW_WIDTH + 1)),
            (float)(rand() % (WINDOW_HEIGHT + 1))
        };

        float scale = 0.5f + ((float)rand() / RAND_MAX) * (1.6f - 0.5f);
        starData.push_back({ pos, scale });
    }


}


