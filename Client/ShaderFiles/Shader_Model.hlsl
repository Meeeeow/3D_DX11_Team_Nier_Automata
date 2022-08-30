#include "Shader_Defines.hlsli"

struct tagBoneMatrixArr {
	matrix				matBones[128];
};

cbuffer tagBoneMatricesDesc {
	tagBoneMatrixArr	g_tBoneMatrices;
};

cbuffer tagAlphaDesc {
	float				fAlpha = 1.0f;
};


texture2D g_texDiffuse;

sampler DefaultSampler = sampler_state {
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN {
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};


VS_OUT VS_MAIN_NONANIM(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_ANIM(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	/* 애니메이션 재생에 따른 상태변환 행렬.
	로컬 -> 로컬 */
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		matBone = g_tBoneMatrices.matBones[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_tBoneMatrices.matBones[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_tBoneMatrices.matBones[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_tBoneMatrices.matBones[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), matBone);
	vector		vNormal = mul(vector(In.vNormal, 0.f), matBone);

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT {
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	if (Out.vDiffuse.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DEAD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	Out.vDiffuse.a = fAlpha;

	return Out;
}

technique11		DefaultTechnique {
	pass NonAnim {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Anim {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NonAnim_BackCull {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Anim_Dead {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEAD();
	}
};
