#include "Shader_Defines.hlsli"

struct tagBoneMatrixArr {
	matrix				matBones[128];
};

cbuffer tagBoneMatricesDesc {
	tagBoneMatrixArr	g_tBoneMatrices;
};

cbuffer tagPreBoneMatricesDesc {
	tagBoneMatrixArr	g_tPreBoneMatrices;
};


cbuffer tagRimLightBuffer {
	float3				g_vRimLightColor = float3(0.f, 0.f, 0.f);
	float				fRimLightPadding = 0.f;
};

// For DOF
cbuffer DOF {
	float4		vDofParams;
	//			x - near plane;
	//			y - focal plane;
	//			z - far plane;
	//			w - max alpha;
};

float	ComputeDepthBlur(float Depth)
{
	float f;

	if (Depth < vDofParams.y)
	{
		f = (Depth - vDofParams.y) / (vDofParams.y - vDofParams.x);
	}
	else
	{
		f = (Depth - vDofParams.y) / (vDofParams.z - vDofParams.y);
		f = clamp(f, 0.f, vDofParams.w);
	}

	return f * 0.5f + 0.5f;
}

float		g_fDissolve;
float		fAlpha = 1.0f;

int			g_iIsGlow = 0;
bool		g_isActive = false;
texture2D	g_texDiffuse;
texture2D	g_texNormal;
texture2D	g_texAlphaOne;
texture2D	g_texColor;
texture2D	g_texDissolve;
texture2D	g_texEffect;

cbuffer tagEffectTextureBuffer
{
	int			g_iIsEffectTexture = 0;
	float3		vEffectTexturePadding;
};

sampler DefaultSampler = sampler_state {
	
	AddressU = wrap;
	AddressV = wrap;
};

// Deferred ////////////////////////////////////////////////////

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
	float4		vRim : TEXCOORD3;
	float		fDepth : TEXCOORD4;
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
	
	float4	vViewPos = mul(Out.vWorldPos, g_matView);
	Out.fDepth = vViewPos.z;

	// RimLight

	vector ToCameraVec = vector(g_vCamPos - Out.vWorldPos);
	ToCameraVec.a = 0.f;
	ToCameraVec = normalize(ToCameraVec);
	float fRim = saturate(dot(Out.vNormal, ToCameraVec));


	Out.vRim.rgba = fRim + 0.1f;

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
	Out.vNormal = normalize(mul(vNormal, g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vPosition, g_matWorld);
	Out.vProjPos = Out.vPosition;
	float4	vViewPos = mul(Out.vWorldPos, g_matView);
	Out.fDepth = vViewPos.z;

	// RimLight
	vector ToCameraVec = vector(g_vCamPos - Out.vWorldPos);
	ToCameraVec.a = 0.f;
	ToCameraVec = normalize(ToCameraVec);
	float fRim = saturate(dot(Out.vNormal, ToCameraVec));
	//if (fRim > 0.7f)
	//{
	//	Out.vRim.rgba = fRim;
	//}
	Out.vRim.rgba = fRim + 0.05f;

	return Out;
}


VS_OUT VS_MAIN_SKY(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	/* XMVector3TransformCoord : 곱셈 후, w값으로 xyzw를 나눈다. */
	/* mul : 곱셈연산만 수행한다. */
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);
	float4	vViewPos = mul(Out.vWorldPos, g_matView);
	Out.fDepth = vViewPos.z;

	return Out;
}

struct PS_IN {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vRim : TEXCOORD3;
	float		fDepth : TEXCOORD4;
};

struct PS_OUT {
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vGlow : SV_TARGET3;
	vector		vRimLight : SV_TARGET4;
};


struct PS_OUT_SKY {
	vector		vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, In.fDepth / g_fFar, 1.0f);

	if (g_iIsGlow != 0)
	{
		Out.vGlow = Out.vDiffuse;
	}

	float fRim = In.vRim.x;
	float3 rimColor = g_vRimLightColor;

	rimColor *= fRim;

	Out.vRimLight = vector(rimColor, 0.f);

	Out.vGlow += Out.vRimLight;

	if (g_iIsEffectTexture)
	{
		vector vEffectTexture = g_texEffect.Sample(DefaultSampler, In.vTexUV);
		Out.vDiffuse.rgb += vEffectTexture.rgb;
	}


	if (Out.vDiffuse.a <= 0.01f)
	{
		discard;
	}
	else
	{
		Out.vDiffuse.a = ComputeDepthBlur(In.fDepth);
	}


	return Out;
}

PS_OUT PS_MAIN_NONANIM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, In.fDepth / g_fFar, 1.0f);

	if (g_iIsGlow != 0)
	{
		Out.vGlow = Out.vDiffuse;
	}

	float fRim = In.vRim.x;

	float3 rimColor = g_vRimLightColor;
	rimColor *= fRim;
	Out.vRimLight = vector(rimColor, 0.f);
	Out.vGlow += Out.vRimLight;

	if (Out.vDiffuse.a == 0.0f)
	{
		discard;
	}
	else
	{
		Out.vDiffuse.a = ComputeDepthBlur(In.fDepth);
	}


	return Out;
}

PS_OUT_SKY PS_MAIN_SKY(PS_IN In)
{
	PS_OUT_SKY		Out = (PS_OUT_SKY)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse.a = ComputeDepthBlur(In.fDepth);

	return Out;
}

PS_OUT PS_MAIN_VENDINGMACHINE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, In.fDepth / g_fFar, 1.0f);

	if (g_iIsGlow != 0)
	{
		Out.vGlow = Out.vDiffuse;
	}

	float fRim = In.vRim.x;

	float3 rimColor = g_vRimLightColor;
	rimColor *= fRim;
	Out.vRimLight = vector(rimColor, 0.f);
	Out.vGlow += Out.vRimLight;

	if (Out.vDiffuse.a <= 0.01f)
	{
		discard;
	}
	else
	{
		Out.vDiffuse.a = ComputeDepthBlur(In.fDepth);
	}

	if (g_isActive == false)
	{
		Out.vDiffuse.rgb = (Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f;
	}

	return Out;
}

struct VS_PLAYER_IN {
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_PLAYER_OUT {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vRim : TEXCOORD3;
	float4		vDir : TEXCOORD4;
};

struct PS_PLAYER_IN {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vRim : TEXCOORD3;
	float4		vDir : TEXCOORD4;
};

struct PS_PLAYER_OUT {
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vGlow : SV_TARGET3;
	vector		vRimLight : SV_TARGET4;
};

VS_PLAYER_OUT VS_MAIN_ANIM_PLAYER(VS_PLAYER_IN In)
{
	VS_PLAYER_OUT		Out = (VS_PLAYER_OUT)0;

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
	Out.vNormal = normalize(mul(vNormal, g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vPosition, g_matWorld);
	Out.vProjPos = Out.vPosition;

	// RimLight
	vector ToCameraVec = vector(g_vCamPos - Out.vWorldPos);
	Out.vDir = normalize(ToCameraVec);

	ToCameraVec.a = 0.f;
	ToCameraVec = normalize(ToCameraVec);
	float fRim = saturate(dot(Out.vNormal, ToCameraVec));
	//if (fRim > 0.7f)
	//{
	//	Out.vRim.rgba = fRim;
	//}
	Out.vRim.rgba = fRim + 0.1f;

	return Out;
}


PS_PLAYER_OUT PS_PLAYER(PS_PLAYER_IN In)
{
	PS_PLAYER_OUT		Out = (PS_PLAYER_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 1.0f);


	float fRim = In.vRim.x;
	float3 rimColor = g_vRimLightColor;
	rimColor *= fRim;
	Out.vRimLight = vector(0.f,0.f, 0.f, 0.f);
	Out.vGlow += Out.vRimLight;

	vector vLook = normalize(In.vDir);
	float fRimLight = 1.f - saturate(dot(normalize(In.vNormal), vLook));
	vector vRimLightColor = vector(fRimLight, fRimLight, fRimLight, 1.f);

	vector vAdditionalColor = vector(0.85f, 0.65f, 0.12f, 1.f);


	
	Out.vDiffuse.a = fAlpha;

	if(Out.vDiffuse.a == 0.1f)
		discard;


	if (Out.vDiffuse.a <= 0.1f)
	{
		Out.vDiffuse = 0.f;
	}

	return Out;
}

PS_OUT PS_ROBOT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, In.fDepth / g_fFar, 1.0f);
	//Out.vGlow = In.vWorldPos;
	vector vAlphaOne = g_texAlphaOne.Sample(DefaultSampler, In.vTexUV);
	vector vColor = g_texColor.Sample(DefaultSampler, In.vTexUV);

	vAlphaOne.rgb = vAlphaOne.rgb * 0.5f + vColor.rgb * 0.5f;
	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 0.5f + vAlphaOne.rgb * 0.5f;


	if (Out.vDiffuse.a <= 0.1f)
		discard;

	return Out;
}


PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, In.fDepth / g_fFar, 1.0f);
	if (g_iIsGlow)
	{
		Out.vGlow = Out.vDiffuse;
	}

	//if (Out.vDiffuse.a <= 0.1f)
	//	discard;

	Out.vDiffuse.a = fAlpha;

	return Out;
}



PS_OUT PS_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, In.fDepth / g_fFar, 1.0f);
	//Out.vGlow = In.vWorldPos;


	vector vDissolve = g_texDissolve.Sample(DefaultSampler, In.vTexUV);

	if (Out.vDiffuse.a <= 0.1f)
		discard;


	if (vDissolve.r < g_fDissolve)
		discard;

	//if (vDissolve.r < g_fDissolve)
	//	Out.vDiffuse.a = 1.f;
	//else
	//	Out.vDiffuse.a = 0.f;

	if (g_fDissolve >= vDissolve.r - 0.09f && g_fDissolve <= vDissolve.r + 0.09)
		Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);

	if (g_fDissolve >= vDissolve.r - 0.05f && g_fDissolve <= vDissolve.r + 0.05)
		Out.vDiffuse = vector(1.f, 1.f, 0.f, 1.f);

	if (g_fDissolve >= vDissolve.r - 0.01f && g_fDissolve <= vDissolve.r + 0.01)
		Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);


	return Out;
}


PS_OUT PS_WEAPON_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, In.fDepth / g_fFar, 1.0f);
	Out.vGlow = Out.vDiffuse;


	vector vDissolve = g_texDissolve.Sample(DefaultSampler, In.vTexUV);

	if (Out.vDiffuse.a <= 0.1f)
		discard;


	if (vDissolve.r < g_fDissolve)
		discard;

	//if (vDissolve.r < g_fDissolve)
	//	Out.vDiffuse.a = 1.f;
	//else
	//	Out.vDiffuse.a = 0.f;

	if (g_fDissolve >= vDissolve.r - 0.4f && g_fDissolve <= vDissolve.r + 0.4)
		Out.vDiffuse = vector(1.f, 0.7f, 0.3f, 1.f);

	if (g_fDissolve >= vDissolve.r - 0.2f && g_fDissolve <= vDissolve.r + 0.2)
		Out.vDiffuse = vector(1.f, 1.f, 0.5f, 1.f);

	if (g_fDissolve >= vDissolve.r - 0.1f && g_fDissolve <= vDissolve.r + 0.1)
		Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);

	Out.vGlow = Out.vDiffuse;

	return Out;
}


// Deferred ////////////////////////////////////////////////////

// Light Depth /////////////////////////////////////////////////

struct VS_OUT_SHADOW_DEPTH {
	float4		vPosition : SV_POSITION;
	float4		vLightProjPos : TEXCOORD0;
};

texture2D	g_texDepth;


VS_OUT_SHADOW_DEPTH VS_MAIN_NONANIM_SHADOW_DEPTH(VS_IN In)
{
	VS_OUT_SHADOW_DEPTH		Out = (VS_OUT_SHADOW_DEPTH)0;

	matrix		matWV, matWVP;
	
	matWV = mul(g_matWorld, g_matLightView);
	matWVP = mul(matWV, g_matLightProj);
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vLightProjPos = Out.vPosition;
	
	return Out;
}


VS_OUT_SHADOW_DEPTH VS_MAIN_ANIM_SHADOW_DEPTH(VS_IN In)
{
	VS_OUT_SHADOW_DEPTH		Out = (VS_OUT_SHADOW_DEPTH)0;

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		matBone = g_tBoneMatrices.matBones[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_tBoneMatrices.matBones[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_tBoneMatrices.matBones[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_tBoneMatrices.matBones[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), matBone);

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matLightView);
	matWVP = mul(matWV, g_matLightProj);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vLightProjPos = Out.vPosition;

	return Out;
}

struct PS_IN_SHADOW_DEPTH {
	float4		vPosition : SV_POSITION;
	float4		vLightProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW_DEPTH {
	vector		vShadowDepth : SV_TARGET0;
};


PS_OUT_SHADOW_DEPTH PS_MAIN_SHADOW_DEPTH(PS_IN_SHADOW_DEPTH In)
{
	PS_OUT_SHADOW_DEPTH		Out = (PS_OUT_SHADOW_DEPTH)0;

	Out.vShadowDepth.rgb = In.vLightProjPos.w / g_fFar;

	Out.vShadowDepth.a = 1.f;

	return Out;
}
////////////////////////////////////////////////////////////////////

// Motion Blur

struct VS_OUT_BLUR {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vVelocity : TEXCOORD1;
};

VS_OUT_BLUR VS_MAIN_ANIM_BLUR(VS_IN In)
{
	VS_OUT_BLUR		Out = (VS_OUT_BLUR)0;

	/* 애니메이션 재생에 따른 상태변환 행렬.
	로컬 -> 로컬 */
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		matBone = g_tBoneMatrices.matBones[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_tBoneMatrices.matBones[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_tBoneMatrices.matBones[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_tBoneMatrices.matBones[In.vBlendIndex.w] * fWeightW;

	matrix		matPreBone = g_tPreBoneMatrices.matBones[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_tPreBoneMatrices.matBones[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_tPreBoneMatrices.matBones[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_tPreBoneMatrices.matBones[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), matBone);
	vector		vNormal = mul(vector(In.vNormal, 0.f), matBone);
	vector		vPrePosition = mul(vector(In.vPosition, 1.f), matPreBone);
	vector		vPreNormal = mul(vector(In.vNormal, 0.f), matPreBone);
	
	matrix		matWV, matWVP;
	matrix		matOldWV, matOldWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	matOldWV = mul(g_matOldWorld, g_matOldView);
	matOldWVP = mul(matOldWV, g_matProj);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_matWorld));
	

	Out.vTexUV = In.vTexUV;

	float4	vNewPos = Out.vPosition;
	float4	vOldPos = mul(vPrePosition, matOldWVP);

	float4	vBlurDir = normalize(vNewPos - vOldPos);

	float	a = dot(vBlurDir, Out.vNormal);

	Out.vPosition = a > 0 ? vNewPos : vOldPos;

	float2	velocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);

	Out.vVelocity.xy = velocity * 0.5f;
	Out.vVelocity.y *= -1.f;

	Out.vVelocity.z = Out.vPosition.z;
	Out.vVelocity.w = Out.vPosition.w;

	return Out;
}


struct PS_IN_BLUR {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vVelocity : TEXCOORD1;
};

struct PS_OUT_BLUR {
	vector		vBlur : SV_TARGET0;
};

PS_OUT_BLUR PS_MAIN_BLUR(PS_IN_BLUR In)
{
	PS_OUT_BLUR		Out = (PS_OUT_BLUR)0;

	Out.vBlur.xy = In.vVelocity.xy;
	Out.vBlur.z = 1.f;
	Out.vBlur.w = In.vVelocity.z / In.vVelocity.w;

	return Out;
}


struct VS_OUT_RIMDODGE {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vDir : TEXCOORD1;
};


VS_OUT_RIMDODGE VS_MAIN_RIMDODGE(VS_IN In)
{
	VS_OUT_RIMDODGE		Out = (VS_OUT_RIMDODGE)0;

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
	Out.vNormal = normalize(mul(vNormal, g_matWorld));
	Out.vTexUV = In.vTexUV;

	vector vWorldPos = mul(vPosition, g_matWorld);

	// RimLight
	vector ToCameraVec = vector(g_vCamPos - vWorldPos);
	Out.vDir = normalize(ToCameraVec);

	return Out;
}


struct PS_IN_RIMDODGE {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vDir : TEXCOORD1;
};

struct PS_OUT_RIMDODGE {
	vector		vRimDodge : SV_TARGET0;
};

PS_OUT_RIMDODGE PS_MAIN_RIMDODGE(PS_IN_RIMDODGE In)
{
	PS_OUT_RIMDODGE		Out = (PS_OUT_RIMDODGE)0;

	vector vLook = normalize(In.vDir);
	float fRimLight = 1.f - saturate(dot(normalize(In.vNormal), vLook));
	vector vRimLightColor = vector(fRimLight, fRimLight, fRimLight, 1.f);

	vector vAdditionalColor = vector(0.85f, 0.65f, 0.12f, 1.f);

	//Out.vRimDodge = (vRimLightColor + (vAdditionalColor / 3.f));
	Out.vRimDodge = (vRimLightColor);

	Out.vRimDodge.a = 1.f - fAlpha;
	Out.vRimDodge.rgb *= Out.vRimDodge.a;

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

	pass NonAnim_NoneCull {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NONANIM();
	}

	pass NonAnim_Alpha {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	pass Anim_Alpha {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	pass NonAnim_WireFrame {
		SetRasterizerState(RS_WireFrame);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Anim_WireFrame {
		SetRasterizerState(RS_WireFrame);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Anim_Player {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM_PLAYER();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PLAYER();
	}

	pass Anim_Robot {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ROBOT();
	}

	pass NonAnim_ShadowDepth {
		SetRasterizerState(RS_CounterClockWise);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM_SHADOW_DEPTH();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW_DEPTH();
	}

	pass Anim_ShadowDepth {
		SetRasterizerState(RS_CounterClockWise);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM_SHADOW_DEPTH();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW_DEPTH();
	}

	pass NONANIM_DISSOLVE {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_NONANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}

	pass ANIM_DISSOLVE {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}


	pass Sky {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN_SKY();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SKY();
	}

	pass MotionBlur {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM_BLUR();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	pass RimDodge {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_RIMDODGE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RIMDODGE();
	}

	pass VendingMachine {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_VENDINGMACHINE();
	}

	pass WeaponDissolve {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_WEAPON_DISSOLVE();
	}

};
