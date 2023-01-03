#include "headers/Game.h"

int main(void)
{
	Game game01(800, 600, "Game");

	while (!WindowShouldClose())
	{
		game01.UpdateDrawGame();
	}
	
	return 0;
}
