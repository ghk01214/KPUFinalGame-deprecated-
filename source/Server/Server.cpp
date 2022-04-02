// Server.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "Server.h"

#define MAX_LOADSTRING 100

enum class WndSize
{
	WIDTH = 800,
	HEIGHT = 600
};

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HWND connectionLog;	// 연결 로그 컨트롤
HWND hClient1, hClient2, hClient3, hClient4, hEnd;	// 클라이언트 버튼 및 종료 버튼
HINSTANCE cliInst1, cliInst2, cliInst3, cliInst4;	// 각 클라이언트 별 인스턴스

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

	HACCEL hAccelTable{ LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDC_SERVER)) };

	MSG msg{};

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
			DispatchMessageW(&msg);
        }
    }

	//DialogBox(hInstance, MAKEINTRESOURCEW(IDD_MASTER), nullptr, DlgProc);

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_SERVER));
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SERVER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	// 화면의 해상도에 따라서 윈도우의 크기 및 위치 결정
	HWND hWnd{ CreateWindowW(szWindowClass, szTitle, WS_SYSMENU | WS_MINIMIZEBOX,
		(GetSystemMetrics(SM_CXSCREEN) - (int)WndSize::WIDTH * 0.75) / 2, (GetSystemMetrics(SM_CYSCREEN) - (int)WndSize::HEIGHT * 0.75) / 2,
		(int)WndSize::WIDTH * 0.75, (int)WndSize::HEIGHT * 0.75, nullptr, nullptr, hInstance, nullptr) };

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);

		// 연결 로그를 출력하는 에디트 컨트롤 자식 윈도우 생성
		connectionLog = CreateWindowW(L"edit", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, rc.right - 110, rc.bottom, hWnd, (HMENU)IDC_CONNECTION, hInst, nullptr);

		// 각 클라이언트 로그 및 종료 버튼 자식 윈도우 생성
		hClient1 = CreateWindowW(L"button", L"Client 1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			rc.right - 105, rc.top + 10, 100, 30, hWnd, (HMENU)IDC_CLIENT1, hInst, nullptr);
		hClient2 = CreateWindowW(L"button", L"Client 2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			rc.right - 105, rc.top + 50, 100, 30, hWnd, (HMENU)IDC_CLIENT2, hInst, nullptr);
		hClient3 = CreateWindowW(L"button", L"Client 3", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			rc.right - 105, rc.top + 90, 100, 30, hWnd, (HMENU)IDC_CLIENT3, hInst, nullptr);
		hClient4 = CreateWindowW(L"button", L"Client 4", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			rc.right - 105, rc.top + 130, 100, 30, hWnd, (HMENU)IDC_CLIENT4, hInst, nullptr);

		hEnd = CreateWindowW(L"button", L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
			rc.right - 105, rc.bottom - 40, 100, 30, hWnd, (HMENU)IDC_EXIT, hInst, nullptr);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
		switch (LOWORD(wParam))
            {
		case IDC_CLIENT1:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT1), hWnd, DlgProc), SW_SHOW);
			break;
		case IDC_CLIENT2:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT2), hWnd, DlgProc), SW_SHOW);
			break;
		case IDC_CLIENT3:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT3), hWnd, DlgProc), SW_SHOW);
			break;
		case IDC_CLIENT4:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT4), hWnd, DlgProc), SW_SHOW);
                break;
		case IDC_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
		HDC hdc{ BeginPaint(hWnd, &ps) };
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		//SendMessage(connectionLog, 0, 0, TEXT("TextOut이다. ㅜㅜ 우리 텍스트 아웃되버렸지만 수고했어."), 34);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
	{
		//SendMessage(connectionLog, EM_SETLIMITTEXT, 512, 0);
	}
        return (INT_PTR)TRUE;

    case WM_COMMAND:
	{
		// 메뉴 선택을 구문 분석합니다:
		switch (LOWORD(wParam))
        {
		case IDC_CLIENT1:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT1), hDlg, DlgProc), SW_SHOW);
			break;
		case IDC_CLIENT2:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT2), hDlg, DlgProc), SW_SHOW);
			break;
		case IDC_CLIENT3:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT3), hDlg, DlgProc), SW_SHOW);
			break;
		case IDC_CLIENT4:
			ShowWindow(CreateDialogW(hInst, MAKEINTRESOURCEW(IDD_CLIENT4), hDlg, DlgProc), SW_SHOW);
			break;
		case IDOK: case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
	}
	return (INT_PTR)FALSE;
    }
    return (INT_PTR)FALSE;
}
