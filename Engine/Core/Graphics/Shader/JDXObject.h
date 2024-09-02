#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Interface/IRenderable.h"


[[nodiscard]] inline HRESULT LoadVertexShader(ID3D11Device*        Device, const JWText& VertexFileName,
											  ID3D11VertexShader** VertexShader,
											  ID3DBlob**           OutBlob = nullptr, LPCSTR EntryPoint = nullptr);
[[nodiscard]] inline HRESULT LoadPixelShader(ID3D11Device*       Device, const JWText& PixelFileName,
											 ID3D11PixelShader** pixelShader,
											 ID3DBlob**          OutBlob = nullptr, LPCSTR EntryPoint = nullptr);

[[nodiscard]] inline HRESULT LoadGeometryShader(ID3D11Device*          Device, const JWText&      ShaderFile,
												ID3D11GeometryShader** geometryShader, ID3DBlob** OutBlob = nullptr,
												LPCSTR                 FuncName                           = nullptr);
[[nodiscard]] inline HRESULT LoadHullShaderFile(ID3D11Device*      Device, const JWText&,
												ID3D11HullShader** hullShader, ID3DBlob** OutBlob = nullptr,
												LPCSTR             FuncName                       = nullptr);
[[nodiscard]] inline HRESULT LoadDomainShaderFile(ID3D11Device*        Device, const JWText&    ShaderFile,
												  ID3D11DomainShader** domainShader, ID3DBlob** OutBlob = nullptr,
												  LPCSTR               FuncName                         = nullptr);
[[nodiscard]] inline HRESULT LoadComputeShaderFile(ID3D11Device*         Device, const JWText&     ShaderFile,
												   ID3D11ComputeShader** computeShader, ID3DBlob** OutBlob = nullptr,
												   LPCSTR                FuncName                          = nullptr);


[[nodiscard]] inline HRESULT CompileShader(const WCHAR* FileName, LPCSTR        EntryPoint,
										   LPCSTR       ShaderModel, ID3DBlob** OutBlob);

class JDXObject : public IRenderable
{
public:
	explicit JDXObject(const JWText& InShaderFile, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	~JDXObject();

public:
	void Release();

#pragma region Render Interface
	void PreRender() override;
	void Render() override;
	void PostRender() override;
#pragma endregion

public:
	void SetVertexShader(JWTextView InFile, LPCSTR FuncName);
	void SetPixelShader(JWTextView InFile, LPCSTR FuncName);
	void SetGeometryShader(JWTextView InFile, LPCSTR FuncName);
	void SetHullShader(JWTextView InFile, LPCSTR FuncName);
	void SetDomainShader(JWTextView InFile, LPCSTR FuncName);
	void SetComputeShader(JWTextView InFile, LPCSTR FuncName);

public:
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetVertexBuffer() const { return mVertexBuffer.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetIndexBuffer() const { return mIndexBuffer.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetCBuffer() const { return mConstantBuffer.Get(); }
	[[nodiscard]] FORCEINLINE D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return mPrimitiveType; }

private:
	void HandleLayout();

private:
	JWText mShaderFile;

#pragma region Shader

	ComPtr<ID3D11Buffer> mVertexBuffer;
	ComPtr<ID3D11Buffer> mIndexBuffer;
	ComPtr<ID3D11Buffer> mConstantBuffer;

	ComPtr<ID3D11VertexShader>   mVertexShader;
	ComPtr<ID3D11PixelShader>    mPixelShader;
	ComPtr<ID3D11GeometryShader> mGeometryShader;
	ComPtr<ID3D11HullShader>     mHullShader;
	ComPtr<ID3D11DomainShader>   mDomainShader;
	ComPtr<ID3D11ComputeShader>  mComputeShader;

	ComPtr<ID3DBlob> mVertexShaderBuf;
	ComPtr<ID3DBlob> mPixelShaderBuf;
	ComPtr<ID3DBlob> mGeometryShaderBuf;
	ComPtr<ID3DBlob> mHullShaderBuf;
	ComPtr<ID3DBlob> mDomainShaderBuf;
	ComPtr<ID3DBlob> mComputeShaderBuf;

	ComPtr<ID3D11InputLayout> mInputLayout;

	class XTexture2D* mTexture;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType;
	uint32_t                 mVertexNum;
	uint32_t                 mIndexNum;
	uint32_t                 mVertexSize;
	uint32_t                 mIndexSize;
#pragma endregion
};
