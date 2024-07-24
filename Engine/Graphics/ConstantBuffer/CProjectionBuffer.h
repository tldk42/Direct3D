#pragma once
#include "CConstantBuffer.h"

class CProjectionBuffer : public CConstantBuffer, public ICoreInterface
{
public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

public:
	inline void SetProjectionMatrix(const Matrix& InMatrix) { mProjectionMatrix = XMMatrixTranspose(InMatrix); }

protected:
	void CreateConstantBuffer() override;

private:
	Matrix mProjectionMatrix = XMMatrixIdentity();
};
