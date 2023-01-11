#include "headers/Game.h"

int main(void)
{
	std::unique_ptr<Game>Game01 = std::make_unique<Game>(800,600, "Break-Out");
	while (!WindowShouldClose())
	{
		Game01->UpdateDrawGame();
	}
	
	return 0;
}
