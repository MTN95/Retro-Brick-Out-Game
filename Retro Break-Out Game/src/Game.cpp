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
	initGame();


}

Game::~Game()
{
	CloseAudioDevice();
	CloseWindow();
}

void Game::UpdateGame()
{
	
	const float rightWallPos = GetScreenWidth() - GamePaddle.sizeX * 0.5f;
	const float leftWallPos = 0 + GamePaddle.sizeX * 0.5f;

	if (!gameOver)
	{
		if (!pause)
		{
			// Pause the game at any time
			if (IsKeyPressed(KEY_P))
			{
				pause = !pause;
			}

			// Player Paddle movement and collision
			if (IsKeyDown(KEY_RIGHT))
				GamePaddle.positionX += GamePaddle.speed;
			if (IsKeyDown(KEY_LEFT))
				GamePaddle.positionX -= GamePaddle.speed;
			
			if (GamePaddle.positionX >= rightWallPos) GamePaddle.positionX = rightWallPos;
			
			if (GamePaddle.positionX <= leftWallPos) GamePaddle.positionX = leftWallPos;
			
			// Launching the ball 
			if (!GameBall.isActive)
			{
				GameBall.positionX = GamePaddle.positionX;
				GameBall.positionY = screenHeight * 7.f / 8 - 30;

				if (IsKeyPressed(KEY_SPACE))
				{
					GameBall.isActive = true;
					GameBall.speedX = 0;
					GameBall.speedY =  -5.f;
				}
			}
			else
			{
				GameBall.positionX += GameBall.speedX;
				GameBall.positionY += GameBall.speedY;
			}

		
			// Ball Collision with walls
			if ((GameBall.positionX >= screenWidth - GameBall.radius) || (GameBall.positionX <= GameBall.radius))
				GameBall.speedX *= -1;

			if (GameBall.positionY >= screenHeight - GameBall.radius)
			{
				GameBall.isActive = false;
				--GamePaddle.life;
				if (GamePaddle.life == 0)
				{
					gameOver = true;
				}
			}

			if (GameBall.positionY <= GameBall.radius) GameBall.speedY *= -1;

			// Collision logic: ball vs player
			if (CheckCollisionCircleRec({ GameBall.positionX,GameBall.positionY }, GameBall.radius,
				Rectangle{
				GamePaddle.positionX - GamePaddle.sizeX * 0.5f, GamePaddle.positionY - GamePaddle.sizeY * 0.5f,
				GamePaddle.sizeX, GamePaddle.sizeY
				}))
			{
				if (GameBall.positionY > 0)
				{
					GameBall.speedY *= -1.0f;
					GameBall.speedX = (GameBall.positionX - GamePaddle.positionX) / (GamePaddle.sizeX * 0.5f) * 5.f;
				}
			}

			// Bricks collision with ball
			for (auto& bricks : GameBricks)
			{
				for (auto& brick : bricks)
				{
					if (brick.isAlive)
					{
						// Check if ball is inside the bounds of the brick
						if (CheckCollisionCircleRec({ GameBall.positionX,GameBall.positionY }, GameBall.radius,
							Rectangle{ static_cast<float>(brick.positionX), static_cast<float>(brick.positionY), static_cast<float>(brick.sizeX), static_cast<float>(brick.sizeY) }))
						{
							// Collision detected!
							brick.isAlive = false;
						
							
							if (GameBall.positionY + GameBall.radius > brick.positionY + brick.sizeY * 0.5f) // bottom
							{
								GameBall.speedY *= -1;
							}
							else if (GameBall.positionY - GameBall.radius < brick.positionY - brick.sizeY * 0.5f) // top
							{
								GameBall.speedY *= -1;
							}

							// Check if ball hit left or right of brick
							else if (GameBall.positionX + GameBall.radius > brick.positionX + brick.sizeX * 0.5f) // right
							{
								GameBall.speedX *= -1;
							}
							else if (GameBall.positionX - GameBall.radius < brick.positionX - brick.sizeX * 0.5f) // left 
							{
								GameBall.speedX *= -1;
							}
						}
					}
				}
			}


			if (GameBricks.empty())
			{
				gameOver = true;
			}

			if (GamePaddle.life == 0)
			{
				gameOver = true;
			}

			if (std::none_of(GameBricks.begin(), GameBricks.end(), [](const auto& row) 
				{ return std::any_of(row.begin(), row.end(), [](const auto& brick) { return brick.isAlive; }); })) 
			{
				// no brick is alive, end the game
				gameOver = true;
			}

		}
	}
	else
	{
		GameOverScreen();
	}
}

void Game::DrawGame() const
{

	BeginDrawing();
		ClearBackground(WHITE);
		DrawFPS(10, screenHeight - 20);
		
		//DrawRectangleRec()
		Rectangle paddleRec= { GamePaddle.positionX - GamePaddle.sizeX * 0.5f, GamePaddle.positionY -
			GamePaddle.sizeY * 0.5f, GamePaddle.sizeX, GamePaddle.sizeY };

		// Draw player paddle
		DrawRectangleRec(paddleRec, GamePaddle.color);

		// Draw ball
		DrawCircleV({GameBall.positionX,GameBall.positionY}, GameBall.radius, GameBall.color);

		// Draw Brick
		for (int i = 0; i < LINES_OF_BRICKS; i++)
		{
			for (int j = 0; j < BRICKS_PER_LINE; j++)
			{
				if (GameBricks[i][j].isAlive)
				{
					DrawRectangleRec( 
						{ GameBricks[i][j].positionX,GameBricks[i][j].positionY, GameBricks[i][j].sizeX, GameBricks[i][j].sizeY},
						GameBricks[i][j].color);
				}
			}
		}	

		// Draw player life bars
		for (int i = 0; i < GamePaddle.life; i++) DrawRectangle(i * screenWidth / 20 + screenWidth - 215, screenHeight - 30, 35, 10, MAROON);

		if (pause)
		{
			int textWidth = MeasureText("PAUSED", 30);
			DrawText("PAUSED", screenMidX - textWidth, screenMidY - 100, 60, BLACK);

		}

	EndDrawing();
}

void Game::initGame()
{

	// Initialize player
	GamePaddle.positionX = screenWidth * 0.5f;
	GamePaddle.positionY = screenHeight * 7.f / 8;
	GamePaddle.sizeX = screenWidth / 8.f;
	GamePaddle.sizeY = 20;
	GamePaddle.speed = 5;
	GamePaddle.life = 1;

	// Initialize ball
	GameBall.positionX = screenWidth * 0.5f;
	GameBall.positionY = screenHeight * 7.f / 8 - 30;
	GameBall.speedX = 0;
	GameBall.speedY = 0;
	GameBall.radius = 7;
	GameBall.isActive = false;

	// Initialize Bricks
	Vector2 BrickInitPos = { GetScreenWidth()/ LINES_OF_BRICKS, GetScreenHeight() / BRICKS_PER_LINE };

	GameBricks.resize(LINES_OF_BRICKS, std::vector<Brick>(BRICKS_PER_LINE));

	for (int i = 0; i < LINES_OF_BRICKS; i++)
	{
		for (int j = 0; j < BRICKS_PER_LINE; j++)
		{

			Brick brick;
			brick.color = DARKGREEN;
			brick.isAlive = true;
			brick.sizeX = 50;
			brick.sizeY = 20;
			brick.positionX = BrickInitPos.x + (brick.sizeX + 6) * i;
			brick.positionY = BrickInitPos.y + (brick.sizeY + 6) * j;
			GameBricks[i][j] = brick;
		}
	}
}

void Game::GameOverScreen()
{
	int textWidth = MeasureText("Game Over!, press enter to play again", 20);
	DrawText("Game Over!, press enter to play again", screenMidX - textWidth, screenMidY - 300, 40, BLACK);

	// //reinitialize the game
	//if (IsKeyPressed(KEY_ENTER))
	//{
	//	for (auto& bricks : GameBricks)
	//	{
	//		for (auto& brick : bricks)
	//		{
	//			brick.isAlive = true;
	//		}
	//	}

	//	initGame();	
	//	pause = false;
	//	gameOver = false;

	//}
}

