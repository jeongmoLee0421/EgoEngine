// 2023 07 13 ������ home

Texture2D texDiffuseMap: register(t0);
SamplerState samLinear: register(s0);

struct PS_INPUT
{
	float4 Pos: SV_POSITION;
	float3 Normal: NORMAL;
	float2 Tex: TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_Target
{
	return texDiffuseMap.Sample(samLinear, input.Tex);
}