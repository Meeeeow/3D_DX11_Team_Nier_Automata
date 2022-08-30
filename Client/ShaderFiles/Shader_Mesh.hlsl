#include "Shader_Defines.hpp"

// Vertex Texture Fetch
struct tagBoneMatrixArray
{
	matrix	Matrices[128];
};

/* 메시컨테이너 단위로 끊어서 들어오는 행렬배열. */
cbuffer BoneMatricesDesc
{
	tagBoneMatrixArray	g_BoneMatrices;
};

cbuffer CameraDesc
{
	vector		g_vCamPosition;
};

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;

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
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN_SKY(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	/* XMVector3TransformCoord : 곱셈 후, w값으로 xyzw를 나눈다. */
	/* mul : 곱셈연산만 수행한다. */
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

VS_OUT VS_MAIN_NONANIM(VS_IN In)
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

struct VS_OUT_NORMAL
{
	float4		vPosition : SV_POSITION;
	
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vRight : TEXCOORD3;
	float3		vUp : TEXCOORD4;
	float3		vLook : TEXCOORD5;

};

VS_OUT_NORMAL VS_MAIN_NONANIM_NORMAL(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	matrix		matWV, matWVP;

	/* XMVector3TransformCoord : 곱셈 후, w값으로 xyzw를 나눈다. */
	/* mul : 곱셈연산만 수행한다. */
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);


	Out.vLook = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
	Out.vRight = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vUp = normalize(cross(Out.vLook, Out.vRight));

	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	

	return Out;

}

VS_OUT VS_MAIN_ANIM(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);


	/* 애니메이션 재생에 따른 상태변환행렬이다. 로컬 -> 로컬 */
	matrix		BoneMatrix = g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.Matrices[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP;

	/* XMVector3TransformCoord : 곱셈 후, w값으로 xyzw를 나눈다. */
	/* mul : 곱셈연산만 수행한다. */
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
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

PS_OUT PS_MAIN_SKY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);	
		
	/* -1 ~ 1 -> 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	if (Out.vDiffuse.a <= 0.3f)
		discard;

	return Out;
}

struct PS_IN_NORMAL
{
	float4		vPosition : SV_POSITION;

	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vRight : TEXCOORD3;
	float3		vUp : TEXCOORD4;
	float3		vLook : TEXCOORD5;

};


PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector	vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;
	
	float3x3	WorldMatrix = float3x3(In.vRight, In.vUp, In.vLook);

	Out.vNormal = vector(mul(vNormal, WorldMatrix), 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	if (Out.vDiffuse.a <= 0.3f)
		discard;

	return Out;
}


technique11		DefaultTechnique
{
	pass NonAnim
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NonAnim_NormalMap
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

	pass Anim
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Sky
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN_SKY();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SKY();
	}
};


