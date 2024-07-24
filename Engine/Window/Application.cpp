#include "common_pch.h"
#include "Application.h"

#include "Graphics/GraphicDevice.h"
#include "Graphics/Font/XDWrite.h"
#include "Graphics/Vertex/XVertexBuffer.h"
#include "Utils/Logger.h"
#include "Utils/Math/Color.h"
#include "Window/Window.h"

Application::Application()
	: Application(L"Renderer", FBasicWindowData(1600, 900, false, false))
{}

Application::Application(LPCWSTR WindowTitle, const FBasicWindowData& WindowData)
	: bRunning(false), bMinimized(false), mDeltaTime(0), mTime(0), mCurrentTime(0), mFramesPerSec(0)
{
	mWindow = std::make_unique<Window>(WindowTitle, WindowData);

}

Application::~Application()
{
	Application::Release();
}

void Application::Run()
{
	Initialize();

	while (bRunning)
	{
		CheckWindowClosure();

		mCurrentTime = mTimer.ElapsedMillis();

		HandleFrame();

		HandleTick();
	}
}

void Application::Initialize()
{
	G_Logger.Initialize();

	mWindow->Initialize();

	mTimer.Reset();
	bRunning   = true;
	bMinimized = false;
	mTime      = 0.f;

	G_Context.Initialize();
}

void Application::Update(float DeltaTime)
{
	mWindow->Update();
	G_Context.Update(DeltaTime);
}

void Application::Render()
{
	G_Context.ClearColor(FLinearColor::Cascade);


	G_Context.Render();
}

void Application::Release()
{

	G_Context.Release();
}

void Application::HandleFrame()
{
	Timer frameTimer;
	{
		Update(mDeltaTime);

		Render();
	}
	mFrameCounter++;
	mDeltaTime = frameTimer.Elapsed();
}

void Application::HandleTick()
{
	if (mTimer.Elapsed() - mTime > 1.f)
	{
		mTime += 1.f;

		mFramesPerSec = mFrameCounter;

		mFrameCounter = 0;
		// TODO: Tick
	}
}

void Application::CheckWindowClosure()
{
	if (mWindow->IsClosed())
	{
		bRunning = false;
	}
}
