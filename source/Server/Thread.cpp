#include "pch.h"

DWORD WINAPI AcceptThread(LPVOID arg)
{

}

DWORD WINAPI WorkerThread(LPVOID arg)
{
	std::unique_ptr<SOCKET> sock{ reinterpret_cast<SOCKET*>(arg) };


}