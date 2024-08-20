#include "common_pch.h"
#include "Core/Window/Application.h"
#include "Core/Window/Window.h"

// SPECTRUM_USE_LIGHT_THEME (light 버전)
#define SPECTRUM_USE_DARK_THEME

constexpr uint32_t FHD_X = 1920;
constexpr uint32_t FHD_Y = 1080;
constexpr uint32_t QHD_X = 2560;
constexpr uint32_t QHD_Y = 1440;

FBasicWindowData g_initialWinData{FHD_X, FHD_Y, false, false};

int main(int argc, char** argv)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Application App(L"J.01", g_initialWinData);
	App.Run();

}
