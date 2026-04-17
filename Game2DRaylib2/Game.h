#pragma once
#include "raylib.h"
#include "Sprite.h"
#include "Timer.h"
#include "Settings.h"
#include <vector>
#include <utility>

class Game
{
public:
	Game();
	~Game();
	void run();

private:
	void update();
	void draw();
	void drawStars();
	void importAssets();
	void shootLaser(Vector2 pos);
	void drawTime() const;
	void spawnMeteor();
	void checkCollision();
	void checkLaserCollision();
	void despawnSprites();
	void drawScore();

	Texture2D playerTex{};
	Texture2D meteorTex{};
	Texture2D starTex{};
	Texture2D laserTex{};
	Music music;
	Sound laserSound;
	Sound explodeSound;
	bool isGameOver = false;

	std::vector<Texture2D> explosionFrames;

	Font font;
	Font score;
	std::vector<std::pair<Vector2, float>> starData;

	Player player;
	std::vector<Laser> lasers;
	std::vector<Meteor> meteors;
	std::vector<AnimatedExplosion> explode;
	std::string scoreString;
	int scoreInt = 0;
	Timer meteorTimer;
};