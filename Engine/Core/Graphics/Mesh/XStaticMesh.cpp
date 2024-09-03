#include "common_pch.h"
#include "XStaticMesh.h"

void XStaticMesh::Initialize()
{
	D3D11_BUFFER_DESC bufferDesc;
	{
		bufferDesc.ByteWidth      = mVertices.size() * sizeof(Vertex::FVertexInfo_Base); // 버퍼크기
		bufferDesc.Usage          = D3D11_USAGE_DEFAULT;	// 버퍼의 읽기/쓰기 방법 지정
		bufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // 파이프라인에 바인딩될 방법
		bufferDesc.CPUAccessFlags = 0; // 생성될 버퍼에 CPU가 접근하는 유형 (DX 성능에 매우 중요)
		bufferDesc.MiscFlags      = 0; // 추가적인 옵션 플래그
	}

	D3D11_SUBRESOURCE_DATA vertexData;
	{
		vertexData.pSysMem = mVertices.data(); // 초기화 데이터 포인터 (정점 배열의 주소를 넘겨준다)
		// InitData.SysMemPitch (텍스처 리소스의 한줄의 크기)
		// InitData.SysMemSlicePitch (3차원 텍스처의 깊이 간격)
	}

	CheckResult(
				G_Context.GetDevice()->CreateBuffer(
													&bufferDesc,
													&vertexData,
													mVertexBuffer.GetAddressOf()
												   ));
}

void XStaticMesh::Update(float_t DeltaTime) {}
void XStaticMesh::Release() {}
void XStaticMesh::PreRender() {}
void XStaticMesh::Render() {}
void XStaticMesh::PostRender() {}
