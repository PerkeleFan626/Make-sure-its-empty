#include <iostream>
#include <vector>
#include <string>
#include <format>
#include "raylib.h"

// C++20 constants and types
namespace Constants {
    constexpr int ScreenWidth = 800;
    constexpr int ScreenHeight = 450;
    constexpr int PaddleWidth = 10;
    constexpr int PaddleHeight = 80;
    constexpr int BallRadius = 7;
    constexpr float PaddleSpeed = 400.0f;
    constexpr float BallSpeedX = 300.0f;
    constexpr float BallSpeedY = 300.0f;
    
    // Bully Awareness Month Pink
    const Color BullyAwarenessPink = { 255, 105, 180, 255 }; // Hot Pink
}

struct Paddle {
    float x, y;
    float width, height;
    float speed;
    int score;

    void Draw() const {
        DrawRectangleRec({ x, y, width, height }, Constants::BullyAwarenessPink);
    }

    void Update(bool isLeft) {
        if (isLeft) {
            if (IsKeyDown(KEY_W)) y -= speed * GetFrameTime();
            if (IsKeyDown(KEY_S)) y += speed * GetFrameTime();
        } else {
            if (IsKeyDown(KEY_UP)) y -= speed * GetFrameTime();
            if (IsKeyDown(KEY_DOWN)) y += speed * GetFrameTime();
        }

        // Keep paddle on screen
        if (y < 0) y = 0;
        if (y > Constants::ScreenHeight - height) y = Constants::ScreenHeight - height;
    }
};

struct Ball {
    float x, y;
    float speedX, speedY;
    float radius;

    void Draw() const {
        DrawCircle(static_cast<int>(x), static_cast<int>(y), radius, Constants::BullyAwarenessPink);
    }

    void Update() {
        x += speedX * GetFrameTime();
        y += speedY * GetFrameTime();

        // Bounce off top and bottom with clamping to prevent getting stuck
        if (y - radius < 0) {
            y = radius;
            speedY *= -1;
        } else if (y + radius > Constants::ScreenHeight) {
            y = Constants::ScreenHeight - radius;
            speedY *= -1;
        }
    }

    void Reset() {
        x = Constants::ScreenWidth / 2.0f;
        y = Constants::ScreenHeight / 2.0f;
        speedX = (speedX > 0) ? -Constants::BallSpeedX : Constants::BallSpeedX; // Serve to the other player
    }
};

int main() {
    // Initialization
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(Constants::ScreenWidth, Constants::ScreenHeight, "PingPong - Bully Awareness Month");

    Paddle leftPaddle{ 
        .x = 20, 
        .y = Constants::ScreenHeight / 2.0f - Constants::PaddleHeight / 2.0f, 
        .width = Constants::PaddleWidth, 
        .height = Constants::PaddleHeight, 
        .speed = Constants::PaddleSpeed, 
        .score = 0 
    };
    
    Paddle rightPaddle{ 
        .x = Constants::ScreenWidth - 20 - Constants::PaddleWidth, 
        .y = Constants::ScreenHeight / 2.0f - Constants::PaddleHeight / 2.0f, 
        .width = Constants::PaddleWidth, 
        .height = Constants::PaddleHeight, 
        .speed = Constants::PaddleSpeed, 
        .score = 0 
    };
    
    Ball ball{ 
        .x = Constants::ScreenWidth / 2.0f, 
        .y = Constants::ScreenHeight / 2.0f, 
        .speedX = Constants::BallSpeedX, 
        .speedY = Constants::BallSpeedY, 
        .radius = Constants::BallRadius 
    };

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        leftPaddle.Update(true);
        rightPaddle.Update(false);
        ball.Update();

        // Collisions with paddles - with repositioning to prevent tunneling
        if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { leftPaddle.x, leftPaddle.y, leftPaddle.width, leftPaddle.height })) {
            if (ball.speedX < 0) {
                ball.speedX *= -1.1f; // Increase speed slightly
                ball.x = leftPaddle.x + leftPaddle.width + ball.radius; // Push out of paddle
                ball.speedY = (ball.y - (leftPaddle.y + leftPaddle.height / 2)) / (leftPaddle.height / 2) * Constants::BallSpeedY;
            }
        }

        if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { rightPaddle.x, rightPaddle.y, rightPaddle.width, rightPaddle.height })) {
            if (ball.speedX > 0) {
                ball.speedX *= -1.1f; // Increase speed slightly
                ball.x = rightPaddle.x - ball.radius; // Push out of paddle
                ball.speedY = (ball.y - (rightPaddle.y + rightPaddle.height / 2)) / (rightPaddle.height / 2) * Constants::BallSpeedY;
            }
        }

        // Scoring
        if (ball.x < 0) {
            rightPaddle.score++;
            ball.Reset();
        }
        if (ball.x > Constants::ScreenWidth) {
            leftPaddle.score++;
            ball.Reset();
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw middle line
        for (int i = 0; i < Constants::ScreenHeight; i += 20) {
            DrawRectangle(Constants::ScreenWidth / 2 - 1, i, 2, 10, GRAY);
        }

        leftPaddle.Draw();
        rightPaddle.Draw();
        ball.Draw();

        // Draw scores using C++20 std::format
        DrawText(std::format("{}", leftPaddle.score).c_str(), Constants::ScreenWidth / 4, 20, 40, WHITE);
        DrawText(std::format("{}", rightPaddle.score).c_str(), 3 * Constants::ScreenWidth / 4, 20, 40, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
