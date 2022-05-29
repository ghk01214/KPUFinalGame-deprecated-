#ifndef _PROTOCOL_HPP_
#define _PROTOCOL_HPP_

using SIZE_ = unsigned char;
using TYPE_ = char;
using ID_ = unsigned int;
using POS_ = float;
using VECTOR_ = float;

namespace KEYINPUT
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

inline constexpr int SERVER_PORT{ 5000 };

namespace VAR
{
	inline constexpr int DATA{ 256 };
	inline constexpr int NAME{ 10 };
	inline constexpr float WORLD_X{ 2000 };			// 257 = terrain pixel size, 9 = scale size
	inline constexpr float WORLD_Z{ 2000 };			// 257 = terrain pixel size, 9 = scale size
}

#pragma pack(push, 1)

namespace SC
{
	inline constexpr TYPE_ LOGIN{ 1 };
	inline constexpr TYPE_ ADD_OBJECT{ 2 };
	inline constexpr TYPE_ REMOVE_OBJECT{ 3 };
	inline constexpr TYPE_ MOVE_OBJECT{ 4 };
	inline constexpr TYPE_ ROTATE_OBJECT{ 5 };
	inline constexpr TYPE_ PLAYER_ATTACK{ 6 };
	inline constexpr TYPE_ STRESS_MOVE{ 7 };

	namespace PACKET
	{
		struct LOGIN
		{
			SIZE_ size;
			TYPE_ type;
			ID_ id;
			POS_ x, y, z;
		};

		struct ADD_OBJECT
		{
			SIZE_ size;
			TYPE_ type{ SC::ADD_OBJECT };
			ID_ id;
			POS_ x, y, z;
			char name[VAR::NAME];
			VECTOR_ look_x, look_y, look_z;			// Look Vector
			VECTOR_ right_x, right_y, right_z;		// Right Vector
			VECTOR_ up_x, up_y, up_z;				// Up Vector
		};

		struct REMOVE_OBJECT
		{
			SIZE_ size;
			TYPE_ type;
			ID_ id;
		};

		struct MOVE_OBJECT
		{
			SIZE_ size;
			TYPE_ type;
			ID_ id;
			POS_ x, y, z;
			unsigned int client_time;
		};

		struct ROTATE_OBJECT
		{
			SIZE_ size;
			TYPE_ type;
			ID_ id;
			VECTOR_ look_x, look_y, look_z;			// Look Vector
			VECTOR_ right_x, right_y, right_z;		// Right Vector
			VECTOR_ up_x, up_y, up_z;				// Up Vector
		};

		struct PLAYER_ATTACK
		{
			SIZE_ size;
			TYPE_ type;
			ID_ id;

		};

		struct STRESS_MOVE
		{
			SIZE_ size;
			TYPE_ type;
			ID_ id;
			POS_ x, y;
			unsigned int client_time;
		};
	}
}

#define CS_TYPE_START SC::STRESS_MOVE + 1

namespace CS
{
	inline constexpr TYPE_ LOGIN{ CS_TYPE_START };
	inline constexpr TYPE_ MOVE_OBJECT{ CS_TYPE_START + 1 };
	inline constexpr TYPE_ ROTATE_OBJECT{ CS_TYPE_START + 2 };
	inline constexpr TYPE_ PLAYER_ATTACK{ CS_TYPE_START + 3 };

	namespace PACKET
	{
		struct LOGIN
		{
			SIZE_ size;
			TYPE_ type;
			char name[VAR::NAME];
			VECTOR_ look_x, look_y, look_z;			// Look Vector
			VECTOR_ right_x, right_y, right_z;		// Right Vector
			VECTOR_ up_x, up_y, up_z;				// Up Vector
			float pitch, yaw;
		};

		struct MOVE_OBJECT
		{
			SIZE_ size;
			TYPE_ type;
			char direction;				// 0x01 : FORWARD, 0x02 : BACKWARD, 0x04 : LEFT, 0x08 : RIGHT, 0x10 : UP, 0x20 : DOWN
			unsigned int client_time;
		};

		struct ROTATE_OBJECT
		{
			SIZE_ size;
			TYPE_ type;
			float cx, cy;
		};

		struct PLAYER_ATTACK
		{
			SIZE_ size;
			TYPE_ type;
			char mode;
		};
	}
}

#pragma pack(pop)

#endif // !_PROTOCOL_HPP_
