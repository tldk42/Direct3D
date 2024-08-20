#pragma once
#include "common_include.h"
#include "Core/Interface/IRenderable.h"

class XVertexBuffer;

class XStaticMesh : public IRenderable
{
public:
#pragma region Render Interface
	void       PreRender() override;
	void       Render() override;
	void       PostRender() override;
	ELayerType GetLayerType() override;
#pragma endregion

protected:
	UPtr<XVertexBuffer> mVertexBuffer;
};
