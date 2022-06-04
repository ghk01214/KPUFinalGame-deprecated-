#pragma once

#pragma region TYPEDEF
using SIZE_ = unsigned char;
using TYPE = char;
using ID = unsigned int;
using POS = float;
using VECTOR = float;
using DIRECTION = unsigned char;
#pragma endregion

inline constexpr int SERVER_PORT{ 5000 };

namespace KEY
{
	inline constexpr int FORWARD{ 0x01 };
	inline constexpr int BACKWARD{ 0x02 };
	inline constexpr int LEFT{ 0x04 };
	inline constexpr int RIGHT{ 0x08 };
	inline constexpr int UP{ 0x10 };
	inline constexpr int DOWN{ 0x20 };
	inline constexpr int LCLICK{ 0x40 };
}

namespace VAR
{
	inline constexpr int DATA{ 200 };			// 한 패킷에 들어갈 수 있는 최대 데이터 크기
	inline constexpr int NAME{ 10 };			// 최대 이름 길이
	inline constexpr float WORLD_XL{ 0 };		// 월드의 왼쪽 끝
	inline constexpr float WORLD_XR{ 2000 };	// 월드의 오른쪽 끝
	inline constexpr float WORLD_ZF{ 2000 };	// 월드의 앞쪽 끝
	inline constexpr float WORLD_ZB{ 0 };		// 월드의 뒤쪽 끝
}

#pragma pack(push, 1)
namespace SC
{
	inline constexpr TYPE LOGIN{ 1 };
	inline constexpr TYPE ADD_OBJ{ 2 };
	inline constexpr TYPE DELETE_OBJ{ 3 };
	inline constexpr TYPE MOVE_OBJ{ 4 };
	inline constexpr TYPE ROTATE_OBJ{ 5 };
	inline constexpr TYPE PLAYER_ATTACK{ 6 };
	//inline constexpr TYPE STRESS_MOVE{ 7 };

	namespace P		// P = PACKET
	{
		struct LOGIN
		{
			SIZE_ size;
			TYPE type;
			ID id;
			POS x, y, z;
		};

		struct ADD_OBJ
		{
			SIZE_ size;
			TYPE type;
			ID id;
			POS x, y, z;
			char name[VAR::NAME];
			VECTOR look_x, look_y, look_z;			// Look Vector
			VECTOR right_x, right_y, right_z;		// Right Vector
			VECTOR up_x, up_y, up_z;				// Up Vector
		};

		struct DELETE_OBJ
		{
			SIZE_ size;
			TYPE type;
			ID id;
		};

		struct MOVE_OBJ
		{
			SIZE_ size;
			TYPE type;
			ID id;
			POS x, y, z;
			unsigned int client_time;
		};

		struct ROTATE_OBJ
		{
			SIZE_ size;
			TYPE type;
			ID id;
			VECTOR look_x, look_y, look_z;			// Look Vector
			VECTOR right_x, right_y, right_z;		// Right Vector
			VECTOR up_x, up_y, up_z;				// Up Vector
		};

		struct PLAYER_ATTACK
		{
			SIZE_ size;
			TYPE type;
			ID id;
		};
	}
}

#define CS_TYPE_START SC::PLAYER_ATTACK + 1

namespace CS
{
	inline constexpr TYPE LOGIN{ CS_TYPE_START };
	inline constexpr TYPE MOVE_OBJ{ CS_TYPE_START + 1 };
	inline constexpr TYPE ROTATE_OBJ{ CS_TYPE_START + 2 };
	inline constexpr TYPE PLAYER_ATTACK{ CS_TYPE_START + 3 };

	namespace P		// P = PACKET
	{
		struct LOGIN
		{
			SIZE_ size;
			TYPE type;
			char name[VAR::NAME];
			VECTOR look_x, look_y, look_z;			// Look Vector
			VECTOR right_x, right_y, right_z;		// Right Vector
			VECTOR up_x, up_y, up_z;				// Up Vector
			float pitch, yaw;
		};

		struct MOVE_OBJ
		{
			SIZE_ size;
			TYPE type;
			DIRECTION direction;
		};

		struct ROTATE_OBJ
		{
			SIZE_ size;
			TYPE type;
			float cx, cy;
		};

		struct PLAYER_ATTACK
		{
			SIZE_ size;
			TYPE type;
			char mode;
		};
	}
}
#pragma pack(pop)