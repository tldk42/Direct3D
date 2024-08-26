#include "common_pch.h"
#include "MManagerInterface.h"

#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Viewport/MViewportManager.h"


MManagerInterface::MManagerInterface()
	: TextureManager(),
	  ShaderManager(),
	  GUIManager(),
	  ViewportManager(),
	  CameraManager(),
	  LayerManager() {}

void MManagerInterface::Initialize()
{
	CameraManager.Initialize();

	GUIManager.Initialize();

	// 에디터 뷰포트 창 생성
	ViewportManager.CreateOrLoad(Name_Editor_Viewport, 1920, 1080);

	G_DebugBatch.Initialize();		 // Primitive Batch
}

void MManagerInterface::Update(float DeltaTime)
{
	GUIManager.Update(DeltaTime);

	G_DebugBatch.Update(DeltaTime);
}

void MManagerInterface::Render()
{
	GUIManager.Render();

	// GUI 먼저 업데이트 후 뷰포트 업데이트
	ViewportManager.SetRenderTarget("Editor Viewport");

	G_DebugBatch.PreRender();

	G_DebugBatch.Render();

	G_DebugBatch.DrawGrid(
						  {40.f, 0.f, 0.f, 0.f},
						  {0.f, 0.f, 40.f, 0.f},
						  {0.f, -5.f, 0.f, 0.f},
						  40.f, 40.f,
						  Colors::Gray
						 );

	G_DebugBatch.PostRender();

	LayerManager.Render();
}

void MManagerInterface::Release()
{
	G_DebugBatch.Release();

	GUIManager.Release();
}
