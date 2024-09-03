#pragma once
#include "common_include.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/ICoreInterface.h"
#include "Core/Interface/IRenderable.h"

class XVertexMesh;

class XStaticMesh : public ICoreInterface, public IRenderable
{
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
#pragma endregion

protected:
	std::vector<FVertexInfo_Simple> mVertices;

	ComPtr<ID3D11Buffer> mVertexBuffer;
};
