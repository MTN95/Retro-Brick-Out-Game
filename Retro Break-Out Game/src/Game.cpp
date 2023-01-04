#include "headers/Game.h"

Game::Game(int width, int height, std::string title)
	: screenWidth(width), screenHeight(height), title(title)
{
	InitWindow(screenWidth, screenHeight, title.c_str());

	InitAudioDevice();

	// only updates as fast as the user's refresh rate
	SetWindowState(FLAG_VSYNC_HINT);

	SetTargetFPS(60);
	// initialize game variables

	this->pause = false;
	this->gameOver = false;


	// Initialize player
	GamePaddle.position = Vector2{ screenWidth * 0.5f, screenHeight * 7.f / 8 };
	GamePaddle.size = Vector2{ screenWidth / 8.f, 20 };
	GamePaddle.speed = 5;
	GamePaddle.life = 5;

	// Initialize ball
	GameBall.position = Vector2{ screenWidth * 0.5f, screenHeight * 7.f / 8 - 30 };
	GameBall.speed = Vector2{ 0, 0 };
	GameBall.radius = 7;
	GameBall.isActive = false;

}

Game::~Game()
{
	CloseAudioDevice();
	CloseWindow();
}

void Game::UpdateGame()
{
	// --------------------------------------------------------------------------------------------------------------
		// Update Game
		// --------------------------------------------------------------------------------------------------------------
	const int rightWallPos = GetScreenWidth() - static_cast<int>(GamePaddle.size.x) / 2;
	const int leftWallPos = 0 + static_cast<int>(GamePaddle.size.x) / 2;

	if (!gameOver)
	{
		if (!pause)
		{
			// Pause the game at any time
			if (IsKeyPressed(KEY_P)) pause = !pause;

			// Player Paddle movement and collision
			if (IsKeyDown(KEY_RIGHT))
				GamePaddle.position.x += GamePaddle.speed;
			if (IsKeyDown(KEY_LEFT))
				GamePaddle.position.x -= GamePaddle.speed;


			if (GamePaddle.position.x >= rightWallPos) GamePaddle.position.x = static_cast<float>(rightWallPos);

			if (GamePaddle.position.x <= leftWallPos) GamePaddle.position.x = static_cast<float>(leftWallPos);

			// Launching the ball 
			if (!GameBall.isActive)
			{
				if (IsKeyPressed(KEY_SPACE))
				{
					GameBall.isActive = true;
					GameBall.speed = { 0, -5.f };
				}
			}

			if (GameBall.isActive)
			{
				GameBall.position.x += GameBall.speed.x;
				GameBall.position.y += GameBall.speed.y;
			}
			else
			{
				GameBall.position = Vector2{ GamePaddle.position.x, screenHeight * 7.f / 8 - 30 };
			}

			// Ball Collision with walls
			if ((GameBall.position.x >= screenWidth - GameBall.radius) || (GameBall.position.x <= GameBall.radius))
				GameBall.speed.x *= -1;

			if (GameBall.position.y >= screenHeight - GameBall.radius && !gameOver)
			{
				GameBall.isActive = false;
				--GamePaddle.life;
				if (GamePaddle.life == 0) gameOver = true;
			}

			if (GameBall.position.y <= GameBall.radius) GameBall.speed.y *= -1;


			// Collision logic: ball vs player
			if (CheckCollisionCircleRec(GameBall.position, GameBall.radius,
				Rectangle{
				GamePaddle.position.x - GamePaddle.size.x * 0.5f, GamePaddle.position.y - GamePaddle.size.y * 0.5f, GamePaddle.size.x, GamePaddle.size.y
				}))
			{
				if (GameBall.speed.y > 0)
				{
					GameBall.speed.y *= -1.1f;
					GameBall.speed.x = (GameBall.position.x - GamePaddle.position.x) / (GamePaddle.size.x * 0.5f) * 5.f;
				}
			}
		}
	}
	else
	{
		int textWidth = MeasureText("Game Over!, press enter to play again", 20);
		DrawText("Game Over!, press enter to play again", screenMidX - textWidth, screenMidY - 100, 40, BLACK);

		// re initialize the game
		if (IsKeyPressed(KEY_ENTER))
		{
			GameBall.isActive = true;
			GamePaddle.life = 6;
			gameOver = false;
		}
	}

}

void Game::DrawGame()
{

	BeginDrawing();
	ClearBackground(WHITE);
	DrawFPS(10, 10);

	// Draw player paddle
	DrawRectangle(static_cast<int>(GamePaddle.position.x) - static_cast<int>(GamePaddle.size.x) / 2, static_cast<int>(GamePaddle.position.y) -
		static_cast<int>(GamePaddle.size.y) / 2, static_cast<int>(GamePaddle.size.x), static_cast<int>(GamePaddle.size.y), GamePaddle.color);

	// Draw ball
	DrawCircleV(GameBall.position, GameBall.radius, MAROON);

	// Draw player life bars
	for (int i = 0; i < GamePaddle.life; i++) DrawRectangle(i * screenWidth / 20 + screenWidth - 215, screenHeight - 30, 35, 10, MAROON);


	if (pause)
	{
		int textWidth = MeasureText("PAUSED", 30);
		DrawText("PAUSED", screenMidX - textWidth, screenMidY - 100, 60, BLACK);

	}

	EndDrawing();
}

void Game::UpdateDrawGame()
{
	UpdateGame();
	DrawGame();
}

void Game::UnloadGame()
{
	// unload assets
}
