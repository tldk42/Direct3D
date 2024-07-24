cbuffer WorldMatrixBuffer : register(b0)
{
matrix world;
}

cbuffer ViewProjectionBuffer : register(b1)
{
matrix view;
}

cbuffer ProjectionBuffer : register(b2)
{
matrix projection;
}

struct VS_INPUT
{
float3 Pos : POSITION;
float4 Col : COLOR;
float2 Tex : TEX;
};

struct VS_OUTPUT
{
float4 Pos : SV_Position;
float4 Col : COLOR;
float2 Tex : TEXCOORD0;
};

VS_OUTPUT vs(VS_INPUT input)
{
VS_OUTPUT output;

output.Pos = float4(input.Pos,  1.f);
//
// output.Pos = mul(output.Pos, world);
// output.Pos = mul(output.Pos, view);
// output.Pos = mul(output.Pos, projection);

output.Tex = input.Tex;
output.Col = input.Col;

return output;
}


Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

float4 ps(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor;
    float4 overlayColor = input.Col;
	
    texColor = shaderTexture.Sample(SampleType, input.Tex);
    
    texColor.a *= input.Col.a;
    
    // if (texColor.a <= .4f)
    // {
    //     discard;
    // }    

	return texColor;
}
