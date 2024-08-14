#pragma once

#include "GUI_Base.h"

class GUI_Viewport : public GUI_Base
{
public:
	GUI_Viewport(const char* InTitle);
	~GUI_Viewport() override = default;

public:
	void Initialize() override;
	void Render() override;

public:
	[[nodiscard]] FORCEINLINE bool IsBIsFocused() const { return bIsFocused; }
	[[nodiscard]] FORCEINLINE bool IsBIsHovered() const { return bIsHovered; }

protected:
	bool bIsFocused;
	bool bIsHovered;
};
