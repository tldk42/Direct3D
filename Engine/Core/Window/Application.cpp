#include "common_pch.h"
#include "Application.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/Font/XDWrite.h"
#include "Core/Graphics/Mesh/FBXLoader/MFBXManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/MManagerInterface.h"
#include "GUI/imgui/GUI_Viewport.h"
#include "Core/Utils/Logger.h"
#include "Core/Utils/Timer.h"
#include "Core/Utils/Math/Color.h"
#include "Core/Window/Window.h"

XDWrite g_FpsText(nullptr);

Application::Application()
	: Application(L"Renderer", FBasicWindowData(1600, 900, false, false)) {}

Application::Application(LPCWSTR WindowTitle, const FBasicWindowData& WindowData)
	: bRunning(false), bMinimized(false), mDeltaTime(0), mTime(0), mCurrentTime(0), mFramesPerSec(0)
{
	mWindow = std::make_unique<Window>(WindowTitle, WindowData);
}

Application::~Application() {}

void Application::Run()
{
	Initialize();

	while (bRunning)
	{
		mCurrentTime = mTimer.ElapsedMillis();

		HandleFrame();

		if (mTimer.Elapsed() - mTime > 1.f)
		{
			HandleTick();
		}

		CheckWindowClosure();
	}

	Application::Release();
}

void Application::Initialize()
{
	mTimer.Reset();
	bRunning   = true;
	bMinimized = false;
	mTime      = 0.f;

	//---------------------------------- 초기화 --------------------------------------------
	G_Logger.Initialize();			 // Logger

	mWindow->Initialize();			 // Win32

	G_Context.Initialize();			 // Graphic Device, Context...

	IManager.Initialize();

	g_FpsText.Initialize();
	g_FpsText.SetRenderTarget(IManager.ViewportManager.FetchResource(Name_Editor_Viewport)->RTV_2D.Get());
	g_FpsText.SetFontSize(48);
	g_FpsText.SetColor(FLinearColor::Orange);
	g_FpsText.SetScreenPosition({25, 25});

	// MFBXManager loader;
	// loader.Load();
}

void Application::Update(float DeltaTime)
{
	IManager.Update(DeltaTime);

	G_Context.Update(DeltaTime);

	g_FpsText.Update(DeltaTime);
	g_FpsText.SetText(std::format(L"fps: {:d}", mFramesPerSec));
}

void Application::Render()
{
	G_Context.ClearColor(FLinearColor::EbonyClay);

	IManager.Render(); // GUI Render

	g_FpsText.PreRender();
	g_FpsText.Render();
	g_FpsText.PostRender();

	G_Context.Present();
}

void Application::Release()
{
	IManager.Release();
	G_Context.Release();
}

uint32_t Application::GetWindowWidth() const
{
	return mWindow->GetWindowWidth();
}

uint32_t Application::GetWindowHeight() const
{
	return mWindow->GetWindowHeight();
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
	mTime += 1.f;

	mFramesPerSec = mFrameCounter;

	mFrameCounter = 0;
	// TODO: Tick
}


void Application::CheckWindowClosure()
{
	mWindow->Update();

	if (mWindow->IsClosed())
	{
		bRunning = false;
	}
}
