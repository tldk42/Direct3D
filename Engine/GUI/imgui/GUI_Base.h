#pragma once
#include <imgui/imgui.h>
#include "Core/Interface/ICoreInterface.h"
#include "Core/Interface/IRenderable.h"

class GUI_Base : public ICoreInterface, public IRenderable
{
public:
	explicit GUI_Base(const char* InTitle);
	~GUI_Base() override = default;

public:
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;

public:
	void PreRender() override;
	void Render() override;
	void PostRender() override;

	ERenderingLayer GetLayerType() override;

	FORCEINLINE bool GetVisibility() const { return bVisible; }
	FORCEINLINE void SetVisibility(bool bActiveVisible) { bVisible = bActiveVisible; }

protected:
	const char* mTitle;
	bool        bVisible;
};
