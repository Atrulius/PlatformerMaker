#include <iostream>
#include "Game.h"

Game game;

int main()
{

	while (game.running()) {

		game.Update();

		game.Draw();
	}

	return 0;
}