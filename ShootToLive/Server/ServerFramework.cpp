#include "ServerFramework.h"

ExOverlapped::ExOverlapped(OPERATION op, char size, char* msg) : oper(op)
{
	ZeroMemory(&WSAover, sizeof(WSAover));
	WSAbuf.buf = sendMsg;
	WSAbuf.len = size;
	memcpy(sendMsg, msg, size);
}

void Client::Send(int size, char* msg)
{
	std::unique_ptr<ExOverlapped> exOver{ std::make_unique<ExOverlapped>(static_cast<OPERATION>(OPERATION::SEND), size, msg) };

	WSASend(cSocket, &exOver->WSAbuf, 1, 0, 0, &exOver->WSAover, nullptr);
}

void Client::Recv()
{
	DWORD flag{ 0 };

	ZeroMemory(&recvOver.WSAover, sizeof(recvOver.WSAover));
	recvOver.WSAbuf.buf = recvOver.sendMsg + prevSize;
	recvOver.WSAbuf.len = sizeof(recvOver.WSAbuf.buf) - prevSize;
	WSARecv(cSocket, &recvOver.WSAbuf, 1, 0, &flag, &recvOver.WSAover, nullptr);
}

void CServerFramework::Initialize()
{
	if (WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
		return;

	lSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	if (lSocket == INVALID_SOCKET)
		ErrorQuit(L"WSASocket()");

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(lSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
		ErrorQuit(L"bind()");

	if (listen(lSocket, SOMAXCONN) == SOCKET_ERROR)
		ErrorQuit(L"listen()");

	HANDLE hIocp{ CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 0) };
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(lSocket), hIocp, 0, 0);

	SOCKET cSocket{ WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED) };
	char acceptBuf[sizeof(sockaddr_in) * 2 + 32 + 100];
	ExOverlapped acceptEx;

	AcceptEx(lSocket, cSocket, acceptBuf, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, nullptr, &acceptEx.WSAover);

	while (true)
	{
		GetQueuedCompletionStatus(hIocp, &size, &client, &wsaover, INFINITE);
	}
}

void CServerFramework::ErrorQuit(std::wstring msg)
{
	std::wstring msgBuf;

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgBuf.data(), 0, nullptr);

	MessageBox(nullptr, msgBuf.c_str(), msg.c_str(), MB_ICONERROR);

	LocalFree(msgBuf.data());
	exit(1);
}

void CServerFramework::ErrorDisplay(std::wstring msg, int ErrorNum)
{
	std::wstring msgBuf;

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ErrorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgBuf.data(), 0, nullptr);

	std::wcout << msg << L" 에러" << msgBuf << std::endl;

	LocalFree(msgBuf.data());
}
