# 셰이더 필기 노트





작성할건 Vertex와 Pixel 셰이더에 대해서 작성

- Vertex Shader
  - 정점 위치등 추가 데이터 결정
``` hlsl
  struct VS_INPUT
  {
        // 정점 데이터에서 위치 정보를 가져와 mPosition에 대입
        float4 mPosition : POSITION;~~~~ 
  }
```
- Pixel Shader
  - 출력 색상을 결정


