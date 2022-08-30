

cbuffer LightDesc
{
	vector		g_vLightDir;
	vector		g_vLightPosition;
	vector		g_vLightDiffuse;
	vector		g_vLightAmbient;
	vector		g_vLightSpecular;
	float		g_fRange;
};

cbuffer MaterialDesc
{
	vector		g_vMtrlDiffuse = (vector)1.f;
	vector		g_vMtrlAmbient = (vector)1.f;
	vector		g_vMtrlSpecular = (vector)1.f;
	vector		g_vMtrlEmissive = (vector)1.f;
	float		g_fPower = 30.0f;
};

cbuffer InverseMatrices
{
	matrix		g_ProjMatrixInverse;
	matrix		g_ViewMatrixInverse;
};

cbuffer Camera
{
	vector		g_vCamPosition;	
};


texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_ShadeTexture;
texture2D	g_DepthTexture;
texture2D	g_SpecularTexture;

/* 텍스쳐로부터 피셀을 가져오는 방식. */
sampler DefaultSampler = sampler_state
{

};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPosition = vector(In.vPosition, 1.f);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
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

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTION(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * 300.f;
	vector		vWorldPos;

	/* 투영 스페이스 상의 위치.(월드위치 * 뷰행렬 * 투영행렬 / 뷰`s Z) */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰 스페이스 상의 위치.(월드위치 * 뷰행렬 * 투영행렬) */
	vWorldPos = vWorldPos * fViewZ;

	/* 뷰 스페이스 상의 위치.(월드위치 * 뷰행렬) */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드 스페이스 상의 위치.(월드위치) */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);	

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) 
		+ (g_vLightAmbient * g_vMtrlAmbient));

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);
	return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	
	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/* 투영 스페이스 상의 위치.(월드위치 * 뷰행렬 * 투영행렬 / 뷰`s Z) */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰 스페이스 상의 위치.(월드위치 * 뷰행렬 * 투영행렬) */
	vWorldPos = vWorldPos * fViewZ;

	/* 뷰 스페이스 상의 위치.(월드위치 * 뷰행렬) */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드 스페이스 상의 위치.(월드위치) */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	vector		vLightDir = vWorldPos - g_vLightPosition;
	float		fLength = length(vLightDir);

	float		fAtt = saturate(((g_fRange - fLength) / g_fRange) * 3.f);

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	vector		vReflect = reflect(normalize(vLightDir), vNormal);
	vector		vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular) * fAtt;
	return Out;
}


PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = (vDiffuse * vShade) + vSpecular;

	if (vDiffuse.a == 0.0f)
		discard;

	return Out;
}

RasterizerState CullMode_Solid
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

DepthStencilState Depth_Stencil_NonZEnable
{
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};

BlendState Blend_None
{
	BlendEnable[0] = false;
};


BlendState Blend_Add
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};


technique11		DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_NonZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass LightAcc_DIRECTION
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_NonZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTION();
	}

	pass LightAcc_POINT
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_NonZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}

	pass Blend
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_NonZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

};


