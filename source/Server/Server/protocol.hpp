#ifndef _PROTOCOL_HPP_
#define _PROTOCOL_HPP_

namespace DIRECTION
{
	inline constexpr int FORWARD{ 0x01 };
	inline constexpr int BACKWARD{ 0x02 };
	inline constexpr int LEFT{ 0x04 };
	inline constexpr int RIGHT{ 0x08 };
	inline constexpr int UP{ 0x10 };
	inline constexpr int DOWN{ 0x20 };
	inline constexpr int LCLICK{ 0x40 };
}

namespace ATTACK_MODE
{
	inline constexpr int NONE{ 0 };
	inline constexpr int SHOT{ 1 };
	inline constexpr int BURST{ 2 };
}

inline constexpr int SERVER_PORT{ 4000 };

namespace VAR_SIZE
{
	inline constexpr int DATA{ 256 };
	inline constexpr int NAME{ 10 };
	inline constexpr float WORLD_X{ 257 * 9.0f };			// 257 = terrain pixel size, 9 = scale size
	inline constexpr float WORLD_Z{ 257 * 9.0f };			// 257 = terrain pixel size, 9 = scale size
}

#pragma pack(push, 1)

namespace SC
{
	inline constexpr char LOGIN{ 0 };
	inline constexpr char ADD_PLAYER{ 1 };
	inline constexpr char REMOVE_PLAYER{ 2 };
	inline constexpr char MOVE_PLAYER{ 3 };
	inline constexpr char PLAYER_ATTACK{ 5 };

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

		struct PLAYER_ATTACK
		{
			unsigned char size{ sizeof(SC::PACKET::PLAYER_ATTACK) };
			char type{ SC::PLAYER_ATTACK };
			unsigned short id;

		};
	}
}

namespace CS
{
	inline constexpr char LOGIN{ 6 };
	inline constexpr char MOVE_PLAYER{ 7 };
	inline constexpr char ROTATE_PLAYER{ 8 };
	inline constexpr char PLAYER_ATTACK{ 9 };

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
		struct PLAYER_ATTACK
		{
			unsigned char size{ sizeof(CS::PACKET::PLAYER_ATTACK) };
			char type{ CS::PLAYER_ATTACK };
			char mode;
		};
	}
}

#pragma pack(pop)

#endif // !_PROTOCOL_HPP_
