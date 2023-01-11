#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <raylib.h>
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"


class Game
{
public:

	Game(int width, int height, std::string title);
	
	~Game();

	void UpdateGame();
	
	void DrawGame();
	
	void UpdateDrawGame();
	
	void UnloadGame(); // for cleaning custom assets, sound, textures etc..

	void setTitle(std::string& newTitle) { title = newTitle; }
	
	bool isGamePaused() { return pause; }
	bool isGameOver() { return gameOver; }

private:
	int screenWidth = 0;
	int screenHeight = 0;
	const int screenMidX = screenWidth / 2;
	const int  screenMidY = screenHeight / 2;
	
	std::string title = " ";

	bool pause = false;
	bool gameOver = false;

	std::unique_ptr<aBall> GameBall = std::make_unique<aBall>();
	std::unique_ptr<aPaddle> GamePaddle = std::make_unique<aPaddle>();
	std::vector<std::unique_ptr<aBrick>> GameBricks;
	
};