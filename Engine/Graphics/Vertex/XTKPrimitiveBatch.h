#pragma once

#include "Graphics/graphics_common_include.h"
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <directxtk/Effects.h>
#include <directxtk/PrimitiveBatch.h>
#include <directxtk/VertexTypes.h>

#define G_Batch XTKPrimitiveBatch::Get()

class XTKPrimitiveBatch : public ICoreInterface, public TSingleton<XTKPrimitiveBatch>
{
public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Render() override;
	void EndRender() const;
	void Release() override;
#pragma endregion

public:
	void Draw(BoundingSphere& InSphere, FXMVECTOR InColor = Colors::White) const;
	void Draw(const BoundingBox& InBox, FXMVECTOR InColor = Colors::White) const;
	void Draw(const BoundingOrientedBox& InObb, FXMVECTOR InColor = Colors::White) const;
	void Draw(const BoundingFrustum& InFrustum, FXMVECTOR InColor = Colors::White) const;
	void DrawCube(CXMMATRIX matWorld, FXMVECTOR color) const;
	void DrawGrid(FXMVECTOR InXAxis, FXMVECTOR InYAxis, FXMVECTOR InOrigin, size_t InXdivs, size_t InYdivs,
				  GXMVECTOR InColor = Colors::White) const;
	void DrawRing(FXMVECTOR InOrigin, FXMVECTOR InMajorAxis, FXMVECTOR InMinorAxis,
				  GXMVECTOR InColor = Colors::White) const;
	void DrawRay(FXMVECTOR InOrigin, FXMVECTOR InDirection, bool bNormalize = true,
				 FXMVECTOR InColor                                          = Colors::White) const;
	void DrawTriangle(FXMVECTOR InPointA, FXMVECTOR InPointB, FXMVECTOR InPointC, GXMVECTOR InColor = Colors::White) const;
	void DrawQuad(FXMVECTOR InPointA, FXMVECTOR InPointB, FXMVECTOR InPointC, GXMVECTOR InPointD,
				  HXMVECTOR InColor = Colors::White) const;

private:
	std::unique_ptr<BasicEffect>                         mBatchEffect;
	std::unique_ptr<PrimitiveBatch<VertexPositionColor>> mBatch;
	ComPtr<ID3D11InputLayout>                            mBatchInputLayout;

	#pragma region Singleton Boilerplate

private:
	friend class TSingleton<XTKPrimitiveBatch>;

	XTKPrimitiveBatch();
	~XTKPrimitiveBatch() override;

public:
	XTKPrimitiveBatch(const XTKPrimitiveBatch&)            = delete;
	XTKPrimitiveBatch& operator=(const XTKPrimitiveBatch&) = delete;

#pragma endregion
};
