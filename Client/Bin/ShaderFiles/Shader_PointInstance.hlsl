#include "Shader_Defines.hlsli"

texture2D	g_texDiffuse;
cbuffer CameraDesc
{
	vector		g_vCamPosition;
};

cbuffer IsGlowBuffer
{
	int			g_iIsGlow;
	float3		IsGlowPadding;
};

cbuffer IsDistortionBuffer
{
	int			g_iIsDistortion;
	float3		IsDistortionPadding;
};
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
	float		fPSize : PSIZE;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;	
	float4		vUV : TEXCOORD5;
	float4		vFade : TEXCOORD6;
	float4		vDirection : TEXCOORD7;
};


struct VS_OUT
{
	float4		vPosition	: POSITION;
	float4		vUV			: TEXCOORD0;
	float4		vFade		: TEXCOORD1;
	float4		vDirection	: TEXCOORD2;
	float		fPSize		: PSIZE;
	float4		vProjPos	: TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
	vPosition = mul(vPosition, g_matWorld);

	Out.vPosition = vPosition;
	Out.vUV = In.vUV;
	Out.vFade = In.vFade;
	Out.vDirection = In.vDirection;
	Out.fPSize = In.fPSize;
	Out.vProjPos = Out.vPosition;
	return Out;
}

struct GS_IN
{
	float4		vPosition	: POSITION;
	float4		vUV			: TEXCOORD0;
	float4		vFade		: TEXCOORD1;
	float4		vDirection	: TEXCOORD2;
	float		fPSize		: PSIZE;
	float4		vProjPos	: TEXCOORD3;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vUV			: TEXCOORD0;
	float4		vFade		: TEXCOORD1;
	float2		vTexUV		: TEXCOORD2;
	float4		vProjPos	: TEXCOORD3;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[4];

	vector		vLook = g_vCamPosition - In[0].vPosition;
	vector		vRight;

	vector		vTempUp = mul(In[0].vDirection, g_matView);
	vTempUp.zw = 0.f;
	if (In[0].vDirection.x < 0.1f
		&& In[0].vDirection.y < 0.1f
		&& In[0].vDirection.z < 0.1f)
	{
		vRight = vector(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].fPSize * 0.5f, 0.f);
	}
	else
	{
		vRight = vector(normalize(cross(float3(In[0].vDirection.xyz), vLook.xyz)) * In[0].fPSize * 0.5f, 0.f);
	}



	vRight = vector(normalize(cross(float3(vTempUp.xyz), vLook.xyz)) * In[0].fPSize * 0.5f, 0.f);

	//vector		vUp = vector(normalize(cross(vLook.xyz, vRight.xyz)) * In[0].fPSize * 0.5f, 0.f);
	vector		vUp = vector(normalize(vTempUp.xyz) * In[0].fPSize * 0.5f, 0.f);

	matrix		matVP;
	matVP = mul(g_matView, g_matProj);

	Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matVP);
	//Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].vTexUV = float2(0.f, 1.f);
	Out[0].vUV = In[0].vUV;
	Out[0].vFade = In[0].vFade;
	Out[0].vProjPos = In[0].vProjPos;

	Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matVP);
	//Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].vTexUV = float2(0.f, 0.f);
	Out[1].vUV = In[0].vUV;
	Out[1].vFade = In[0].vFade;
	Out[1].vProjPos = In[0].vProjPos;

	Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matVP);
	//Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].vTexUV = float2(1.f, 0.f);
	Out[2].vUV = In[0].vUV;
	Out[2].vFade = In[0].vFade;
	Out[2].vProjPos = In[0].vProjPos;

	Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matVP);
	//Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].vTexUV = float2(1.f, 1.f);
	Out[3].vUV = In[0].vUV;
	Out[3].vFade = In[0].vFade;
	Out[3].vProjPos = In[0].vProjPos;

	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);

	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);

	OutStream.RestartStrip();


}

[maxvertexcount(36)]
void GS_TRAIL(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[14];

	vector		vLook = vector(normalize(g_vCamPosition - In[0].vPosition));

	//vector vRight = vector(normalize(cross(float3(In[0].vDirection.xyz), vLook.xyz)) * In[0].fPSize * 0.5f, 0.f);
	vector vRight = vector(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].fPSize * 0.5f, 0.f);
	vector vUp = vector(normalize(cross(vLook.xyz, vRight.xyz)) * In[0].fPSize * 0.5f, 0.f);

	matrix		matVP;
	matVP = mul(g_matView, g_matProj);


	float4 vOffset = (In[0].vDirection * In[0].fPSize);
	vOffset.w = 0;

	float4 OffsetPosition;
	for (int i = 0; i < 6 + 1; ++i)
	{

		OffsetPosition = In[0].vPosition;
		OffsetPosition -= vOffset * i;

		int iIndex = i * 2;
		Out[iIndex + 0].vPosition = mul(OffsetPosition - vRight - vUp, matVP);
		Out[iIndex + 0].vTexUV = float2(1.f / (6.f + 1.f) * i, 1.f);
		Out[iIndex + 0].vUV = In[0].vUV;
		Out[iIndex + 0].vFade = In[0].vFade;
		Out[iIndex + 0].vProjPos = Out[iIndex + 0].vPosition;

		Out[iIndex + 1].vPosition = mul(OffsetPosition - vRight + vUp, matVP);
		Out[iIndex + 1].vTexUV = float2(1.f / (6.f + 1.f) * i, 0.f);
		Out[iIndex + 1].vUV = In[0].vUV;
		Out[iIndex + 1].vFade = In[0].vFade;
		Out[iIndex + 1].vProjPos = Out[iIndex + 1].vPosition;
	}


	for (int j = 0; j < 6; ++j)
	{
		int iIndex = j * 2;

		OutStream.Append(Out[j + 0]);
		OutStream.Append(Out[j + 1]);
		OutStream.Append(Out[j + 3]);

		OutStream.RestartStrip();

		OutStream.Append(Out[j + 0]);
		OutStream.Append(Out[j + 3]);
		OutStream.Append(Out[j + 2]);

		OutStream.RestartStrip();
	}
}

struct PS_IN
{
	float4		vPosition	: SV_POSITION;
	float4		vUV			: TEXCOORD0;
	float4		vFade		: TEXCOORD1;
	float2		vTexUV		: TEXCOORD2;
	float4		vProjPos	: TEXCOORD3;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;	
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 ResultUV = (float2)0;
	ResultUV.x = In.vUV.x + In.vUV.z * In.vTexUV.x;
	ResultUV.y = In.vUV.y + In.vUV.w * In.vTexUV.y;
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, ResultUV);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	// Fade In
	if (In.vFade.x)
	{
		if (In.vFade.y < Out.vColor.a)
		{
			Out.vColor.a = In.vFade.y;
		}
	}

	// Fade In
	if (In.vFade.z)
	{
		if (In.vFade.w < Out.vColor.a)
		{
			Out.vColor.a = In.vFade.w;
		}
	}


	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

struct PS_GLOW_OUT
{
	vector		vColor : SV_TARGET0;
	vector		vGlow : SV_TARGET1;
	vector		vDistortion : SV_TARGET2;
	vector		vDepth : SV_TARGET3;
};

PS_GLOW_OUT PS_GLOW(PS_IN In)
{
	PS_GLOW_OUT		Out = (PS_GLOW_OUT)0;

	float2 ResultUV = (float2)0;
	ResultUV.x = In.vUV.x + In.vUV.z * In.vTexUV.x;
	ResultUV.y = In.vUV.y + In.vUV.w * In.vTexUV.y;
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, ResultUV);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
	
	// Fade In
	if (In.vFade.x)
	{
		if (In.vFade.y < Out.vColor.a)
		{
			Out.vColor.a = In.vFade.y;
		}
	}

	// Fade In
	if (In.vFade.z)
	{
		if (In.vFade.w < Out.vColor.a)
		{
			Out.vColor.a = In.vFade.w;
		}
	}


	if (Out.vColor.a < 0.001f)
		discard;

	if (0 != g_iIsGlow)
	{
		Out.vGlow = Out.vColor;
	}
	if (0 != g_iIsDistortion)
	{
		Out.vDistortion = Out.vColor;
	}



	return Out;
}



technique11		DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_GLOW();
	}
	pass TrailPass
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_TRAIL();
		PixelShader = compile ps_5_0 PS_GLOW();
	}

};


