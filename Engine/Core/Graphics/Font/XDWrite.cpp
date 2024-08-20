#include "common_pch.h"
#include "XDWrite.h"

#include "Core/Graphics/GraphicDevice.h"
#include "Core/Utils/Math/Color.h"

XDWrite::XDWrite(ID2D1RenderTarget* InRenderTarget)
	: mRenderTarget(InRenderTarget),
	  mFontFamily(L"Gabriola"),
	  mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
	  mFontStyle(DWRITE_FONT_STYLE_NORMAL),
	  mFontStretch(DWRITE_FONT_STRETCH_NORMAL),
	  mFontSize(18),
	  mBrushColor({0, 0, 0, 1})
{}

XDWrite::~XDWrite() {}

void XDWrite::Initialize()
{
	AdjustTextFormat();
}

void XDWrite::Update(float_t DeltaTime) {}

void XDWrite::PreRender()
{
	// Draw 호출 전 Begin
	mRenderTarget->BeginDraw();
}

void XDWrite::Render()
{
	ID2D1SolidColorBrush* brush = GraphicDevice::GetBrush();
	assert(brush, "invalid brush");

	brush->SetColor(mBrushColor);

	mRenderTarget->DrawTextW(
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
}

void XDWrite::PostRender()
{
	// Draw 호출 후 End
	CheckResult(mRenderTarget->EndDraw());
}

ELayerType XDWrite::GetLayerType()
{
	return ELayerType::UI;
}

void XDWrite::SetRenderTarget(ID2D1RenderTarget* InRenderTarget)
{
	mRenderTarget = InRenderTarget;
}

void XDWrite::Release()
{
	mTextFormat = nullptr;
}


void XDWrite::SetText(const JWText& InText)
{
	mText = InText;
}

void XDWrite::SetColor(const FLinearColor& InColor)
{
	mBrushColor.r = InColor.R;
	mBrushColor.g = InColor.G;
	mBrushColor.b = InColor.B;
	mBrushColor.a = InColor.A;
}

void XDWrite::SetFontFamily(const JWText& InFontFamily)
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
