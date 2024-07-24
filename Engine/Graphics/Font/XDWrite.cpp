#include "common_pch.h"
#include "XDWrite.h"
#include "Graphics/GraphicDevice.h"

XDWrite::XDWrite()
	: mFontFamily(L"Gabriola"),
	  mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
	  mFontStyle(DWRITE_FONT_STYLE_NORMAL),
	  mFontStretch(DWRITE_FONT_STRETCH_NORMAL),
	  mFontSize(18) {}

XDWrite::~XDWrite() {}

void XDWrite::Initialize()
{
	AdjustTextFormat();
}

void XDWrite::Update(float_t DeltaTime) {}

void XDWrite::Render()
{
	// Draw 호출 전 Begin
	GraphicDevice::GetSurface1()->BeginDraw();

	GraphicDevice::GetSurface1()->DrawTextW(
		mText.c_str(),
		mText.length(),
		mTextFormat.Get(),
		{
			mScreenPosition.X,
			mScreenPosition.Y,
			mScreenPosition.X + 600.f,
			mScreenPosition.Y + 600.f
		},
		GraphicDevice::GetBrush()
	);

	// Draw 호출 후 End
	CheckResult(GraphicDevice::GetSurface1()->EndDraw());
}

void XDWrite::Release()
{
	mTextFormat = nullptr;
}

void XDWrite::SetText(const WText& InText)
{
	mText = InText;
}

void XDWrite::SetFontFamily(const WText& InFontFamily)
{
	mFontFamily = InFontFamily.c_str();

	AdjustTextFormat();
}

void XDWrite::SetFontStyle(const DWRITE_FONT_STYLE InFontStyle)
{
	mFontStyle = InFontStyle;

	AdjustTextFormat();
}

void XDWrite::SetFontStretch(const DWRITE_FONT_STRETCH InFontStretch)
{
	mFontStretch = InFontStretch;

	AdjustTextFormat();
}

void XDWrite::SetFontWeight(const DWRITE_FONT_WEIGHT InFontWeight)
{
	mFontWeight = InFontWeight;

	AdjustTextFormat();
}

void XDWrite::SetFontSize(const float InFontSize)
{
	mFontSize = InFontSize;

	AdjustTextFormat();
}

void XDWrite::SetScreenPosition(const FVector2D& InLocation)
{
	mScreenPosition = InLocation;
}

void XDWrite::AdjustTextFormat()
{
	mTextFormat = nullptr;

	// DWrite에서는 Format만 관리
	CheckResult(
		GraphicDevice::GetWriteFactory()->
		CreateTextFormat(
			mFontFamily,
			nullptr,
			mFontWeight,
			mFontStyle,
			mFontStretch,
			mFontSize,
			L"en-us",
			mTextFormat.GetAddressOf()
		));
}
