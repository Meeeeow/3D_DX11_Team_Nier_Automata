#include "Shader_Defines.hlsli"

texture2D	g_texDefault;
texture2D	g_texAlphaOne;
texture2D	g_texMask;

int3		g_fIsTexture;



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
	float4		vUV	: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
	float4		vUV	: TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = vector(In.vPosition, 1.f);

	Out.vPosition = vPosition;
	Out.fPSize = In.fPSize;
	Out.vUV = In.vUV;

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float		fPSize : PSIZE;
	float4		vUV	: TEXCOORD0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vUV	: TEXCOORD0;
	float2		vTexUV : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[4];

	vector		vLook = g_vCamLook;
	vLook.xyz *= -1.f;
	vector		vRight = vector(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].fPSize * 0.5f, 0.f);
	vector		vUp = vector(normalize(cross(vLook.xyz, vRight.xyz)) * In[0].fPSize * 0.5f, 0.f);

	matrix		matVP, matWVP;
	matVP = mul(g_matView, g_matProj);
	matWVP = mul(g_matWorld, matVP);


	Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matWVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].vUV = In[0].vUV;

	Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matWVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].vUV = In[0].vUV;

	Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matWVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].vUV = In[0].vUV;

	Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matWVP);
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
	vector vAlphaOneColor = (vector)0;
	vector vMaskColor = (vector)0;

	if (g_fIsTexture.x)
		Out.vColor = g_texDefault.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
	{
		Out.vColor.rgb = 0.f;
	}


	if (g_fIsTexture.y)
		vAlphaOneColor = g_texAlphaOne.Sample(DefaultSampler, In.vTexUV);

	if (g_fIsTexture.z)
		vMaskColor = g_texMask.Sample(DefaultSampler, In.vTexUV);

	// if Default Texture is NOT null
	if (g_fIsTexture.x != 0.f)
	{
		// If MaskTexture exists
		if (g_fIsTexture.z != 0.f)
		{
			if (vMaskColor.a == 0.f)
			{
				discard;
			}
		}
		// If MaskTexture exists
		if (g_fIsTexture.y != 0.f)
		{
			if (vAlphaOneColor.a != 0.f)
			{
				Out.vColor.rgb += vAlphaOneColor.rgb;
				Out.vColor.rgb *= 0.5f;
			}
		}
	}
	// else if Default Texture is null
	else
	{
		if (In.vTexUV.x <= 0.02f || In.vTexUV.x >= 0.98f
			|| In.vTexUV.y <= 0.02f || In.vTexUV.y >= 0.98f
			)
		{
			Out.vColor.r = 0.5f;
			Out.vColor.g = 0.f;
			Out.vColor.b = 0.f;
			Out.vColor.a = 0.5f;
		}
		else
		{
			discard;
		}

	}



	return Out;
}

PS_OUT PS_EMPTY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	if (In.vTexUV.x <= 0.02f || In.vTexUV.x >= 0.98f
		|| In.vTexUV.y <= 0.02f || In.vTexUV.y >= 0.98f
		|| In.vTexUV.x == In.vTexUV.y
		)
	{
		Out.vColor.r = 1.f;
		Out.vColor.g = 1.f;
		Out.vColor.b = 0.f;
		Out.vColor.a = 1.f;
	}
	else
	{
		discard;
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
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass EmptyPass
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_EMPTY();
	}
};


