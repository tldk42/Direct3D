#pragma once
#include "Core/Interface/ICoreInterface.h"
#include "Core/Manager/Manager_Base.h"
#include "Core/Utils/Timer.h"

class XVertexBuffer;
struct FBasicWindowData;

#define MainApp Application::Get()

class Application final : public ICoreInterface, public TSingleton<Application>
{
public:
	void Run();

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render();
	void Release() override;
#pragma endregion

public:
	[[nodiscard]] FORCEINLINE float_t GetDeltaSeconds() const { return mDeltaTime; }
	[[nodiscard]] FORCEINLINE int32_t GetFramePerSeconds() const { return mFramesPerSec; }
	[[nodiscard]] uint32_t            GetWindowWidth() const;
	[[nodiscard]] uint32_t            GetWindowHeight() const;

private:
	void HandleFrame();
	void HandleTick();
	void CheckWindowClosure();

private:
	std::unique_ptr<class Window> mWindow;

	bool bRunning;
	bool bMinimized;

	Timer   mTimer;
	float_t mDeltaTime = 0.f;
	float_t mTime;
	float_t mCurrentTime;
	int32_t mFramesPerSec;
	int32_t mFrameCounter;


	// #pragma region Singleton Boilerplate
	//
	// private:
	// 	friend class TSingleton;
	//
public:
	Application();
	Application(LPCWSTR WindowTitle, const FBasicWindowData& WindowData);
	~Application() override;
	//
	// public:
	// 	Application(const Application&)            = delete;
	// 	Application& operator=(const Application&) = delete;
	//
	// #pragma endregion
};
