#include "common_pch.h"
#include "GUI_Base.h"

GUI_Base::GUI_Base(const char* InTitle)
	: mTitle(InTitle),
	  bVisible(true)
{}

void GUI_Base::Initialize()
{}

void GUI_Base::Update(float_t DeltaTime)
{}

void GUI_Base::PreRender()
{}

void GUI_Base::Render()
{}

void GUI_Base::PostRender() {}

ERenderingLayer GUI_Base::GetLayerType()
{
	return ERenderingLayer::GUI;
}

void GUI_Base::Release()
{}
