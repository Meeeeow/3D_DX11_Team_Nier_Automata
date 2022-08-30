#include "Shader_Defines.hpp"

cbuffer CameraDesc
{
	vector		g_vCamPosition;
};

cbuffer BrushDesc
{
	vector		g_vBrushPosition = vector(10.f, 0.f, 10.f, 1.f);
	float		g_fRange = 3.f;
};

texture2D	g_SourTexture;
texture2D	g_DestTexture;
texture2D	g_FilterTexture;
texture2D	g_BrushTexture;

/* 텍스쳐로부터 피셀을 가져오는 방식. */
sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
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
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;	
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vSourDiffuse = g_SourTexture.Sample(DefaultSampler, In.vTexUV * 20.f);
	vector		vDestDiffuse = g_DestTexture.Sample(DefaultSampler, In.vTexUV * 20.f);
	vector		vFilter = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);
	
	
	vector		vBrush = (vector)0.f;

	if (g_vBrushPosition.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPosition.x + g_fRange &&
		g_vBrushPosition.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPosition.z + g_fRange)
	{
		float2		vTexUV;

		vTexUV.x = (In.vWorldPos.x - (g_vBrushPosition.x - g_fRange)) / (2.f * g_fRange);
		vTexUV.y = ((g_vBrushPosition.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange);

		vBrush = g_BrushTexture.Sample(DefaultSampler, vTexUV);
	}

	Out.vDiffuse = vSourDiffuse * vFilter + vDestDiffuse * (1.f - vFilter) + vBrush;

	Out.vDiffuse.a = 1.f;

	/* -1 ~ 1 -> 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;
}







technique11		DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Default, 0);
		

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass WireFrame
	{
		SetRasterizerState(CullMode_Wireframe);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Default, 0);


		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};


