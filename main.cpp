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

enum class GameState {
    Menu,
    Playing
};

enum class GameMode {
    PvP,
    PvAI
};

struct Paddle {
    float x, y;
    float width, height;
    float speed;
    int score;

    void Draw() const {
        DrawRectangleRec({ x, y, width, height }, Constants::BullyAwarenessPink);
    }

    void Update(bool isLeft, bool isAI, float ballY) {
        if (isLeft) {
            if (IsKeyDown(KEY_W)) y -= speed * GetFrameTime();
            if (IsKeyDown(KEY_S)) y += speed * GetFrameTime();
        } else {
            if (isAI) {
                // Simple AI: try to center the paddle with the ball
                float paddleCenter = y + height / 2.0f;
                if (paddleCenter < ballY - 10) y += speed * 0.8f * GetFrameTime(); // Slightly slower AI
                else if (paddleCenter > ballY + 10) y -= speed * 0.8f * GetFrameTime();
            } else {
                if (IsKeyDown(KEY_UP)) y -= speed * GetFrameTime();
                if (IsKeyDown(KEY_DOWN)) y += speed * GetFrameTime();
            }
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

        // Bounce off top and bottom
        if (y - radius < 0 || y + radius > Constants::ScreenHeight) {
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

    GameState currentState = GameState::Menu;
    GameMode currentMode = GameMode::PvP;

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
        if (currentState == GameState::Menu) {
            // Menu Logic
            if (IsKeyPressed(KEY_ONE)) {
                currentMode = GameMode::PvP;
                currentState = GameState::Playing;
            }
            if (IsKeyPressed(KEY_TWO)) {
                currentMode = GameMode::PvAI;
                currentState = GameState::Playing;
            }

            // Menu Draw
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("PING PONG", Constants::ScreenWidth / 2 - MeasureText("PING PONG", 60) / 2, 80, 60, Constants::BullyAwarenessPink);
            DrawText("1. Player vs Player", Constants::ScreenWidth / 2 - MeasureText("1. Player vs Player", 20) / 2, 220, 20, WHITE);
            DrawText("2. Player vs Computer", Constants::ScreenWidth / 2 - MeasureText("2. Player vs Computer", 20) / 2, 260, 20, WHITE);
            DrawText("Press 1 or 2 to start", Constants::ScreenWidth / 2 - MeasureText("Press 1 or 2 to start", 15) / 2, 350, 15, GRAY);

            EndDrawing();
        } else {
            // Playing Logic
            leftPaddle.Update(true, false, ball.y);
            rightPaddle.Update(false, (currentMode == GameMode::PvAI), ball.y);
            ball.Update();

            // Collisions with paddles
            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { leftPaddle.x, leftPaddle.y, leftPaddle.width, leftPaddle.height })) {
                if (ball.speedX < 0) {
                    ball.speedX *= -1.1f; // Increase speed slightly
                    ball.speedY = (ball.y - (leftPaddle.y + leftPaddle.height / 2)) / (leftPaddle.height / 2) * Constants::BallSpeedY;
                }
            }

            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { rightPaddle.x, rightPaddle.y, rightPaddle.width, rightPaddle.height })) {
                if (ball.speedX > 0) {
                    ball.speedX *= -1.1f; // Increase speed slightly
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

            // Return to menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = GameState::Menu;
                leftPaddle.score = 0;
                rightPaddle.score = 0;
                ball.Reset();
            }

            // Playing Draw
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

            DrawText("ESC to Menu", 10, Constants::ScreenHeight - 20, 15, DARKGRAY);

            EndDrawing();
        }
    }

    CloseWindow();

    return 0;
}
