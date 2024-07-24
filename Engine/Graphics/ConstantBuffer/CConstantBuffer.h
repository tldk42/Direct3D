#pragma once

#include "Graphics/graphics_common_include.h"

class CConstantBuffer
{
public:
	CConstantBuffer()          = default;
	virtual ~CConstantBuffer() = default;

protected:
	virtual void CreateConstantBuffer() = 0;

protected:
	ComPtr<ID3D11Buffer> mConstantBuffer;
};
