#ifndef _IOCP_NETWORK_SERVER_H_
#define _IOCP_NETWORK_SERVER_H_

#include "pch.h"
#include "packets.h"

enum class OPERATION
{
	SEND,
	RECV,
	ACCEPT
};

constexpr int SERVER_PORT{ 4000 };
constexpr int BUF_SIZE{ 256 };

class ExOverlapped
{
public:
	ExOverlapped(OPERATION op = static_cast<OPERATION>(OPERATION::RECV)) : oper(op) {}
	ExOverlapped(OPERATION op, char bytes, char* msg);
public:
	OVERLAPPED		 WSAover;				// 확장된 Overlapped 구조체
	OPERATION		 oper;					// send/recv/accept 구별
	WSABUF			 WSAbuf;
	char			 sendMsg[BUF_SIZE];		// IOCP send/recv 버퍼
};

class Client
{
public:
	Client() : x(0), y(0), prevSize(0) {}
	Client(int id, SOCKET s) : x(0), y(0), prevSize(0), id(id), cSocket(s) {}
	~Client() { closesocket(cSocket); }
public:
	void Send(int size, char* msg);
	void Recv();
private:
	ExOverlapped	 recvOver;
	SOCKET			 cSocket;
	int				 prevSize;
private:
	int				 id;
	short			 x, y;
};

class CIOCPNetworkServer
{
public:
	CIOCPNetworkServer() {}
	~CIOCPNetworkServer() {}
public:
	bool Initialize();
	void Run();
	void ErrorQuit(std::wstring msg);
	void ErrorDisplay(std::wstring msg, int ErrorNum);
public:
	DWORD WINAPI WorkerThread(LPVOID arg);
private:
	SOCKET lSocket;
	sockaddr_in serverAddr;
private:
	Client client;
};

#endif // !_IOCP_NETWORK_SERVER_H_