#include "NetworkServer.h"

Client::Client() : sock(INVALID_SOCKET)
{
	ZeroMemory(&sendOver, sizeof(sendOver));
	ZeroMemory(&recvOver, sizeof(recvOver));
}

CNetworkServer::CNetworkServer() : hIocp(INVALID_HANDLE_VALUE), workerRun(true), acceptRun(true)
{
}

CNetworkServer::~CNetworkServer()
{
	WSACleanup();
}

bool CNetworkServer::Initialize()
{
	if (WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
		ErrorQuit(L"WSAStartup()");

	hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 0);

	if (hIocp == INVALID_HANDLE_VALUE)
		ErrorQuit(L"CreateIoCompletionPort(IOCP kernel)");

	lSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	if (lSocket == INVALID_SOCKET)
		ErrorQuit(L"WSASocket(listen socket)");

	if (BOOL option{ TRUE }; setsockopt(lSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&option), sizeof(option)) == SOCKET_ERROR)
		ErrorQuit(L"setsockopt(SO_REUSEADDR)");

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(lSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
		ErrorQuit(L"bind(listen socket)");

	if (listen(lSocket, SOMAXCONN) == SOCKET_ERROR)
		ErrorQuit(L"listen()");

	return true;
}

void CNetworkServer::Run()
{
	clients.reserve(100);

	for (int i = 0; i < 100; ++i)
	{
		clients.emplace_back();
	}

	workerThreads.reserve(4);

	for (int i = 0; i < 4; ++i)
	{
		workerThreads.emplace([this]() {WorkerThread(); });
	}

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(lSocket), hIocp, 0, 0);

	SOCKET cSocket{ WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED) };
	char acceptBuf[sizeof(sockaddr_in) * 2 + 32 + 100];
	ExOverlapped acceptEx;

	AcceptEx(lSocket, cSocket, acceptBuf, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, nullptr, &acceptEx.WSAover);



}

void CNetworkServer::OnSend(Client* client, char* message, int length)
{
	DWORD recvByte;


}

void CNetworkServer::OnReceive()
{
}

void CNetworkServer::OnDisconnect()
{
}

void CNetworkServer::WorkerThread()
{
	Client*			 client{ nullptr };			// CompletionKey를 받을 포인터
	DWORD			 ioSize;					// Overlapped I/O 작업에서 전송된 데이터 크기
	LPOVERLAPPED	 lpOverlapped{ nullptr };	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터

	while (workerRun)
	{
		GetQueuedCompletionStatus(hIocp, &ioSize, reinterpret_cast<PULONG_PTR>(&client), &lpOverlapped, INFINITE);

		ExOverlapped* exOver{ reinterpret_cast<ExOverlapped*>(lpOverlapped) };

		if (exOver->oper == OPERATION::RECV)
		{
			exOver->msg[ioSize] = NULL;
			std::cout << "[수신] : " << ioSize << " 바이트, " << exOver->msg << std::endl;

			SendMsg(client, exOver->msg, ioSize);
			
		}
	}
}

void CNetworkServer::AcceptThread()
{
}

void CNetworkServer::ErrorQuit(std::wstring msg)
{
	std::wstring msgBuf;

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgBuf.data(), 0, nullptr);

	MessageBox(nullptr, msgBuf.c_str(), msg.c_str(), MB_ICONERROR);

	LocalFree(msgBuf.data());
	exit(1);
}

void CNetworkServer::ErrorDisplay(std::wstring msg, int ErrorNum)
{
	std::wstring msgBuf;

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ErrorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgBuf.data(), 0, nullptr);

	std::wcout << msg << L" 에러" << msgBuf << std::endl;

	LocalFree(msgBuf.data());
}

bool CNetworkServer::CreateWorkerThread()
{


	return false;
}

bool CNetworkServer::CreateAcceptThread()
{
	return false;
}
