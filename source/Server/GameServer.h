#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include "NetworkServer.h"

class CGameServer : public CNetworkServer
{
public:
	CGameServer() {}
	virtual ~CGameServer() = 0;

private:

};

#endif // !_GAME_SERVER_H_