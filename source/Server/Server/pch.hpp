#ifndef _PCH_HPP_
#define _PCH_HPP_

// C++ 기본 헤더
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>

// STL Container
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <unordered_map>

// 멀티쓰레드 프로그래밍 헤더
#include <thread>
#include <atomic>
#include <mutex>

// 비동기 네트워크 통신 헤더
#include <WS2tcpip.h>
#include <MSWSock.h>

// 비동기 네트워크 통신 라이브러리 링킹
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

extern std::default_random_engine dre;

void ErrorQuit(std::wstring_view msg, int errorNum);
void DisplayError(std::wstring_view msg, int errorNum);

#endif // !_PCH_HPP_
