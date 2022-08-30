cbuffer LightDesc {
	vector		g_vLightDir;
	vector		g_vLightDiffuse;
	vector		g_vLightAmbient;
	vector		g_vLightSpecular;

	vector		g_vLightPosition;
	float		g_fRange;
	float		g_fFar;
};

cbuffer MaterialDesc {
	vector		g_vMtrlDiffuse = (vector)1.f;
	vector		g_vMtrlAmbient = (vector)1.f;
	vector		g_vMtrlSpecular = (vector)1.f;
	vector		g_vMtrlEmissive = (vector)1.f;
	float		g_fPower = 30.0f;
};

cbuffer CameraDesc {
	vector		g_vCamPos;
};

cbuffer InverseMat {
	matrix		g_matProjInv;
	matrix		g_matViewInv;
};


texture2D		g_texDiffuse;
texture2D		g_texNormal;
texture2D		g_texDepth;
texture2D		g_texShade;
texture2D		g_texSpecular;

//////////////////////////////////////////////////////////////////
// For Shadow

cbuffer LightMatrix
{
	matrix		g_matLightView;
	matrix		g_matLightProj;
}
float			g_fLightFar;
float			g_fScreenWidth;
texture2D		g_texShadowDepth0;
texture2D		g_texPosition;
////////////////////////////////////////////////////////////////////

///////////////////////////
// For PostProcessing


texture2D		g_texFilter0;
texture2D		g_texFilter1;

float			g_fSourceRate;
float			g_fOtherRate;

//////////////////////////////

sampler DefaultSampler = sampler_state {

};



struct VS_IN {
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT {
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


struct PS_IN {
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT {
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
	vector		vShadowDepth0 : SV_TARGET2;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTION(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_texNormal.Sample(DefaultSampler, In.vTexUV);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector		vDepthDesc = g_texDepth.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * g_fFar;
	vector		vWorld;

	/* In Projection Space Pos (World Pos * View Matrix * Proj Matrix / View's Z */
	vWorld.x = In.vTexUV.x *  2.f - 1.f;
	vWorld.y = In.vTexUV.y * -2.f + 1.f;
	vWorld.z = vDepthDesc.x;
	vWorld.w = 1.f;

	/* In View Space Pos (World Pos * View Matrix * Proj Matrix) ( = * View's Z ) */
	vWorld = vWorld * fViewZ;

	/* In View Space Pos (World Pos * View Matrix) ( = * Inverse Proj Matrix ) */
	vWorld = mul(vWorld, g_matProjInv);

	/* In World Space Pos (World Pos) ( = * Inverse View Matrix ) */
	vWorld = mul(vWorld, g_matViewInv);

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal))
		+ (g_vLightAmbient * g_vMtrlAmbient));

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vLook = normalize(vWorld - g_vCamPos);

	Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);


	//// Caculate with Light's World, View, Proj
	//vector vLightWVP = In.vPosition;
	//vLightWVP = mul(vLightWVP, g_matLightView);
	//vLightWVP = mul(vLightWVP, g_matLightProj);

	////Out.vShadowDepth0 = vector(vLightWVP.z / vLightWVP.w, vLightWVP.w / g_fLightFar, 0.0f, 1.0f);
	////Out.vShadowDepth0 = vector(0.f, vLightWVP.w / g_fLightFar, 0.0f, 1.0f);
	//Out.vShadowDepth0 = vector(vLightWVP.z / vLightWVP.w, 0.f, 0.0f, 0.0f);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_texNormal.Sample(DefaultSampler, In.vTexUV);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector		vDepthDesc = g_texDepth.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * g_fFar;
	vector		vWorld;

	/* In Projection Space Pos (World Pos * View Matrix * Proj Matrix / View's Z */
	vWorld.x = In.vTexUV.x * 2.f - 1.f;
	vWorld.y = In.vTexUV.y * -2.f + 1.f;
	vWorld.z = vDepthDesc.x;
	vWorld.w = 1.f;

	/* In View Space Pos (World Pos * View Matrix * Proj Matrix) ( = * View's Z ) */
	vWorld = vWorld * fViewZ;

	/* In View Space Pos (World Pos * View Matrix) ( = * Inverse Proj Matrix ) */
	vWorld = mul(vWorld, g_matProjInv);

	/* In World Space Pos (World Pos) ( = * Inverse View Matrix ) */
	vWorld = mul(vWorld, g_matViewInv);



	vector		vLightDir = vWorld - g_vLightPosition;
	float		fLength = length(vLightDir);
	float		fAtt = saturate((g_fRange - fLength) / g_fRange);

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal))
		+ (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	vector		vReflect = reflect(normalize(vLightDir), vNormal);
	vector		vLook = normalize(vWorld - g_vCamPos);

	Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower)
		* (g_vLightSpecular * g_vMtrlSpecular) * fAtt;

	return Out;
}


PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector		vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_texShade.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_texSpecular.Sample(DefaultSampler, In.vTexUV);
	vector		vDepth = g_texDepth.Sample(DefaultSampler, In.vTexUV);





	//////////////////////////////////
	// Shadow
	//vector		vShadowDepth0 = g_texShadowDepth0.Sample(DefaultSampler, In.vTexUV);
	//vector		vWorldPosition = g_texPosition.Sample(DefaultSampler, In.vTexUV);
	//vWorldPosition.rgb = vWorldPosition.rgb * 10000.f - 1000.f;
	//vWorldPosition.a = 1.f;
	//float4 vWorldPos = vWorldPosition;										// PositionTarget으로부터 World좌표를 구해온다.
	//float4 vShadowProj = mul(vWorldPos, g_matLightProj);					// Light 시점으로 투영시킨 좌표 구하기
	//float fDepth = vShadowProj.z / vShadowProj.w;							// w 로 나눠서 실제 투영좌표 z 값을 구한다.(올바르게 비교하기 위해서)

	// 계산된 투영좌표를 UV 좌표로 변경해서 ShadowMap 에 기록된 깊이값을 꺼내온다.
	//float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
	//						, (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);

	//if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f
	//	&& 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
	//{
	//	float fShadowDepth = vShadowDepth0.r;

	//	// 그림자인 경우 빛을 약화시킨다.
	//	if (fShadowDepth != 0.f && (fDepth > fShadowDepth + 0.00001f))
	//	{
	//		vDiffuse *= 0.1f;
	//		vSpecular = (float4) 0.f;
	//	}
	//}
	////////////////////////////////////////////////


	Out.vColor = (vDiffuse * vShade) + vSpecular;


	float	fViewZ = vDepth.x * g_fLightFar;

	vector vPosition;


	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	//vPosition.z = vDepth.x * 100.f;
	vPosition.z = vDepth.x * fViewZ;
	vPosition.w = fViewZ;

	// 뷰 상
	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);
	vPosition = mul(vPosition, g_matLightView);

	vector		vUVPos = mul(vPosition, g_matLightProj);
	float2		vNewUV;

	vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
	vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

	vector	vShaderDepth0 = g_texShadowDepth0.Sample(DefaultSampler, vNewUV);

	if (vPosition.z - 0.1f > vShaderDepth0.r * g_fLightFar)
		Out.vColor.rgb = Out.vColor.rgb * 0.1f;



	return Out;
}

PS_OUT PS_POSTPROCESSING_BEFOREUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector		vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	vector		vFilter0 = g_texFilter0.Sample(DefaultSampler, In.vTexUV);

	vector		vMixed = (vector)0.f;
	float2		vUV = In.vTexUV;

	float		fOriginTexel = 1.f / g_fScreenWidth;
	float		fTexel = fOriginTexel;


	fTexel = fOriginTexel * 7.f;
	if (In.vTexUV.x > 0.1f && In.vTexUV.x < 0.9f
		&& In.vTexUV.y > 0.1f && In.vTexUV.y < 0.9f)
	{
		fTexel = fOriginTexel * 5.f;
		if (In.vTexUV.x > 0.2f && In.vTexUV.x < 0.8f
			&& In.vTexUV.y > 0.2f && In.vTexUV.y < 0.8f)
		{
			fTexel = fOriginTexel * 3.f;
			if (In.vTexUV.x > 0.4f && In.vTexUV.x < 0.6f
				&& In.vTexUV.y > 0.3f && In.vTexUV.y < 0.7f)
			{
				fTexel = fOriginTexel * 1.f;
			}
		}

	}



	//	AAOAA
	//	AOOOA
	//	OOOOO
	//	AOOOA
	//	AAOAA

	///////////////////////////////////////////
	vUV.y -= fTexel * 2.f;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	////////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= fTexel; vUV.x -= fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	////////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= fTexel * 2.f;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	///////////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += fTexel; vUV.x -= fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vUV.x += fTexel;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	///////////////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += fTexel * 2.f;
	vMixed += g_texDiffuse.Sample(DefaultSampler, vUV);

	vMixed.rgb /= 13.f;


	Out.vColor.rgb = vMixed.rgb * g_fSourceRate + vFilter0.rgb * g_fOtherRate;
	Out.vColor.a = vDiffuse.a;

	return Out;

}

PS_OUT PS_POSTPROCESSING_AFTERUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector		vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	vector		vFilter0 = g_texFilter0.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.rgb = (vDiffuse.rgb + vFilter0.rgb) * 0.5f;
	Out.vColor.a = vDiffuse.a;

	return Out;
}


RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockWise = false;
};

BlendState BS_None {
	BlendEnable[0] = false;
};

BlendState BS_Add {
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};

DepthStencilState DSS_Priority {
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};


technique11		DefaultTechnique {
	pass Deffered {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Dir {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTION();
	}

	pass Light_Point {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}

	pass Blend {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass PostProcessing_BeforeUI_Focus {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI();
	}

	pass PostProcessing_BeforeUI_Filter {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI();
	}

	pass PostProcessing_BeforeUI_Focus_Filter {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI();
	}

	pass PostProcessing_AfterUI {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_AFTERUI();
	}

};