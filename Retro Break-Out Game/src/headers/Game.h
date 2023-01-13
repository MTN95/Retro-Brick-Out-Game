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

	bool isGamePaused() const { return pause; }
	bool isGameOver() const { return gameOver; }

	void initGame();

private:
	int screenWidth = 0;
	int screenHeight = 0;
	const int screenMidX = screenWidth / 2;
	const int  screenMidY = screenHeight / 2;
	
	std::string title = " ";

	bool pause = false;
	bool gameOver = false;
	
	Ball GameBall;
	Paddle GamePaddle;
	std::vector<std::vector<Brick>> GameBricks;

	const int LINES_OF_BRICKS = 12;
	const int BRICKS_PER_LINE = 8;
	
};