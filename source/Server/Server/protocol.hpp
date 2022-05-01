#ifndef _PROTOCOL_HPP_
#define _PROTOCOL_HPP_

enum class DIRECTION
{
	FORWARD = 1, BACKWARD, LEFT, RIGHT, UP, DOWN
};

inline constexpr int SERVER_PORT{ 4000 };

namespace VAR_SIZE
{
	inline constexpr int DATA{ 256 };
	inline constexpr int NAME{ 10 };
	inline constexpr int WORLD_WIDTH{ 8 };
	inline constexpr int WORLD_HEIGHT{ 8 };
}

#pragma pack(push, 1)

namespace SC
{
	inline constexpr char LOGIN{ 0 };
	inline constexpr char ADD_PLAYER{ 1 };
	inline constexpr char REMOVE_PLAYER{ 2 };
	inline constexpr char MOVE_PLAYER{ 3 };

	namespace PACKET
	{
		struct LOGIN
		{
			unsigned char size{ sizeof(SC::PACKET::LOGIN) };
			char type{ SC::LOGIN };
			unsigned short id;
			short x, y, z;
		};

		struct ADD_PLAYER
		{
			unsigned char size{ sizeof(SC::PACKET::ADD_PLAYER) };
			char type{ SC::ADD_PLAYER };
			unsigned short id;
			short x, y, z;
			char name[VAR_SIZE::NAME];
		};

		struct REMOVE_PLAYER
		{
			unsigned char size{ sizeof(SC::PACKET::REMOVE_PLAYER) };
			char type{ SC::REMOVE_PLAYER };
			unsigned short id;
		};

		struct MOVE_PLAYER
		{
			unsigned char size{ sizeof(SC::PACKET::MOVE_PLAYER) };
			char type{ SC::MOVE_PLAYER };
			unsigned short id;
			short x, y, z;
		};
	}
}

namespace CS
{
	inline constexpr char LOGIN{ 4 };
	inline constexpr char MOVE_PLAYER{ 5 };

	namespace PACKET
	{
		struct LOGIN
		{
			unsigned char size{ sizeof(CS::PACKET::LOGIN) };
			char type{ CS::LOGIN };
			char name[VAR_SIZE::NAME];
		};

		struct MOVE_PLAYER
		{
			unsigned char size{ sizeof(CS::PACKET::MOVE_PLAYER) };
			char type{ CS::MOVE_PLAYER };
			char direction;			// 1 : FORWARD, 2 : BACKWARD, 3 : LEFT, 4 : RIGHT, 5 : UP, 6 : DOWN
		};
	}
}

#pragma pack(pop)

#endif // !_PROTOCOL_HPP_
