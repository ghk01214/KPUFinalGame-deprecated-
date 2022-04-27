#include "pch.h"
#include "Network.h"
#include "../../Server/Server/protocol.hpp"

CNetwork::CNetwork() : server(INVALID_SOCKET), remain_size(0)
{
	ZeroMemory(&recv_over, sizeof(recv_over));
	ZeroMemory(&send_over, sizeof(send_over));
}

CNetwork::~CNetwork()
{
}

void CNetwork::CreateNetworkSocket()
{
	WSADATA wsa;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
	{
		ErrorQuit(L"WSAStartup fuction error", WSAGetLastError());
	}

	server = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	InetPton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	WSAConnect(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);
}

void CNetwork::RecvData(SOCKET sock)
{
	DWORD flag{ 0 };
	ZeroMemory(&recv_over, sizeof(recv_over));

	recv_over.wsa_buf.buf = recv_over.data + remain_size;
	recv_over.wsa_buf.len = VAR_SIZE::DATA - remain_size;

	WSARecv(server, &recv_over.wsa_buf, 1, 0, &flag, &recv_over.over, nullptr);
}

void CNetwork::SendData(void* packet)
{

}

OVERLAPPEDEX::OVERLAPPEDEX()
{
}

void OVERLAPPEDEX::SetPacket(char* packet)
{
}
