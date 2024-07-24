#pragma once

#include "Graphics/graphics_common_include.h"
#include "Utils/Math/Vector2D.h"


class XDWrite : public ICoreInterface
{
public:
	XDWrite();
	~XDWrite() override;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

public:
	void SetText(const WText& InText);
	void SetFontFamily(const WText& InFontFamily);
	void SetFontWeight(const DWRITE_FONT_WEIGHT InFontWeight);
	void SetFontStyle(const DWRITE_FONT_STYLE InFontStyle);
	void SetFontStretch(const DWRITE_FONT_STRETCH InFontStretch);
	void SetFontSize(const float InFontSize);

	void SetScreenPosition(const FVector2D& InLocation);

private:
	void AdjustTextFormat();

private:
	ComPtr<IDWriteTextFormat> mTextFormat;

	const wchar_t*      mFontFamily;
	DWRITE_FONT_WEIGHT  mFontWeight;
	DWRITE_FONT_STYLE   mFontStyle;
	DWRITE_FONT_STRETCH mFontStretch;
	float               mFontSize;
	WText               mText;

	FVector2D mScreenPosition;
};
