#include "Shader_Defines.hlsli"

cbuffer CameraDesc
{
	vector		g_vCamPosition;
};

texture2D	g_texDiffuse;

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
};


struct VS_OUT
{
	float4		vPosition : POSITION;
	float4		vUV			: TEXCOORD;
	float		fPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
	vPosition = mul(vPosition, g_matWorld);

	Out.vPosition = vPosition;
	Out.vUV = In.vUV;
	Out.fPSize = In.fPSize;

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float4		vUV	: TEXCOORD0;
	float		fPSize : PSIZE;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vUV	: TEXCOORD;
	float2		vTexUV : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[4];

	vector		vLook = g_vCamPosition - In[0].vPosition;
	vector		vRight = vector(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].fPSize * 0.5f, 0.f);
	vector		vUp = vector(normalize(cross(vLook.xyz, vRight.xyz)) * In[0].fPSize * 0.5f, 0.f);
	
	matrix		matVP;
	matVP = mul(g_matView, g_matProj);

	Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].vUV = In[0].vUV;

	Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].vUV = In[0].vUV;

	Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].vUV = In[0].vUV;

	Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].vUV = In[0].vUV;

	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);

	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);

	OutStream.RestartStrip();


}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vUV	: TEXCOORD0;
	float2		vTexUV : TEXCOORD1;
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
	//Out.vColor.r = 1.f;
	//Out.vColor.gb = 0.f;

	if (Out.vColor.a < 0.1f)
		discard;

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
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};


