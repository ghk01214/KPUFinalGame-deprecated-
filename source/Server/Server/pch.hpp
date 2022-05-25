#ifndef _PCH_HPP_
#define _PCH_HPP_

#define DEBUG_GAME

// C++ 기본 헤더
#pragma region C++
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#pragma endregion

// STL Container
#pragma region STL_CONTAINER
#include <string>
#include <array>
#include <vector>
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
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#pragma endregion

// 비동기 네트워크 통신 헤더
#pragma region NETWORK
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma endregion

#pragma region DIRECT3D
#include <DirectXMath.h>
#pragma endregion

// Lua 스크립트 언어 헤더
#include"include/lua.hpp"

#pragma region LIBRARY
// 비동기 네트워크 통신 라이브러리
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

// Lua 스크립트 언어 라이브러리
#pragma comment(lib, "lua54")
#pragma endregion

using namespace DirectX;

extern std::default_random_engine dre;

#pragma region DEFINE
using POS = float;
using VECTOR = float;
#pragma endregion

// 최대 동접 인원
#pragma region CONSTANT
inline constexpr int MAX_USER{ 10 };
inline constexpr int NPC_NUM{ 200000 };
inline constexpr int SIGHT_RANGE{ 200 };
inline constexpr int SECTOR_RANGE{ 400 };
#pragma endregion

#pragma region FUNCTION
void ErrorQuit(std::wstring_view msg);
void DisplayError(std::wstring_view msg);
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

	inline void TransformNormal(XMFLOAT3* vector, XMMATRIX& matrix)
	{
		XMStoreFloat3(vector, XMVector3TransformNormal(XMLoadFloat3(vector), matrix));
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
