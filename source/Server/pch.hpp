#ifndef _PCH_HPP_
#define _PCH_HPP_

#pragma region HEADER
// C++ 기본 헤더
#pragma region C++
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#pragma endregion

// STL Container
#pragma region CONTAINER
#include <array>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#pragma endregion

// 멀티쓰레드 프로그래밍 헤더
#pragma region MULTITHREADING
#include <thread>
#include <atomic>
#include <shared_mutex>
#pragma endregion

// 멀티쓰레드 프로그래밍 Container
#pragma region PPL
//#include <concurrent_vector.h>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#pragma endregion

// 비동기 네트워크 통신 헤더
#pragma region NETWORK
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.hpp"
#pragma endregion

#pragma region DIRECT3D
#include <DirectXMath.h>
#pragma endregion

// Lua 스크립트 언어 헤더
#pragma region SCRIPT
#include"include/lua.hpp"
#pragma endregion
#pragma endregion

#pragma region LIB
// 비동기 네트워크 통신 라이브러리
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

// Lua 스크립트 언어 라이브러리
#pragma comment(lib, "lua54")
#pragma endregion

//#define DEBUG_GAME

#pragma region USING
using namespace DirectX;
using c_set = concurrency::concurrent_unordered_set<int>;
using POS = float;
using VECTOR = float;
#pragma endregion

extern std::default_random_engine dre;

// 최대 동접 인원
#pragma region CONSTANT
inline constexpr int MAX_USER{ 10 };
inline constexpr int NPC_NUM{ 200 };
inline constexpr int SIGHT_RANGE{ 200 };
inline constexpr int SECTOR_RANGE{ 400 };

#define NPC_START MAX_USER
#pragma endregion

#pragma region FUNCTION
void ErrorQuit(std::wstring_view msg);
void DisplayError(std::wstring_view msg);

inline bool IsInSight(int dis1, int dis2)
{
	if (SIGHT_RANGE < std::abs(dis1))
		return false;
	if (SIGHT_RANGE < std::abs(dis2))
		return false;

	return true;
}

inline int LuaInt(lua_State* L, int pos)
{
	return static_cast<int>(lua_tonumber(L, pos));
}
inline const char* LuaString(lua_State* L, int pos)
{
	return static_cast<const char*>(lua_tostring(L, pos));
}
#pragma endregion

namespace DXMATH
{
	inline void AddVector(XMFLOAT3* vector1, XMFLOAT3* vector2)
	{
		XMStoreFloat3(vector1, XMLoadFloat3(vector1) + (XMLoadFloat3(vector2)));
	}

	inline void AddVector(XMFLOAT3* vector1, XMFLOAT3* vector2, float scalar)
	{
		XMStoreFloat3(vector1, XMLoadFloat3(vector1) + (XMLoadFloat3(vector2) * scalar));
	}

	inline void TransformNormal(XMFLOAT3* vector, XMMATRIX* matrix)
	{
		XMStoreFloat3(vector, XMVector3TransformNormal(XMLoadFloat3(vector), *matrix));
	}

	inline void Normalize(XMFLOAT3* vector)
	{
		XMStoreFloat3(vector, XMVector3Normalize(XMLoadFloat3(vector)));
	}

	inline void CrossNormalize(XMFLOAT3* dest, XMFLOAT3* vector1, XMFLOAT3* vector2)
	{
		XMStoreFloat3(dest, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(vector1), XMLoadFloat3(vector2))));
	}
}

#endif // !_PCH_HPP_
