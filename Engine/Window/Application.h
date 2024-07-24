#pragma once
#include "common_include.h"
#include "Manager/Manager_Base.h"
#include "Utils/Timer.h"

class XVertexBuffer;
struct FBasicWindowData;

#define App Application::Get()

class Application final : public ICoreInterface, public TSingleton<Application>
{
public:
	void Run();

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

public:
	[[nodiscard]] float_t GetDeltaSeconds() const { return mDeltaTime; }
	[[nodiscard]] int32_t GetFramePerSeconds() const { return mFramesPerSec; }

private:
	void HandleFrame();
	void HandleTick();
	void CheckWindowClosure();

private:
	std::unique_ptr<class Window> mWindow;

	bool bRunning;
	bool bMinimized;

	Timer   mTimer;
	float_t mDeltaTime;
	float_t mTime;
	float_t mCurrentTime;
	int32_t mFramesPerSec;
	int32_t mFrameCounter;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton;

	Application();
	Application(LPCWSTR WindowTitle, const FBasicWindowData& WindowData);
	~Application() override;

public:
	Application(const Application&)            = delete;
	Application& operator=(const Application&) = delete;

#pragma endregion
};
