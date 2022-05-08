#include "pch.hpp"
#include "NetworkFramework.hpp"

int main()
{
	CNetworkFramework network;

	network.BootServer();
	network.CreateThread();
}