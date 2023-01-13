#include "headers/Game.h"

int main(void)
{
	Game game(800,600, "Break-Out");
	while (!WindowShouldClose())
	{
		game.UpdateGame();
		game.DrawGame();
	}

	return 0;
}
