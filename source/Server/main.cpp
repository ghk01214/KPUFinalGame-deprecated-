#include "pch.hpp"
#include "GameServer.hpp"

int main()
{
	std::unique_ptr<GameServer> game_server{ std::make_unique<GameServer>() };

	game_server->Run();
}