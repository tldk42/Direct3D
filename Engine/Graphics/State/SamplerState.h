#pragma once
#include "Graphics/graphics_common_include.h"
#include "Utils/Math/Color.h"

enum class ESamplerType
{
	PointWrap,
	LinearWrap,
	PointBorder,
	LinearBorder
};

class SamplerState : public ICoreInterface
{
public:
	SamplerState(const UINT          InSlot  = 0, const ESamplerType InSamplerType = ESamplerType::LinearWrap,
				  const FLinearColor& InColor = FLinearColor());
	~SamplerState() override;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

private:
	UINT                       mSlot;
	ESamplerType               mSamplerType;
	FLinearColor               mColor;
	ComPtr<ID3D11SamplerState> mSampler;
};
