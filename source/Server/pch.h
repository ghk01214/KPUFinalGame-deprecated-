#pragma once

#pragma region HEADER
#pragma region C++
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#pragma endregion

#pragma region CONTAINER
#include <array>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#pragma endregion

#pragma region MULTITHREADING
#include <thread>
#include <atomic>
#include <shared_mutex>
#pragma endregion

#pragma region PPL
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#pragma endregion

#pragma region NETWORK
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.h"
#pragma endregion

#pragma region LUA
#include "include/lua.hpp"
#pragma endregion

#pragma region DIRECTX
#include <DirectXMath.h>
#pragma endregion
#pragma endregion

#pragma region LIB
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

#pragma comment(lib, "lua54")
#pragma endregion

// 디버깅용 헤더
#ifndef DEBUG
#define DEBUG
#endif // !DEBUGGING

#pragma region USING
using namespace DirectX;
using c_set = concurrency::concurrent_unordered_set<ID>;
#pragma endregion

extern std::default_random_engine dre;

#pragma region CONSTANT
inline constexpr int SIGHT_RANGE{ 1000 };
inline constexpr int SECTOR_RANGE{ 500 };

// NPC 시작 인덱스
inline constexpr int NPC_START{ VAR::MAX_USER };
inline constexpr int NPC_1{ NPC_START + 50 };
inline constexpr int NPC_2{ NPC_1 + 25 };
inline constexpr int NPC_3{ NPC_2 + 15 };
inline constexpr int NPC_4{ NPC_3 + 10 };
#pragma endregion

void ErrorQuit(std::wstring_view msg);
void DisplayError(std::wstring_view msg);

inline bool IsInSight(float dis1, float dis2)
{
	//if (SIGHT_RANGE < std::abs(dis1))
	//	return false;
	//if (SIGHT_RANGE < std::abs(dis2))
	//	return false;

	//return true;
	return std::abs(dis1) + std::abs(dis2) <= SIGHT_RANGE;
}

namespace LUA
{
	inline int LuaInt(lua_State* L, int pos)
	{
		return static_cast<int>(lua_tonumber(L, pos));
	}

	inline std::string LuaString(lua_State* L, int pos)
	{
		return static_cast<const char*>(lua_tostring(L, pos));
	}
}

namespace DXMATH
{
	inline void AddVector(XMFLOAT3* vec1, XMFLOAT3* vec2)
	{
		XMStoreFloat3(vec1, XMLoadFloat3(vec1) + (XMLoadFloat3(vec2)));
	}

	inline void AddVector(XMFLOAT3* vec1, XMFLOAT3* vec2, float scalar)
	{
		XMStoreFloat3(vec1, XMLoadFloat3(vec1) + (XMLoadFloat3(vec2) * scalar));
	}

	inline void TransformNormal(XMFLOAT3* vec, XMMATRIX* matrix)
	{
		XMStoreFloat3(vec, XMVector3TransformNormal(XMLoadFloat3(vec), *matrix));
	}

	inline void Normalize(XMFLOAT3* vec)
	{
		XMStoreFloat3(vec, XMVector3Normalize(XMLoadFloat3(vec)));
	}

	inline void CrossNormalize(XMFLOAT3* dest, XMFLOAT3* vec1, XMFLOAT3* vec2)
	{
		XMStoreFloat3(dest, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(vec1), XMLoadFloat3(vec2))));
	}
}