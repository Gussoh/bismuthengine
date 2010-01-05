#include "stdafx.h"
#include "DemoApplication.h"

int main(int, char**) {
	std::cout << "Bismuth Engine Demo 1.0" << std::endl;

	std::cout << "Server? (y/N) ";
	char isServer;
	std::cin >> isServer;

	DemoApplication *app;

	if (isServer == 'y') {
		std::cout << std::endl << "Number of players: ";
		int numberOfPlayers;
		std::cin >> numberOfPlayers;
		std::cout << std::endl;
		app = new DemoApplication(true, "", numberOfPlayers);
	} else {
		std::cout << std::endl << "Server host: ";
		std::string host;
		std::cin >> host;
		std::cout << std::endl;
		app = new DemoApplication(false, host);
	}

	app->run();

	delete app;
	
	return 0;
}
