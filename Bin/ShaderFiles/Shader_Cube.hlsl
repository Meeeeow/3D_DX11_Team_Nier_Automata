#include "Shader_Defines.hpp"

textureCUBE	g_DiffuseTexture;

/* 텍스쳐로부터 피셀을 가져오는 방식. */
sampler DefaultSampler = sampler_state
{

};




struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;	
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	/* XMVector3TransformCoord : 곱셈 후, w값으로 xyzw를 나눈다. */
	/* mul : 곱셈연산만 수행한다. */
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;	
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}




technique11		DefaultTechnique
{
	pass Sky
	{
		SetRasterizerState(CullMode_CW);
		SetBlendState(Blend_None, vector(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};


