#include "headers/Game.h"
#include <algorithm>


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
	GamePaddle->position = Vector2{ screenWidth * 0.5f, screenHeight * 7.f / 8 };
	GamePaddle->size = Vector2{ screenWidth / 8.f, 20 };
	GamePaddle->speed = 5;
	GamePaddle->life = 5;

	// Initialize ball
	GameBall->position = Vector2{ screenWidth * 0.5f, screenHeight * 7.f / 8 - 30 };
	GameBall->speed = Vector2{ 0, 0 };
	GameBall->radius = 7;
	GameBall->isActive = false;


	// Initialize Bricks
	const int BricksRowNumber = 12;
	const int BricksColNumber = 12;
	Vector2 BrickInitPos = { GetScreenWidth() * 0.1f, GetScreenHeight() * 0.1f };

	for (int i = 0; i < BricksRowNumber; i++) 
	{
		for (int j = 0; j < BricksColNumber; j++)
		{
			auto brick = std::make_unique<aBrick>();
			brick->color = DARKGREEN;
			brick->isAlive = true;
			brick->size = { 50, 15 };
			Vector2 spacing = aBrick::CalculateSpacing(brick->size);
			brick->position = Vector2{ BrickInitPos.x + spacing.x * i, BrickInitPos.y + spacing.y * j };
			GameBricks.emplace_back(std::move(brick));
		}
	}
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
	const int rightWallPos = GetScreenWidth() - GamePaddle->size.x * 0.5f;
	const int leftWallPos = 0 + GamePaddle->size.x * 0.5f;

	if (!gameOver)
	{
		if (!pause)
		{
			// Pause the game at any time
			if (IsKeyPressed(KEY_P)) pause = !pause;

			// Player Paddle movement and collision
			if (IsKeyDown(KEY_RIGHT))
				GamePaddle->position.x += GamePaddle->speed;
			if (IsKeyDown(KEY_LEFT))
				GamePaddle->position.x -= GamePaddle->speed;
			
			if (GamePaddle->position.x >= rightWallPos) GamePaddle->position.x = static_cast<float>(rightWallPos);
			
			if (GamePaddle->position.x <= leftWallPos) GamePaddle->position.x = static_cast<float>(leftWallPos);
			
			// Launching the ball 
			if (!GameBall->isActive)
			{
				if (IsKeyPressed(KEY_SPACE))
				{
					GameBall->isActive = true;
					GameBall->speed = { 0, -5.f };
				}
			}

			if (GameBall->isActive)
			{
				GameBall->position.x += GameBall->speed.x;
				GameBall->position.y += GameBall->speed.y;
			}
			else
			{
				GameBall->position = Vector2{ GamePaddle->position.x, screenHeight * 7.f / 8 - 30 };
			}
			
			// Ball Collision with walls
			if ((GameBall->position.x >= screenWidth - GameBall->radius) || (GameBall->position.x <= GameBall->radius))
				GameBall->speed.x *= -1;

			if (GameBall->position.y >= screenHeight - GameBall->radius && !gameOver)
			{
				GameBall->isActive = false;
				--GamePaddle->life;
				if (GamePaddle->life == 0) gameOver = true;
			}

			if (GameBall->position.y <= GameBall->radius) GameBall->speed.y *= -1;

			// Collision logic: ball vs player
			if (CheckCollisionCircleRec(GameBall->position, GameBall->radius,
				Rectangle{
				GamePaddle->position.x - GamePaddle->size.x * 0.5f, GamePaddle->position.y - GamePaddle->size.y * 0.5f,
				GamePaddle->size.x, GamePaddle->size.y
				}))
			{
				if (GameBall->position.y > 0)
				{
					GameBall->speed.y *= -1.0f;
					GameBall->speed.x = (GameBall->position.x - GamePaddle->position.x) / (GamePaddle->size.x * 0.5f) * 5.f;
				}
			}

			// Bricks collision with ball
			for (auto& brick : GameBricks)
			{
				if (brick->isAlive)
				{
					// Check if ball is inside the bounds of the brick
					if (CheckCollisionCircleRec(GameBall->position, GameBall->radius,
						Rectangle{ brick->position.x, brick->position.y, brick->size.x, brick->size.y }))
					{
						// Collision detected!
						brick->isAlive = false;
						
						// Change ball's velocity
						GameBall->speed.y *= -1;

					}
				}
			}

			if (GameBricks.empty())
			{
				gameOver = true;
			}

			if (GamePaddle->life == 0) gameOver = true;

			if (std::all_of(GameBricks.begin(), GameBricks.end(), [](const auto& brick) { return !brick->isAlive; }))
			{
				// all bricks are not alive, end the game
				gameOver = true;
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
			for (auto& brick : GameBricks)
				brick->isAlive = true;

			// Initialize player
			GamePaddle->position = Vector2{ screenWidth * 0.5f, screenHeight * 7.f / 8 };
			GamePaddle->size = Vector2{ screenWidth / 8.f, 20 };
			GamePaddle->speed = 5;
			GamePaddle->life = 5;

			// Initialize ball
			GameBall->position = Vector2{ screenWidth * 0.5f, screenHeight * 7.f / 8 - 30 };
			GameBall->speed = Vector2{ 0, 0 };
			GameBall->radius = 7;
			GameBall->isActive = false;

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
		DrawRectangle(GamePaddle->position.x - GamePaddle->size.x * 0.5f, GamePaddle->position.y -
			GamePaddle->size.y * 0.5f, GamePaddle->size.x, GamePaddle->size.y, GamePaddle->color);

		// Draw ball
		DrawCircleV(GameBall->position, GameBall->radius, MAROON);

		// Draw Brick
		for (auto& brick : GameBricks)
		{
			if (brick->isAlive) 
			{
				DrawRectangle( brick->position.x, brick->position.y, brick->size.x, brick->size.y, brick->color);
			}
		}
		

		// Draw player life bars
		for (int i = 0; i < GamePaddle->life; i++) DrawRectangle(i * screenWidth / 20 + screenWidth - 215, screenHeight - 30, 35, 10, MAROON);

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
