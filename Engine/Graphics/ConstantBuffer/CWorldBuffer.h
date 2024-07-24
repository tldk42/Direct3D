#pragma once
#include "CConstantBuffer.h"

class CWorldBuffer : public CConstantBuffer, public ICoreInterface
{
public:
	CWorldBuffer()           = default;
	~CWorldBuffer() override = default;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

public:
	void SetWorldMatrix(const Matrix& InMatrix) { mWorldMatrix = XMMatrixTranspose(InMatrix); }

protected:
	void CreateConstantBuffer() override;

private:
	Matrix mWorldMatrix = XMMatrixIdentity();
};
