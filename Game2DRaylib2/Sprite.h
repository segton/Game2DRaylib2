#pragma once
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include "Settings.h"
#include <vector>
#include <functional>
#include <memory>

class Sprite
{
public:
    Texture2D texture{};
    Vector2 pos{};
    float speed = 0.0f;
    bool despawn = false;
    Vector2 direction{};
    Vector2 size{};
    float collisionRadius = 0.0f;

    Sprite() = default;

    Sprite(Texture texture, Vector2 pos, float speed, Vector2 direction) : texture(texture), pos(pos), speed(speed), direction(direction)
    {
        size = { (float)texture.width, (float)texture.height };
        collisionRadius = size.y / 2.f;
    }

    void move(float dt)
    {
        pos.x += direction.x * speed * dt;
        pos.y += direction.y * speed * dt;
    }

    virtual Vector2 getCenter() const
    {
        return {pos.x + size.x /2.f, pos.y + size.y / 2.f};
    }
    virtual void checkDespawn()
    {
        if (!(-300.0f < pos.y && pos.y < WINDOW_HEIGHT + 300.0f))
        {
            despawn = true;
            //std::cout << "despawned meteor\n";
        }
        else
        {
            despawn = false;
        }
    }
    virtual void update(float dt)
    {
        move(dt);
        checkDespawn();
    }
    virtual void draw() const
    {
        DrawTextureV(texture, pos, WHITE);
    }
};

class Player : public Sprite
{
public:
    std::function<void(Vector2)> shootLaser;

    Player() = default;

    Player(Texture2D texture, Vector2 pos, std::function<void(Vector2)> shootLaser)
        : Sprite(texture, pos, PLAYER_SPEED, Vector2{ 0, 0 }), shootLaser(shootLaser)

    {
    }

    void input()
    {
        direction.x = (float)(IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));
        direction.y = (float)(IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP));


        if (direction.x != 0.0f || direction.y != 0.0f)
            direction = Vector2Normalize(direction);

        if (IsKeyPressed(KEY_SPACE))
        {
            shootLaser({ pos.x + size.x / 2.0f,  pos.y - 60.f });
        }
    };



    void update(float dt) override
    {
        input();
        move(dt);
        constraint();
    }

    void constraint()
    {
        pos.x = std::max(0.0f, std::min(pos.x, (float)WINDOW_WIDTH - size.x));
        pos.y = std::max(0.0f, std::min(pos.y, (float)WINDOW_HEIGHT - size.y));
    }

};
class AnimatedExplosion
{
public:
    std::vector<Texture2D> animationFrames{};
    Vector2 size{};
    Vector2 pos{};
    float index = 0;
    float animationSpeed = 20.f;
    bool despawn = false;

    AnimatedExplosion(Vector2 pos, const std::vector<Texture2D>& animationFrames) : animationFrames(animationFrames)
    {
        size = { (float)animationFrames[0].width, (float)animationFrames[0].height };
        this->pos = { pos.x - size.x / 2.0f, pos.y - size.y / 2.0f };
    }

    void update(float dt)
    {
        if (index < (float)animationFrames.size() - 1.0f)
        {
            index += animationSpeed * dt;
        }
        else
        {
            despawn = true;
        }
    }
    void draw()
    {
        DrawTextureV(animationFrames[(int)index % animationFrames.size()], pos, WHITE);
    }
};
class Laser : public Sprite
{
public:
    Laser(Texture2D texture, Vector2 pos) : Sprite(texture, pos, LASER_SPEED, Vector2{ 0, -1 })
    {

    }
    Rectangle getRect() const
    {
        return { pos.x, pos.y, size.x, size.y };
    }
};
class Meteor : public Sprite
{
public:
    float rotation = 0.0f;
    Rectangle rect{};

    Meteor(Texture2D texture, Vector2 pos, float speed, Vector2 direction)
        : Sprite(texture, pos, speed, direction)
    {
        rect = { 0, 0, size.x, size.y };
    }
    void update(float dt) override
    {
        Sprite::update(dt);
        rotation += 50.0f * dt;
    }
    Vector2 getCenter() const override
    {
        return pos;
    }
    void draw() const override
    {
        Rectangle targetRect = { pos.x, pos.y, size.x, size.y };
        DrawTexturePro(texture,rect, targetRect, { size.x / 2.0f, size.y / 2.0f } ,rotation,WHITE);
    }
};