#ifndef _NETWORK_SERVER_H_
#define _NETWORK_SERVER_H_

#include "pch.h"
#include "packets.h"

enum class OPERATION : int
{
	SEND,
	RECV,
	ACCEPT
};

constexpr int SERVER_PORT{ 4000 };
constexpr int BUF_SIZE{ 256 };

//DWORD WINAPI AcceptThread(LPVOID arg);
//DWORD WINAPI WorkerThread(LPVOID arg);

// OVERLAPPED 구조체를 확장시킨 구조체
struct ExOverlapped
{
	OVERLAPPED		 WSAover;				// 확장된 Overlapped 구조체
	SOCKET			 sock;					// 클라이언트 소켓
	OPERATION		 oper;					// 작업 동작 종류
	WSABUF			 WSAbuf;				// Overlapped I/O 작업 버퍼
	char			 msg[BUF_SIZE];			// 데이터 버퍼
};

// 클라이언트 정보를 담은 구조체
class Client
{
public:
	Client();
	~Client() { closesocket(sock); }

	ExOverlapped	 sendOver;
	ExOverlapped	 recvOver;
	SOCKET			 sock;
};

class CNetworkServer
{
public:
	CNetworkServer();
	virtual ~CNetworkServer();

	bool Initialize();
	bool Release();

	void Run();

	virtual void OnAccept() = 0;
	virtual void OnSend(Client* client, char* message, int length);
	virtual void OnReceive();
	virtual void OnDisconnect();

	void WorkerThread();
	void AcceptThread();

	void ErrorQuit(std::wstring msg);
	void ErrorDisplay(std::wstring msg, int ErrorNum);

	//friend DWORD WINAPI AcceptThread(LPVOID arg);
	//friend DWORD WINAPI WorkerThread(LPVOID arg);
	bool CreateWorkerThread();
	bool CreateAcceptThread();


private:
	HANDLE hIocp;
	SOCKET lSocket;
	sockaddr_in serverAddr;

	std::vector<Client> clients;
	std::vector<std::thread> workerThreads;
	std::thread acceptThread;

	bool workerRun;
	bool acceptRun;
};

#endif // !_NETWORK_SERVER_H_