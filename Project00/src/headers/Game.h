#pragma once
#include <iostream>
#include <string>
#include <raylib.h>
#include "Ball.h"
#include "Paddle.h"


class Game
{
public:

	Game(int width, int height, std::string title);
	
	~Game();

	void UpdateGame();
	
	void DrawGame();
	
	void UpdateDrawGame();
	
	void UnloadGame();


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

	aBall GameBall = { 0 };
	aPaddle GamePaddle = { 0 };

};