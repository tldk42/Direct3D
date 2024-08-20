#pragma once

#include "Core/Interface/ICoreInterface.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/Math/Vector2D.h"


class XDWrite : public ICoreInterface, public IRenderable
{
public:
	explicit XDWrite(ID2D1RenderTarget* InRenderTarget);
	~XDWrite() override;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;
#pragma endregion

#pragma region Render Interface
	void       PreRender() override;
	void       Render() override;
	void       PostRender() override;
	ELayerType GetLayerType() override;
#pragma endregion

public:
	void SetRenderTarget(ID2D1RenderTarget* InRenderTarget);
	void SetText(const JWText& InText);
	void SetColor(const struct FLinearColor& InColor);
	void SetFontFamily(const JWText& InFontFamily);
	void SetFontWeight(const DWRITE_FONT_WEIGHT InFontWeight);
	void SetFontStyle(const DWRITE_FONT_STYLE InFontStyle);
	void SetFontStretch(const DWRITE_FONT_STRETCH InFontStretch);
	void SetFontSize(const float InFontSize);

	void SetScreenPosition(const FVector2D& InLocation);

private:
	void AdjustTextFormat();

private:
	ComPtr<IDWriteTextFormat> mTextFormat;
	ID2D1RenderTarget*        mRenderTarget;
	const wchar_t*            mFontFamily;
	DWRITE_FONT_WEIGHT        mFontWeight;
	DWRITE_FONT_STYLE         mFontStyle;
	DWRITE_FONT_STRETCH       mFontStretch;
	float                     mFontSize;
	JWText                    mText;
	D2D1_COLOR_F              mBrushColor;

	FVector2D mScreenPosition;
};
