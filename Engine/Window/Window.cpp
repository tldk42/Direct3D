﻿#include "common_pch.h"
#include "Window.h"


EXTERN_C IMAGE_DOS_HEADER __ImageBase; // 모듈(실행 파일) 베이스 주소

int32_t                                 Window::s_windowNum = 0;
std::unordered_map<void*, Window*> Window::s_WindowHandles{};

LRESULT WndProc(HWND HWnd, UINT Message, WPARAM WParam, LPARAM LParam)
{

	switch (Message)
	{
	case WM_SIZE:
		// TODO: Resize Callback
		break;
	case WM_CLOSE:
		DestroyWindow(HWnd);
		UnregisterClass(L"Jacob Window", nullptr);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(HWnd, Message, WParam, LParam);
}


Window::Window()
	: mWindowHandle(nullptr),
	  mWindowTitle(nullptr),
	  bClosed(false)
{
	mInstanceHandle = reinterpret_cast<HINSTANCE>(&__ImageBase);

	mWindowTitle = std::format(L"default_window_{}", s_windowNum++).c_str();

	mWindowData.Width       = 1280;
	mWindowData.Height      = 800;
	mWindowData.bVsync      = false;
	mWindowData.bFullScreen = false;
}

Window::Window(LPCWSTR InWindowTitle, const FBasicWindowData& InWindowData)
	: mInstanceHandle(nullptr),
	  mWindowHandle(nullptr),
	  mWindowTitle(InWindowTitle),
	  mWindowData(InWindowData),
	  bClosed(false)
{
	mInstanceHandle = reinterpret_cast<HINSTANCE>(&__ImageBase);

	Initialize();
}

Window::~Window() {}

void Window::Initialize()
{
	WNDCLASSEX windowClass{};
	{
		windowClass.cbSize        = sizeof(WNDCLASSEX);
		windowClass.hInstance     = mInstanceHandle;
		windowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc   = static_cast<WNDPROC>(WndProc);
		windowClass.lpszClassName = L"Jacob Window";
		windowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hIcon         = static_cast<HICON>(
			LoadImage(nullptr, L"jacobengine.ico", IMAGE_ICON, GetSystemMetrics(SM_CXICON),
					  GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE));
		windowClass.hIconSm = static_cast<HICON>(LoadImage(nullptr, L"jacobengine.ico", IMAGE_ICON,
														   GetSystemMetrics(SM_CXSMICON),
														   GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE));
		windowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);

	}

	if (FAILED(RegisterClassEx(&windowClass)))
	{
		return;
	}

	RECT  size  = {0, 0, mWindowData.Width, mWindowData.Height};
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;

	AdjustWindowRect(&size, style, false);

	mWindowHandle = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		windowClass.lpszClassName,
		mWindowTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		// GetSystemMetrics는 시스템 설정 정보를 반환 nIndex에는 SM_으로 시작하는 상수값을 넣는다. ex) 2560 x 1600 (정중앙에 생성됨)
		GetSystemMetrics(SM_CXSCREEN) / 2 - mWindowData.Width / 2,       // 1280 - 450 = 830 (좌)
		GetSystemMetrics(SM_CYSCREEN) / 2 - mWindowData.Height / 2, // 800 - 300 = 500 (상)
		size.right + (-size.left),                                       // 윈도우 폭: 900
		size.bottom + (-size.top),                                       // 윈도우 높이: 600
		nullptr,
		nullptr,
		mInstanceHandle,
		nullptr
	);

	if (!mWindowHandle)
	{
		return;
	}

	RegisterWindow(mWindowHandle, this);

	ShowWindow(mWindowHandle, SW_SHOW);
	SetFocus(mWindowHandle);
}

void Window::Update()
{
	MSG message;

	while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE) > 0)
	{
		if (message.message == WM_QUIT)
		{
			bClosed = true;
			return;
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void Window::RegisterWindow(void* InHandle, Window* InWindowClass)
{
	s_WindowHandles[InHandle] = InWindowClass;
}

void Window::OnResize(UINT InWidth, UINT InHeight)
{
	mWindowData.Width  = InWidth;
	mWindowData.Height = InHeight;

	RECT  size  = {0, 0, mWindowData.Width, mWindowData.Height};
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&size, style, false);
}

Window* Window::GetWindow(void* InWindowHandle)
{
	if (!InWindowHandle)
	{
		return s_WindowHandles.begin()->second;
	}

	return s_WindowHandles[InWindowHandle];
}
