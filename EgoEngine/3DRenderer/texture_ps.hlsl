// 2023 06 28 ÀÌÁ¤¸ð home

Texture2D texDiffuseMap: register(t0);
SamplerState samLinear: register(s0);

struct PS_INPUT
{
	float4 Pos: SV_POSITION;
	float2 Tex: TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_Target
{
	return texDiffuseMap.Sample(samLinear, input.Tex);
}