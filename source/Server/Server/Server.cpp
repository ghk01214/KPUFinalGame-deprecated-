#include "pch.hpp"
#include "NetworkFramework.hpp"

int main()
{
	CNetworkFramework network;

	network.OnCreate();
	network.CreateThread();
	network.OnDestroy();
}