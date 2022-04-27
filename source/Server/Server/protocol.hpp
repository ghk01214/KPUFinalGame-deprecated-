#ifndef _PROTOCOL_HPP_
#define _PROTOCOL_HPP_

enum class DIRECTION
{
	UP = 1, DOWN, LEFT, RIGHT
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
			unsigned char size;
			char type;
			unsigned short id;
			short x, y;
		};

		struct ADD_PLAYER
		{
			unsigned char size;
			char type;
			unsigned short id;
			short x, y;
			char name[VAR_SIZE::NAME];
		};

		struct REMOVE_PLAYER
		{
			unsigned char size;
			char type;
			unsigned short id;
		};

		struct MOVE_PLAYER
		{
			unsigned char size;
			char type;
			unsigned short id;
			short x, y;
		};
	}
}

namespace CS
{
	inline constexpr char LOGIN{ 4 };
	inline constexpr char MOVE{ 5 };

	namespace PACKET
	{
		struct LOGIN
		{
			unsigned char size;
			char type;
			char name[VAR_SIZE::NAME];
		};

		struct MOVE
		{
			unsigned char size;
			char type;
			char direction;			// 1 : UP, 2 : DOWN, 3 : LEFT, 4 : RIGHT
		};
	}
}

#pragma pack(pop)

#endif // !_PROTOCOL_HPP_
