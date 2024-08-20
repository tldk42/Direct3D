#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Interface/IRenderable.h"

class JActor : public JObject, public IRenderable
{
public:

	
#pragma region Render Interface
	void       PreRender() override;
	void       Render() override;
	void       PostRender() override;
	ELayerType GetLayerType() override;
#pragma endregion


};
