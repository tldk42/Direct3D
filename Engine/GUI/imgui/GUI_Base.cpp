#include "common_pch.h"
#include "GUI_Base.h"

#include "common_include.h"

GUI_Base::GUI_Base(const std::string& InTitle)
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

void GUI_Base::Release()
{}
