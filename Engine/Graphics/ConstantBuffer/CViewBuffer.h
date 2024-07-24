#pragma once
#include "CConstantBuffer.h"

class CViewBuffer : public CConstantBuffer, public ICoreInterface
{
public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

public:
	inline void SetViewMatrix(const Matrix& InMatrix) { mViewMatrix = XMMatrixTranspose(InMatrix); }

protected:
	void CreateConstantBuffer() override;

private:
	Matrix mViewMatrix = XMMatrixIdentity();
};
