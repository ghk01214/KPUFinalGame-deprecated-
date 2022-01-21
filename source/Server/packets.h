#ifndef _PACKETS_H_
#define _PACKETS_H_

enum class CONSTANT : char
{
	MAX_NAME_SIZE = 20,
	MAX_USER = 4
};

constexpr char CS_PACKET_LOGIN{ 1 };
constexpr char CS_PACKET_MOVE{ 2 };

constexpr char SC_PACKET_LOGIN_OK{ 1 };
constexpr char SC_PACKET_MOVE{ 2 };
constexpr char SC_PACKET_PUT_OBJECT{ 3 };
constexpr char SC_PACKET_REMOVE_OBJECT{ 4 };

#pragma pack(push, 1)
namespace SCPacket
{
	struct LoginOK					// 로그인 확인 packet
	{
		unsigned char	 size;		// packet의 크기
		char			 type;		// packet의 종류
		unsigned int	 id;		// 플레이어 id
		short			 x, y;		// 플레이어 위치
		unsigned short	 hp;		// 플레이어 hp
		unsigned short	 level;		// 플레이어 레벨
		unsigned int	 exp;		// 플레이어 경험치
	};

	struct Move						// 플레이어 이동 packet
	{
		unsigned char	 size;		// packet의 크기
		char			 type;		// packet의 종류
		unsigned int	 id;		// 플레이어 id
		short			 x, y;		// 플레이어 이동 위치
	};

	struct Enter					// 플레이어 입장 packet
	{
		unsigned char	 size;		// packet의 크기 
		char			 type;		// packet의 종류 
		unsigned int	 id;		// 플레이어 id 
		char			 name[static_cast<int>(CONSTANT::MAX_NAME_SIZE)];
		char			 objectType;// 
		short			 x, y;		// 플레이어 위치
	};

	struct PutObject				// object 생성 packet
	{
		unsigned char	 size;
		char			 type;
		unsigned int	 id;
		short			 x, y;
		char			 objectType;
		char			 name[static_cast<int>(CONSTANT::MAX_NAME_SIZE)];
	};

	struct RemoveObject			// object 제거
	{
		unsigned char	 size;
		char			 type;
		unsigned int	 id;
	};
}

namespace CSPacket
{
	struct Login
	{
		char		 size;
		char		 type;
		char		 name[static_cast<int>(CONSTANT::MAX_NAME_SIZE)];
	};

	struct Move
	{
		char size;
		char type;
		char direction;		// 0 : up, 1 : down, 2 : left, 3 : right
	};
}
#pragma pack(pop)

#endif // !_PACKETS_H_