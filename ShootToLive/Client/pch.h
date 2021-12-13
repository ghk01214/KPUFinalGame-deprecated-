// pch.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#ifndef _PCH_H_
#define _PCH_H_

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <assert.h>
#include <wrl.h>
#include <process.h>
#include <float.h>
// C++ 런타임 헤더 파일입니다.
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

// Direct3D 12 런터임 헤더 파일입니다.
#include <d3d12.h>
#include <d2d1_3.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <d3d11on12.h>
#include <dwrite_3.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#endif // !_PCH_H_