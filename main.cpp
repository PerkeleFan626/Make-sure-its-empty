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
    Playing,
    GameOver
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

    GameState currentState = GameState::Menu;
    GameMode currentMode = GameMode::PvP;
    int highScore = 0;
    std::string winnerName = "";
    std::string loserMessage = "";

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
                leftPaddle.score = 0;
                rightPaddle.score = 0;
            }
            if (IsKeyPressed(KEY_TWO)) {
                currentMode = GameMode::PvAI;
                currentState = GameState::Playing;
                leftPaddle.score = 0;
                rightPaddle.score = 0;
            }

            // Menu Draw
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("PING PONG", Constants::ScreenWidth / 2 - MeasureText("PING PONG", 60) / 2, 60, 60, Constants::BullyAwarenessPink);
            DrawText("1. Player vs Player", Constants::ScreenWidth / 2 - MeasureText("1. Player vs Player", 20) / 2, 180, 20, WHITE);
            DrawText("2. Player vs Computer", Constants::ScreenWidth / 2 - MeasureText("2. Player vs Computer", 20) / 2, 220, 20, WHITE);
            
            if (highScore > 0) {
                DrawText(std::format("High Score (vs AI): {}", highScore).c_str(), Constants::ScreenWidth / 2 - MeasureText(std::format("High Score (vs AI): {}", highScore).c_str(), 20) / 2, 280, 20, YELLOW);
            }

            DrawText("First to 10 points wins!", Constants::ScreenWidth / 2 - MeasureText("First to 10 points wins!", 15) / 2, 330, 15, GRAY);
            DrawText("Press 1 or 2 to start", Constants::ScreenWidth / 2 - MeasureText("Press 1 or 2 to start", 15) / 2, 380, 15, GRAY);

            EndDrawing();
        } else if (currentState == GameState::Playing) {
            // Playing Logic
            leftPaddle.Update(true, false, ball.y);
            rightPaddle.Update(false, (currentMode == GameMode::PvAI), ball.y);
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
                if (rightPaddle.score >= 10) {
                    currentState = GameState::GameOver;
                    winnerName = (currentMode == GameMode::PvAI) ? "Computer" : "Player 2";
                    loserMessage = (currentMode == GameMode::PvAI) ? "You are a loser!" : "Player 1 is a loser!";
                } else {
                    ball.Reset();
                }
            }
            if (ball.x > Constants::ScreenWidth) {
                leftPaddle.score++;
                if (leftPaddle.score >= 10) {
                    currentState = GameState::GameOver;
                    winnerName = "Player 1";
                    loserMessage = (currentMode == GameMode::PvAI) ? "Computer is a loser!" : "Player 2 is a loser!";
                    if (currentMode == GameMode::PvAI && leftPaddle.score > highScore) {
                        highScore = leftPaddle.score;
                    }
                } else {
                    ball.Reset();
                }
            }

            // Return to menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = GameState::Menu;
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
        } else if (currentState == GameState::GameOver) {
            // GameOver Logic
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) {
                currentState = GameState::Menu;
                ball.Reset();
            }

            // GameOver Draw
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("GAME OVER", Constants::ScreenWidth / 2 - MeasureText("GAME OVER", 50) / 2, 100, 50, RED);
            DrawText(std::format("{} Wins!", winnerName).c_str(), Constants::ScreenWidth / 2 - MeasureText(std::format("{} Wins!", winnerName).c_str(), 30) / 2, 180, 30, WHITE);
            DrawText(loserMessage.c_str(), Constants::ScreenWidth / 2 - MeasureText(loserMessage.c_str(), 20) / 2, 240, 20, Constants::BullyAwarenessPink);
            
            DrawText("Press ENTER to return to Menu", Constants::ScreenWidth / 2 - MeasureText("Press ENTER to return to Menu", 15) / 2, 350, 15, GRAY);

            EndDrawing();
        }
    }

    CloseWindow();

    return 0;
}
