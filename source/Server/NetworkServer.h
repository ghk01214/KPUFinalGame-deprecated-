#ifndef _NETWORK_SERVER_H_
#define _NETWORK_SERVER_H_

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

DWORD WINAPI AcceptThread(LPVOID arg);
DWORD WINAPI WorkerThread(LPVOID arg);

class ExOverlapped
{
public:
	ExOverlapped(OPERATION op = static_cast<OPERATION>(OPERATION::RECV)) : oper(op) {}
	ExOverlapped(OPERATION op, char bytes, char* msg);

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

	void Send(int size, char* msg);
	void Recv();

private:
	ExOverlapped	 recvOver;
	SOCKET			 cSocket;
	int				 prevSize;

	int				 id;
	short			 x, y;
};

class CNetworkServer
{
public:
	CNetworkServer() {}
	virtual ~CNetworkServer() {}

	bool Initialize();
	bool Release();

	void Run();

	virtual void OnAccept() = 0;
	virtual void OnReceive() = 0;
	virtual void OnSend() = 0;
	virtual void OnDisconnect() = 0;

	void ErrorQuit(std::wstring msg);
	void ErrorDisplay(std::wstring msg, int ErrorNum);

	friend DWORD WINAPI AcceptThread(LPVOID arg);
	friend DWORD WINAPI WorkerThread(LPVOID arg);

private:
	HANDLE hIocp;
	SOCKET lSocket;
	sockaddr_in serverAddr;

	Client client;
};

#endif // !_NETWORK_SERVER_H_