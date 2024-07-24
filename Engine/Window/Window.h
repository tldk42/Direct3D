﻿#pragma once

struct FBasicWindowData
{
	uint32_t Width;
	uint32_t Height;
	bool     bFullScreen;
	bool     bVsync;
};

class Window
{
public:
	explicit Window();
	explicit Window(LPCWSTR InWindowTitle, const FBasicWindowData& InWindowData);
	~Window();

public:
	void Initialize();
	void Update();

public:
	static Window* GetWindow(void* InWindowHandle = nullptr);

private:
	static void RegisterWindow(void* InHandle, Window* InWindowClass);

public:
	[[nodiscard]]FORCEINLINE HINSTANCE               GetInstanceHandle() const { return mInstanceHandle; }
	[[nodiscard]]FORCEINLINE HWND                    GetWindowHandle() const { return mWindowHandle; }
	[[nodiscard]]FORCEINLINE LPCWSTR                 GetWindowTitle() const { return mWindowTitle; }
	[[nodiscard]]FORCEINLINE const FBasicWindowData& GetWindowData() const { return mWindowData; }
	[[nodiscard]]FORCEINLINE uint32_t                GetWindowWidth() const { return mWindowData.Width; }
	[[nodiscard]]FORCEINLINE uint32_t                GetWindowHeight() const { return mWindowData.Height; }
	[[nodiscard]]FORCEINLINE bool                    IsClosed() const { return bClosed; }
	[[nodiscard]] FORCEINLINE bool                   IsFullScreen() const { return mWindowData.bFullScreen; }
	[[nodiscard]] FORCEINLINE bool                   IsVsyncEnabled() const { return mWindowData.bVsync; }

public:
	void OnResize(UINT InWidth, UINT InHeight) const;

private:
	void Resize(UINT InWidth, UINT InHeight);
public:
	std::vector<std::function<void(UINT Width, UINT Height)>> ResizeCallbacks;

private:
	HINSTANCE        mInstanceHandle;
	HWND             mWindowHandle;
	LPCWSTR          mWindowTitle;
	FBasicWindowData mWindowData;
	bool             bClosed;

	static int32_t                            s_windowNum;
	static std::unordered_map<void*, Window*> s_WindowHandles;

	friend LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};
