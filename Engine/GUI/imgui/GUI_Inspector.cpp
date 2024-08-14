#include "common_pch.h"
#include "GUI_Inspector.h"

GUI_Inspector::GUI_Inspector(const char* InTitle)
	: GUI_Base(InTitle) {}

void GUI_Inspector::Initialize()
{
	GUI_Base::Initialize();
}

void GUI_Inspector::Update(float_t DeltaTime)
{
	GUI_Base::Update(DeltaTime);
}

void GUI_Inspector::Release()
{
	GUI_Base::Release();
}

void GUI_Inspector::Render()
{
	ImGui::Begin(mTitle);
	{
		ImVec2 curSize = ImGui::GetContentRegionAvail();


		
		ImGui::End();
	}
}
