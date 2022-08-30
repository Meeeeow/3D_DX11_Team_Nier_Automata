texture2D		g_texDefault;
texture2D		g_texDiffuse;
texture2D		g_texNormal;
texture2D		g_texDepth;
texture2D		g_texSSAO;
texture2D		g_texShade;
texture2D		g_texSpecular;
texture2D		g_texGlow;
texture2D		g_texGlow_AfterDeferred;
texture2D		g_texGlow_DeferredResult;
texture2D		g_texGlow_Horizontal;
texture2D		g_texRimLight;
texture2D		g_texAlphaGroup;
texture2D		g_texDistortionMap;
texture2D		g_texDepth_AlphaGroup;


texture2D		g_texShadowDepth;
texture2D		g_texPosition;

texture2D		g_texDistortion;
texture2D		g_texMotionBlur;
texture2D		g_texDodge;
texture2D		g_texDiffuseLow;

texture2D		g_texHalf;
bool				g_bHalfRender;
bool				g_bSSAO;

cbuffer LightDesc {
	vector		g_vLightDir;
	vector		g_vLightDiffuse;
	vector		g_vLightAmbient;
	vector		g_vLightSpecular;

	vector		g_vLightPosition;
	float		g_fRange;
	float		g_fFar;

	float		g_fLength;
};

cbuffer SPOT_LIGHT_DESC {
	vector		g_vTargetPosition;
	float			g_fInnerCosine;
	float			g_fOuterCosine;
};

cbuffer MaterialDesc {
	vector		g_vMtrlDiffuse = (vector)1.f;
	vector		g_vMtrlAmbient = (vector)1.f;
	vector		g_vMtrlSpecular = (vector)1.f;
	vector		g_vMtrlEmissive = (vector)1.f;
	float			g_fPower = 30.0f;
};

cbuffer CameraDesc {
	vector		g_vCamPos;
};

cbuffer InverseMat {
	matrix		g_matViewInv;
	matrix		g_matProjInv;
};
//////////////////////////////////////////////////////////////////
// For GodRay
cbuffer tagVolumetricDesc {
	float4		g_vLightShaft = float4(1.f, 0.6f, 0.6f, 1.f);
	/*
	X = Density;
	Y = Decay;
	Z = Weight;
	W = Exposure
	*/
};
//////////////////////////////////////////////////////////////////
// For Shadow

cbuffer LightMatrix
{
	matrix		g_matLightView;
	matrix		g_matLightProj;
	float			g_fLightFar;
};

cbuffer CameraMatrix {
	matrix		g_matView;
	matrix		g_matProj;
};

cbuffer ScreenBuffer
{
	float			g_fScreenWidth;
	float			g_fScreenHeight;
	float2			ScreenBufferPadding;
};

cbuffer FilterBuffer
{
	float			g_fSourceRate;
	float			g_fOtherRate;
	float2			FilterBufferPadding;
};
///////////////////////////
// For MotionBlur
cbuffer tagPostProcessingTask {
	bool		g_bMotionBlur = false;
	bool		g_bDodge = false;
	bool		g_bDistortion = false;
	bool		g_bGrayDistortion = false;

	bool		g_bBeforeUIDistortion = false;
	bool3		g_bPostPadding;

	float		g_fTimeDelta;
	float		g_fPostPadding;
};
///////////////////////////

///////////////////////////
// For Gaussian
static const float	GaussianWeights[13] = {
	0.0561,
	0.1353,
	0.278,
	0.4868,
	0.7261,
	0.9231,
	1,
	0.9231,
	0.7261,
	0.4868,
	0.278,
	0.1353,
	0.0561
};

static const float	GaussianTotal = 6.2108;
///////////////////////////

///////////////////////////
// For Poisson
static const float2	Poisson[8] = {
	float2( 0.0		   ,  0.0        ),
	float2( 0.527837 , -0.085868),
	float2(-0.040088 ,  0.536087),
	float2(-0.670445 , -0.179949),
	float2(-0.419418 , -0.616039),
	float2( 0.440453 , -0.639399),
	float2(-0.757088 ,  0.349334),
	float2( 0.574619 ,  0.685879)
};
///////////////////////////

///////////////////////////
// For DOF
cbuffer DOFDesc {
	float4	vDofParams;
	float2	vPixelSizeHigh;
	float2	vPixelSizeLow;

	float2	vMaxCoC = float2(7.f, 10.f);
	// x = Min, y = Max
	float	fRadiusScale = 0.4f;

	float	fDofDistance;
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
		f = clamp(f, 0, vDofParams.w);
	}

	return f * 0.5f + 0.5f;
}
///////////////////////////

///////////////////////////
// For SSAO
static const float3 AOKernel[8] = {
	normalize(float3(1,  1,  1)) * 0.125f,
	normalize(float3(-1, -1, -1)) * 0.25f,
	normalize(float3(-1, -1,  1)) * 0.375f,
	normalize(float3(-1,  1, -1)) * 0.5f,
	normalize(float3(-1,  1,  1)) * 0.625f,
	normalize(float3(1, -1, -1)) * 0.75f,
	normalize(float3(1, -1,  1)) * 0.875f,
	normalize(float3(1,  1, -1)) * 1.f,
};
static const float3 Random[16] =
{
	float3( 0.2024537f,  0.841204f , -0.9060141f),
	float3(-0.2200423f,  0.6282339f, -0.8275437f),
	float3( 0.3677659f,  0.1086345f, -0.4466777f),
	float3( 0.8775856f,  0.4617546f, -0.6427765f),
	float3( 0.7867433f, -0.141479f , -0.1567597f),
	float3( 0.4839356f, -0.8253108f, -0.1563844f),
	float3( 0.4401554f, -0.4228428f, -0.3300118f),
	float3( 0.0019193f, -0.8048455f,  0.0726584f),
	float3(-0.7578573f, -0.5583301f,  0.2347527f),
	float3(-0.4540417f, -0.252365f ,  0.0694318f),
	float3(-0.0483353f, -0.2527294f,  0.5924745f),
	float3(-0.4192392f,  0.2084218f, -0.3672943f),
	float3(-0.8433938f,  0.1451271f,  0.2202872f),
	float3(-0.4037157f, -0.8263387f,  0.4698132f),
	float3(-0.6657394f,  0.6298575f,  0.6342437f),
	float3(-0.0001783f,  0.2834622f,  0.8343929f),
};

float3	Get_RandomNormal(float2 vTexUV)
{
	float	NoiseX = frac(sin(dot(vTexUV, float2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f);
	float	NoiseY = frac(sin(dot(vTexUV, float2(11.9899f, 62.223f) * 2.0f)) * 34748.34744f);
	float	NoiseZ = frac(sin(dot(vTexUV, float2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f);

	return normalize(float3(NoiseX, NoiseY, NoiseZ));
}

///////////////////////////

///////////////////////////
// For Tone Mapping
cbuffer tagToneMap {
	int		ToneMappingType;
	float3	ToneMapPadding;
};

static const float Gamma = 2.2f;
static const float MiddleGray = 0.18f;
static const float WhiteCutOff = 0.9f;

static const float3x3	ACESInputMat = {
	{ 0.59719, 0.35458, 0.04823 },
	{ 0.07600, 0.90834, 0.01566 },
	{ 0.02840, 0.13383, 0.83777 }
};

static const float3x3	ACESOutputMat =
{
	{ 1.60475, -0.53108, -0.07367 },
	{ -0.10208,  1.10813, -0.00605 },
	{ -0.00327, -0.07276,  1.07602 }
};

float3	RRTAndODTFit(float3 vColor)
{
	float3	a = vColor * (vColor + 0.0245786f) - 0.000090537f;
	float3	b = vColor * (0.983729f * vColor + 0.4329510f) + 0.238081f;

	return a / b;
}

float	ComputeLuminance(float3 vColor)
{
	return dot(vColor, float3(0.2126f, 0.7152f, 0.0722f));
	//return sqrt(dot(vColor, float3(0.299f, 0.587f, 0.114f)));
}

float3	DirectBasedToneMap(float3 vColor)
{
	vColor = pow(abs(vColor), Gamma);
	vColor *= MiddleGray / (0.08f + 0.001f);
	vColor *= (1.f + (vColor / (WhiteCutOff * WhiteCutOff)));
	vColor /= (1.f + vColor);
	vColor = pow(vColor, 1.f / Gamma);
	return vColor;
}

float3	LuminanceBasedReinhardToneMap(float3 vColor)
{
	vColor = pow(abs(vColor), Gamma);
	float	Luma = ComputeLuminance(vColor.rgb);

	float	toneMappedLuma = Luma / (1.f + Luma);
	vColor *= toneMappedLuma / Luma;

	vColor = pow(vColor, 1.f / Gamma);

	return vColor;
}

float3	Uncharted2ToneMap(float3 vColor)
{
	vColor = pow(vColor, Gamma);
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	vColor = ((vColor * (A * vColor + C * B) + D * E) / (vColor * (A * vColor + B) + D * F)) - E / F;
	return pow(vColor, 1.f/Gamma);
}

float3	Uncharted2ToneMap2(float3 vColor)
{
	vColor = pow(abs(vColor), Gamma);

	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float	Exposure_Bias = 2.f;

	vColor *= Exposure_Bias;
	vColor = ((vColor * (A * vColor + C * B) + D * E) / (vColor * (A * vColor + B) + D * F)) - E / F;

	float	White = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	vColor /= White;

	return pow(abs(vColor), 1.f / Gamma);
}

float3	FilmicToneMap(float3 vColor)
{
	vColor = max(0, vColor - 0.004f);

	vColor = pow(vColor, Gamma);

	vColor = (vColor * (6.2f * vColor + 0.5f)) / (vColor *(6.2f * vColor + 1.7f) + 0.06f);

	return pow(vColor, 1.f / Gamma);
}

float3	RomBinDaHouseToneMap(float3 vColor)
{
	vColor = pow(abs(vColor), Gamma);
	
	vColor = exp(-1.0 / (2.72 * vColor + 0.15f));
	vColor = pow(abs(vColor), 1.f / Gamma);
	return vColor;
}

float3	ACES(float3 vColor)
{
	vColor = mul(ACESInputMat, vColor);
	vColor = RRTAndODTFit(vColor);
	vColor = mul(ACESOutputMat, vColor);
	vColor = saturate(vColor);

	return vColor;
}

float3 ACES2(float3 vColor)
{
	float A = 2.51f;
	float B = 0.03f;
	float C = 2.43f;
	float D = 0.59f;
	float E = 0.14f;

	return saturate((vColor * (A * vColor + B)) / (vColor *(C * vColor + D) + E));
}

///////////////////////////

////////////////////////////////////////////////////////////////////

///////////////////////////
// For PostProcessing
texture2D		g_texFilter0;
texture2D		g_texFilter1;
//////////////////////////////

sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler DepthSampler = sampler_state {
	filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

sampler PointSampler = sampler_state {
	filter = min_mag_mip_point;
	AddressU = clamp;
	AddressV = clamp;
};

sampler ClampSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
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

struct PS_OUT_GLOW {
	vector		vColor	: SV_TARGET0;
	vector		vColor2 : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

struct tagSSAO {
	float2	vUV;
	float	fDepth;
	float	fViewZ;
	float3	vNormal;
};

float4	Get_SSAO(tagSSAO In)
{
	float4 vAmbient = (float4)0;

	float3	vRay;
	float3	vReflect;
	float2	vRandomUV;
	float	fOccNorm;

	int		iColor = 0;

	[unroll]
	for (int i = 0; i < 16; i++)
	{
		vRay = reflect(Get_RandomNormal(In.vUV), Random[i]);
		vReflect = normalize(reflect(vRay, In.vNormal)) * 0.001f;
		vReflect.x *= -1.f;
		vRandomUV = In.vUV + vReflect.xy;
		fOccNorm = g_texDepth.Sample(DefaultSampler, vRandomUV).g * g_fFar * In.fViewZ;

		if (fOccNorm <= In.fDepth + 0.0003f)
			++iColor;
	}
	
	vAmbient = abs((iColor / 16.f) - 1);

	return vAmbient;
}

VS_OUT VS_MAIN_SSAO(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPosition = vector(In.vPosition.x, In.vPosition.y, 0.f, 1.f);
	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4		vDepth = g_texDepth.Sample(DefaultSampler, In.vTexUV);
	float4		vNormal = g_texNormal.Sample(DefaultSampler, In.vTexUV);

	vNormal = normalize(vNormal * 2.f - 1.f);

	tagSSAO SSAO = (tagSSAO)0;
	SSAO.vUV = In.vTexUV;
	SSAO.vNormal = vNormal.rgb;
	SSAO.fViewZ = vDepth.b * g_fFar;
	SSAO.fDepth = vDepth.g * g_fFar * SSAO.fViewZ;

	Out.vColor = (1.f - Get_SSAO(SSAO));

	return Out;
}

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTION(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_texNormal.Sample(DefaultSampler, In.vTexUV);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector		vDepthDesc = g_texDepth.Sample(DefaultSampler, In.vTexUV);
	vector		vWorld;

	// 원근 투영된 위치를 월드 위치로 돌립니다.

	/* 투영 공간 위치 ( 월드 좌표 * 뷰 행렬 * 투영 행렬 ) / 뷰 행렬 Z  */
	vWorld.x = In.vTexUV.x *  2.f - 1.f;
	vWorld.y = In.vTexUV.y * -2.f + 1.f;
	vWorld.w = 1.f;

	// DepthDesc.x = 투영 좌표.z / 투영 좌표w;
	vWorld.z = vDepthDesc.x;
	// DepthDesc.y = 투영 위치.w / 카메라 Far
	float			fViewZ = vDepthDesc.y * g_fFar;

	/* 뷰 공간 위치 ( 월드 좌표 * 뷰 행렬 * 투영 행렬 ) ( = * 뷰 행렬 Z) */
	vWorld = vWorld * fViewZ;

	/* 뷰 공간 위치 ( 월드 좌표 * 뷰 행렬 ) ( = * 역 투영 행렬 ) */
	vWorld = mul(vWorld, g_matProjInv);

	/* 월드 공간 위치 ( 월드 좌표 ) ( = * 역 뷰 행렬 ) */
	vWorld = mul(vWorld, g_matViewInv);

	vector vSSAO = g_texSSAO.Sample(DefaultSampler, In.vTexUV);
	if (g_bSSAO)
	{
		Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal))
			+ ((g_vLightAmbient* vSSAO) * g_vMtrlAmbient));

		vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
		vector		vLook = normalize(vWorld - g_vCamPos);

		Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower)
			* (g_vLightSpecular * g_vMtrlSpecular);
	}
	else
	{
		Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal))
			+ ((g_vLightAmbient) * g_vMtrlAmbient));

		vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
		vector		vLook = normalize(vWorld - g_vCamPos);

		Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower)
			* (g_vLightSpecular * g_vMtrlSpecular);
	}

	if (Out.vShade.a <= 0.1f)
		discard;

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
	float			fLength = length(vLightDir);
	float			fAtt = saturate((g_fRange - fLength) / g_fRange);

	vector		vSSAO = g_texSSAO.Sample(ClampSampler, In.vTexUV);
	if (g_bSSAO)
	{
		Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal))
			+ ((g_vLightAmbient *vSSAO)* g_vMtrlAmbient)) * fAtt;

		vector		vReflect = reflect(normalize(vLightDir), vNormal);
		vector		vLook = normalize(vWorld - g_vCamPos);

		Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower)
			* (g_vLightSpecular * g_vMtrlSpecular) * fAtt;
	}
	else
	{
		Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal))
			+ ((g_vLightAmbient)* g_vMtrlAmbient)) * fAtt;

		vector		vReflect = reflect(normalize(vLightDir), vNormal);
		vector		vLook = normalize(vWorld - g_vCamPos);

		Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower)
			* (g_vLightSpecular * g_vMtrlSpecular) * fAtt;
	}

	if (Out.vShade.a <= 0.1f)
		discard;


	return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_SPOT(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_texNormal.Sample(DefaultSampler, In.vTexUV);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector		vDepthDesc = g_texDepth.Sample(DefaultSampler, In.vTexUV);
	float			fViewZ = vDepthDesc.y * g_fFar;
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

	vector	vPixelDir = normalize(vWorld - g_vLightPosition);
	vector	vLightDir = normalize(g_vTargetPosition - g_vLightPosition);
	float		fPixelCos = dot(vLightDir, vPixelDir);
	float		fEpsilon = g_fInnerCosine - g_fOuterCosine;
	float		fAtt = saturate(fPixelCos - g_fOuterCosine) / fEpsilon;

	vector vSSAO = g_texSSAO.Sample(ClampSampler, In.vTexUV);
	if (g_bSSAO)
	{
		Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal))
			+ ((g_vLightAmbient *vSSAO)* g_vMtrlAmbient)) * fAtt;

		vector		vReflect = reflect(normalize(vLightDir), vNormal);
		vector		vLook = normalize(vWorld - g_vCamPos);

		Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower)
			* (g_vLightSpecular * g_vMtrlSpecular) * fAtt;
	}
	else
	{
		Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal))
			+ ((g_vLightAmbient )* g_vMtrlAmbient)) * fAtt;

		vector		vReflect = reflect(normalize(vLightDir), vNormal);
		vector		vLook = normalize(vWorld - g_vCamPos);

		Out.vSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower)
			* (g_vLightSpecular * g_vMtrlSpecular) * fAtt;
	}
	if (Out.vShade.a <= 0.1f)
		discard;

	return Out;
}


struct PS_OUT_BLEND {
	vector		vColor : SV_TARGET0;
	vector		vGlow : SV_TARGET1;
};

cbuffer Deferred_Fog_Desc {

	float		fFogEnd;
	float		fFogBegin;
	int			iFogging;
	float4		vFogColor;

	int			iFogPadding;

};

PS_OUT_BLEND PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT_BLEND Out = (PS_OUT_BLEND)0;

	vector		vDefault = g_texDefault.Sample(DefaultSampler, In.vTexUV);
	vector		vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	vector		vShade = g_texShade.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecular = g_texSpecular.Sample(DefaultSampler, In.vTexUV);
	vector		vDepth = g_texDepth.Sample(DefaultSampler, In.vTexUV);
	vector		vGlow = g_texGlow.Sample(DefaultSampler, In.vTexUV);
	vector		vRimLight = g_texRimLight.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = (vDiffuse * vShade);
	if (!any(Out.vColor))
	{
		Out.vColor = vDefault;
	}
	else
	{

		Out.vColor += vSpecular + vRimLight;

	}

	

	
	Out.vGlow = vGlow;

	float		fViewZ = vDepth.y * g_fFar;

	// Shadow
	vector vPosition;

	vPosition.x = (In.vTexUV.x *  2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepth.x * fViewZ;
	vPosition.w = fViewZ;


	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);

	vector PixelPosition = vPosition;

	vPosition = mul(vPosition, g_matLightView);

	vector		vLightDepthTexUV = mul(vPosition, g_matLightProj);
	float2		vLightDepthUV;

	vLightDepthUV.x = (vLightDepthTexUV.x / vLightDepthTexUV.w) *  0.5f + 0.5f;
	vLightDepthUV.y = (vLightDepthTexUV.y / vLightDepthTexUV.w) * -0.5f + 0.5f;

	vector		vShadowDepth = g_texShadowDepth.Sample(DepthSampler, vLightDepthUV);

	[flatten]
	if ((saturate(vLightDepthUV.x) == vLightDepthUV.x && saturate(vLightDepthUV.y) == vLightDepthUV.y) && (vShadowDepth.r > 0))
	{
		[flatten]
		if (vPosition.z - 0.22f >= vShadowDepth.r * g_fFar)
		{
			Out.vColor *= vector(0.35f, 0.35f, 0.35f, 0.35f);
			Out.vColor += vector(0.11f, 0.11f, 0.11f, 0.f);
		}
	}
	
	[flatten]
	if (iFogging != 0)
	{
		float PixelLength = length(PixelPosition - g_vCamPos);
		float Fog = clamp((fFogEnd - PixelLength) / (fFogEnd - fFogBegin), 0.f, 0.77f);
		Out.vColor = lerp(Out.vColor, vFogColor, Fog);
	}

	[flatten]
	if (Out.vColor.a == 0.0f)
		discard;


	return Out;
}

PS_OUT PS_HORIZONTAL_GLOW_9(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vGlow;
	vector vAfterDeferred_Glow;
	vector vGlowOverwrite;
	vector vResult = (vector)0;
	float2 vUV = In.vTexUV;
	const float		fWidthTexel = 1.f / g_fScreenWidth;

	// Weighted Average
	const float fWeight0 = 0.35f;
	const float fWeight1 = 0.4f;
	const float fWeight2 = 0.45f;
	const float fWeight3 = 0.5f;
	const float fWeight4 = 1.0f;

	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 4.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 3.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 2.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 1.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight4;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 1.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 2.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 3.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 4.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}

	vResult /= 2.f * (fWeight0 + fWeight1 + fWeight2 + fWeight3) + fWeight4;

	Out.vColor = vResult;

	return Out;
}

PS_OUT PS_HORIZONTAL_GLOW_19(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vGlow;
	vector vAfterDeferred_Glow;
	vector vGlowOverwrite;
	vector vResult = (vector)0;
	float2 vUV = In.vTexUV;
	const float		fWidthTexel = 1.f / g_fScreenWidth;

	//const float		fHeightTexel = 1.f / g_fScreenHeight;

	// Weighted Average
	const float fWeight0 = 0.1f;
	const float fWeight1 = 0.2f;
	const float fWeight2 = 0.3f;
	const float fWeight3 = 0.4f;
	const float fWeight4 = 0.5f;
	const float fWeight5 = 0.6f;
	const float fWeight6 = 0.7f;
	const float fWeight7 = 0.8f;
	const float fWeight8 = 0.9f;
	const float fWeight9 = 2.0f;
	//const float fWeight0 = 0.1f;
	//const float fWeight1 = 0.15f;
	//const float fWeight2 = 0.2f;
	//const float fWeight3 = 0.25f;
	//const float fWeight4 = 0.3f;
	//const float fWeight5 = 0.35f;
	//const float fWeight6 = 0.4f;
	//const float fWeight7 = 0.45f;
	//const float fWeight8 = 0.5f;
	//const float fWeight9 = 1.0f;

	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 9.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 8.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 7.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 6.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 5.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight4;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 4.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight5;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 3.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight6;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 2.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight7;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 1.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight8;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight9;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 1.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight8;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 2.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight7;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 3.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight6;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 4.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight5;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 5.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight4;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 6.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 7.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 8.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 9.f * fWidthTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}

	vResult /= 2.f * (fWeight0 + fWeight1 + fWeight2 + fWeight3 + fWeight4 + fWeight5 + fWeight6 + fWeight7 + fWeight8) + fWeight9;

	Out.vColor = vResult;

	return Out;
}

PS_OUT PS_VERTICAL_GLOW_19(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vGlow_Horizontal;
	vector vResult = (vector)0;
	float2 vUV = In.vTexUV;
	const float		fWidthTexel = 1.f / g_fScreenWidth;
	//const float		fHeightTexel = 1.f / g_fScreenHeight;

	// Weighted Average

	const float fWeight0 = 0.1f;
	const float fWeight1 = 0.2f;
	const float fWeight2 = 0.3f;
	const float fWeight3 = 0.4f;
	const float fWeight4 = 0.5f;
	const float fWeight5 = 0.6f;
	const float fWeight6 = 0.7f;
	const float fWeight7 = 0.8f;
	const float fWeight8 = 0.9f;
	const float fWeight9 = 2.0f;
	//const float fWeight0 = 0.1f;
	//const float fWeight1 = 0.15f;
	//const float fWeight2 = 0.2f;
	//const float fWeight3 = 0.25f;
	//const float fWeight4 = 0.3f;
	//const float fWeight5 = 0.35f;
	//const float fWeight6 = 0.4f;
	//const float fWeight7 = 0.45f;
	//const float fWeight8 = 0.5f;
	//const float fWeight9 = 1.0f;


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 9.f * fWidthTexel;
	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight0;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 8.f * fWidthTexel;
	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight1;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 7.f * fWidthTexel;
	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight2;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 6.f * fWidthTexel;
	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight3;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 5.f * fWidthTexel;
	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight4;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 4.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight5;
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 3.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight6;
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 2.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight7;
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 1.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight8;
	////////////////////////////////////////
	vUV = In.vTexUV;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight9;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 1.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight8;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 2.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight7;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 3.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight6;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 4.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight5;



	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 5.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight4;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 6.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight3;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 7.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight2;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 8.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight1;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 9.f * fWidthTexel;

	vGlow_Horizontal = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow_Horizontal * fWeight0;

	////////////////////////////////////////


	vResult /= 2.f * (fWeight0 + fWeight1 + fWeight2 + fWeight3 + fWeight4 + fWeight5 + fWeight6 + fWeight7 + fWeight8) + fWeight9;
	Out.vColor = vResult;

	return Out;
}

PS_OUT PS_VERTICAL_GLOW_9(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vGlow;
	vector vAfterDeferred_Glow;
	vector vGlowOverwrite;
	vector vResult = (vector)0;
	float2 vUV = In.vTexUV;
	const float		fHeightTexel = 1.f / g_fScreenHeight;

	// Weighted Average
	const float fWeight0 = 0.35f;
	const float fWeight1 = 0.4f;
	const float fWeight2 = 0.45f;
	const float fWeight3 = 0.5f;
	const float fWeight4 = 1.0f;

	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 4.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 3.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 2.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 1.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight4;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 1.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 2.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 3.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 4.f * fHeightTexel;

	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite = vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}

	vResult /= 2.f * (fWeight0 + fWeight1 + fWeight2 + fWeight3) + fWeight4;

	Out.vColor = vResult;

	return Out;
}


PS_OUT_GLOW PS_HORIZONTAL_GLOW_41(PS_IN In)
{
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
	vector vGlow;
	vector vAfterDeferred_Glow;
	vector vGlowOverwrite;
	vector vResult = (vector)0;
	float2 vUV = In.vTexUV;
	const float		fWidthTexel = 1.f / g_fScreenWidth;

	// Weighted Average
	const float fWeight0 = 2.1f;
	const float fWeight1 = 2.2f;
	const float fWeight2 = 2.3f;
	const float fWeight3 = 2.4f;
	const float fWeight4 = 2.5f;
	const float fWeight5 = 2.6f;
	const float fWeight6 = 2.7f;
	const float fWeight7 = 2.8f;
	const float fWeight8 = 2.9f;
	const float fWeight9 = 3.0f;
	const float fWeight10 = 3.1f;
	const float fWeight11 = 3.2f;
	const float fWeight12 = 3.3f;
	const float fWeight13 = 3.4f;
	const float fWeight14 = 3.5f;
	const float fWeight15 = 3.6f;
	const float fWeight16 = 3.7f;
	const float fWeight17 = 3.8f;
	const float fWeight18 = 3.9f;
	const float fWeight19 = 4.0f;
	const float fWeight20 = 10.1f;

	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 20.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 19.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 18.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 17.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 16.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight4;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 15.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight5;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 14.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight6;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 13.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight7;
	}


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 12.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight8;
	}

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 11.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight9;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 10.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight10;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 9.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight11;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 8.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight12;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 7.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight13;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 6.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight14;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 5.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight15;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 4.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight16;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 3.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight17;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 2.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight18;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x -= 1.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight19;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight20;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 1.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight19;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 2.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight18;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 3.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight17;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 4.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight16;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 5.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight15;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 6.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight14;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 7.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight13;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 8.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight12;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 9.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight11;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 10.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight10;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 11.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight9;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 12.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight8;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 13.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight7;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 14.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight6;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 15.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight5;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 16.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight4;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 17.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight3;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 18.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight2;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 19.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight1;
	}
	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.x += 20.f * fWidthTexel;

	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
	vGlowOverwrite = vGlow;
	// overwrite
	if (any(vAfterDeferred_Glow))
	{
		vGlowOverwrite += vAfterDeferred_Glow;
		vResult += vGlowOverwrite * fWeight0;
	}
	vResult /= 2.f
		* (fWeight0 + fWeight1 + fWeight2 + fWeight3 + fWeight4 + fWeight5 + fWeight6 + fWeight7 + fWeight8 + fWeight9
			+ fWeight10 + fWeight11 + fWeight12 + fWeight13 + fWeight14 + fWeight15 + fWeight16 + fWeight17 + fWeight18 + fWeight19)
		+ fWeight20;

	Out.vColor = vResult;

	return Out;
}

PS_OUT_GLOW PS_VERTICAL_GLOW_41(PS_IN In)
{
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
	vector vGlow;
	vector vResult = (vector)0;
	float2 vUV = In.vTexUV;
	const float	fHeightTexel = 1.f / g_fScreenHeight;

	// Weighted Average
	const float fWeight0 = 2.1f;
	const float fWeight1 = 2.2f;
	const float fWeight2 = 2.3f;
	const float fWeight3 = 2.4f;
	const float fWeight4 = 2.5f;
	const float fWeight5 = 2.6f;
	const float fWeight6 = 2.7f;
	const float fWeight7 = 2.8f;
	const float fWeight8 = 2.9f;
	const float fWeight9 = 3.0f;
	const float fWeight10 = 3.1f;
	const float fWeight11 = 3.2f;
	const float fWeight12 = 3.3f;
	const float fWeight13 = 3.4f;
	const float fWeight14 = 3.5f;
	const float fWeight15 = 3.6f;
	const float fWeight16 = 3.7f;
	const float fWeight17 = 3.8f;
	const float fWeight18 = 3.9f;
	const float fWeight19 = 4.0f;
	const float fWeight20 = 10.1f;

	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 20.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight0;
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 19.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight1;
	//////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 18.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight2;



	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 17.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight3;


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 16.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight4;


	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 15.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight5;



	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 14.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight6;



	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 13.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight7;



	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 12.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight8;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 11.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight9;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 10.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight10;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 9.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight11;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 8.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight12;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 7.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight13;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 6.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight14;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 5.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight15;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 4.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight16;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 3.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight17;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 2.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight18;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y -= 1.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight19;

	////////////////////////////////////////
	vUV = In.vTexUV;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight20;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 1.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight19;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 2.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight18;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 3.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight17;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 4.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight16;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 5.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight15;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 6.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight14;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 7.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight13;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 8.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight12;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 9.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight11;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 10.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight10;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 11.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight9;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 12.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight8;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 13.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight7;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 14.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight6;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 15.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight5;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 16.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight4;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 17.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight3;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 18.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight2;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 19.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight1;

	////////////////////////////////////////
	vUV = In.vTexUV;
	vUV.y += 20.f * fHeightTexel;

	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
	vResult += vGlow * fWeight0;




	vResult /= 2.f
		* (fWeight0 + fWeight1 + fWeight2 + fWeight3 + fWeight4 + fWeight5 + fWeight6 + fWeight7 + fWeight8 + fWeight9 +
			fWeight10 + fWeight11 + fWeight12 + fWeight13 + fWeight14 + fWeight15 + fWeight16 + fWeight17 + fWeight18 + fWeight19)
		+ fWeight20;

	Out.vColor = vResult;

	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

//PS_OUT_GLOW PS_HORIZONTAL_GLOW_41(PS_IN In)
//{
//	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
//	vector vGlow;
//	vector vAfterDeferred_Glow;
//	vector vGlowOverwrite;
//	vector vResult = (vector)0;
//	float2 vUV = In.vTexUV;
//	const float		fWidthTexel = 1.f / g_fScreenWidth;
//
//	// Weighted Average
//	const float fWeight0 = 2.1f;
//	const float fWeight1 = 2.2f;
//	const float fWeight2 = 2.3f;
//	const float fWeight3 = 2.4f;
//	const float fWeight4 = 2.5f;
//	const float fWeight5 = 2.6f;
//	const float fWeight6 = 2.7f;
//	const float fWeight7 = 2.8f;
//	const float fWeight8 = 2.9f;
//	const float fWeight9 = 3.0f;
//	const float fWeight10 = 3.1f;
//	const float fWeight11 = 3.2f;
//	const float fWeight12 = 3.3f;
//	const float fWeight13 = 3.4f;
//	const float fWeight14 = 3.5f;
//	const float fWeight15 = 3.6f;
//	const float fWeight16 = 3.7f;
//	const float fWeight17 = 3.8f;
//	const float fWeight18 = 3.9f;
//	const float fWeight19 = 4.0f;
//	const float fWeight20 = 10.1f;
//
//	//////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 20.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight0;
//	}
//	//////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 19.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight1;
//	}
//	//////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 18.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight2;
//	}
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 17.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight3;
//	}
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 16.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight4;
//	}
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 15.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight5;
//	}
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 14.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight6;
//	}
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 13.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight7;
//	}
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 12.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight8;
//	}
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 11.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight9;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 10.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight10;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 9.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight11;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 8.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight12;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 7.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight13;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 6.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight14;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 5.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight15;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 4.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight16;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 3.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight17;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 2.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight18;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x -= 1.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight19;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight20;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 1.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight19;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 2.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight18;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 3.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight17;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 4.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight16;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 5.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight15;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 6.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight14;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 7.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight13;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 8.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight12;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 9.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight11;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 10.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight10;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 11.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight9;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 12.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight8;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 13.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight7;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 14.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight6;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 15.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight5;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 16.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight4;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 17.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight3;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 18.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight2;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 19.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight1;
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.x += 20.f * fWidthTexel;
//
//	vGlow = g_texGlow_DeferredResult.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * fWeight0;
//	}
//	vResult /= 2.f
//		* (fWeight0 + fWeight1 + fWeight2 + fWeight3 + fWeight4 + fWeight5 + fWeight6 + fWeight7 + fWeight8 + fWeight9
//			+ fWeight10 + fWeight11 + fWeight12 + fWeight13 + fWeight14 + fWeight15 + fWeight16 + fWeight17 + fWeight18 + fWeight19)
//		+ fWeight20;
//
//	Out.vColor = vResult;
//
//	return Out;
//}
//
//PS_OUT_GLOW PS_VERTICAL_GLOW_41(PS_IN In)
//{
//	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
//	vector vGlow;
//	vector vResult = (vector)0;
//	float2 vUV = In.vTexUV;
//	const float	fHeightTexel = 1.f / g_fScreenHeight;
//
//	// Weighted Average
//	const float fWeight0 = 2.1f;
//	const float fWeight1 = 2.2f;
//	const float fWeight2 = 2.3f;
//	const float fWeight3 = 2.4f;
//	const float fWeight4 = 2.5f;
//	const float fWeight5 = 2.6f;
//	const float fWeight6 = 2.7f;
//	const float fWeight7 = 2.8f;
//	const float fWeight8 = 2.9f;
//	const float fWeight9 = 3.0f;
//	const float fWeight10 = 3.1f;
//	const float fWeight11 = 3.2f;
//	const float fWeight12 = 3.3f;
//	const float fWeight13 = 3.4f;
//	const float fWeight14 = 3.5f;
//	const float fWeight15 = 3.6f;
//	const float fWeight16 = 3.7f;
//	const float fWeight17 = 3.8f;
//	const float fWeight18 = 3.9f;
//	const float fWeight19 = 4.0f;
//	const float fWeight20 = 10.1f;
//
//	//////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 20.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight0;
//	//////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 19.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight1;
//	//////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 18.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight2;
//
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 17.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight3;
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 16.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight4;
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 15.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight5;
//
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 14.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight6;
//
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 13.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight7;
//
//
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 12.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight8;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 11.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight9;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 10.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight10;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 9.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight11;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 8.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight12;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 7.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight13;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 6.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight14;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 5.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight15;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 4.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight16;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 3.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight17;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 2.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight18;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y -= 1.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight19;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight20;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 1.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight19;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 2.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight18;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 3.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight17;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 4.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight16;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 5.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight15;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 6.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight14;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 7.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight13;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 8.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight12;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 9.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight11;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 10.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight10;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 11.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight9;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 12.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight8;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 13.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight7;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 14.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight6;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 15.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight5;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 16.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight4;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 17.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight3;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 18.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight2;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 19.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight1;
//
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//	vUV.y += 20.f * fHeightTexel;
//
//	vGlow = g_texGlow_Horizontal.Sample(DefaultSampler, vUV);
//	vResult += vGlow * fWeight0;
//
//
//
//
//	vResult /= 2.f
//		* (fWeight0 + fWeight1 + fWeight2 + fWeight3 + fWeight4 + fWeight5 + fWeight6 + fWeight7 + fWeight8 + fWeight9 +
//			fWeight10 + fWeight11 + fWeight12 + fWeight13 + fWeight14 + fWeight15 + fWeight16 + fWeight17 + fWeight18 + fWeight19)
//		+ fWeight20;
//
//	Out.vColor = vResult;
//
//	if (Out.vColor.a <= 0.01f)
//		discard;
//
//	return Out;
//}

//PS_OUT PS_VERTICAL_GLOW_41(PS_IN In)
//{
//	PS_OUT Out = (PS_OUT)0;
//	vector vGlow;
//	vector vAfterDeferred_Glow;
//	vector vGlowOverwrite;
//	vector vResult = (vector)0;
//	float2 vUV = In.vTexUV;
//	const float		fHeightTexel = 1.f / g_fScreenHeight;
//
//	// Weighted Average
//
//	float arrWeight[21] = {
//		0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f,
//		1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f, 2.0f,
//		10.1f
//	};
//
//	//////////////////////////////////////////
//	// 20 ~ 1
//	for (int i = 20; i > 0; --i)
//	{
//		vUV = In.vTexUV;
//		vUV.y -= i * fHeightTexel;
//
//		vGlow = g_texGlow.Sample(DefaultSampler, vUV);
//		vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//		vGlowOverwrite = vGlow;
//		// overwrite
//		if (any(vAfterDeferred_Glow))
//		{
//			vGlowOverwrite += vAfterDeferred_Glow;
//			vResult += vGlowOverwrite * arrWeight[21 - i - 1];
//		}
//	}
//	////////////////////////////////////////
//	vUV = In.vTexUV;
//
//	vGlow = g_texGlow.Sample(DefaultSampler, vUV);
//	vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//	vGlowOverwrite = vGlow;
//	// overwrite
//	if (any(vAfterDeferred_Glow))
//	{
//		vGlowOverwrite += vAfterDeferred_Glow;
//		vResult += vGlowOverwrite * arrWeight[20];
//	}
//	////////////////////////////////////////
//	for (int i = 0 + 1; i < 20 + 1; --i)
//	{
//		vUV = In.vTexUV;
//		vUV.y += i * fHeightTexel;
//
//		vGlow = g_texGlow.Sample(DefaultSampler, vUV);
//		vAfterDeferred_Glow = g_texGlow_AfterDeferred.Sample(DefaultSampler, vUV);
//		vGlowOverwrite = vGlow;
//		// overwrite
//		if (any(vAfterDeferred_Glow))
//		{
//			vGlowOverwrite += vAfterDeferred_Glow;
//			vResult += vGlowOverwrite * arrWeight[i - 1];
//		}
//	}
//
//	float fDevide = 0.f;
//	for (int i = 0; i < 20; ++i)
//	{
//		fDevide += arrWeight[i] * 2.f;
//	}
//	fDevide += arrWeight[20];
//
//	vResult /= fDevide;
//
//	Out.vColor = vResult;
//
//	return Out;
//}


PS_OUT PS_POSTPROCESSING_BEFOREUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector		vDistortion = g_texDistortionMap.Sample(DefaultSampler, In.vTexUV);
	float2			vDistortionWeight;
	vDistortionWeight.x = (vDistortion.r + vDistortion.g) / 2.f * 0.06f;
	vDistortionWeight.y = (vDistortion.r + vDistortion.b) / 2.f * 0.06f;

	vector		vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV + vDistortionWeight);
	vector		vAlphaGroup = g_texAlphaGroup.Sample(DefaultSampler, In.vTexUV + vDistortionWeight);
	vector		vGlow = g_texGlow.Sample(DefaultSampler, In.vTexUV + vDistortionWeight);
	vector		vDepth_AlphaGroup = g_texDepth_AlphaGroup.Sample(DefaultSampler, In.vTexUV);
	vector		vDepth = g_texDepth.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = vDiffuse.a;
	    
	float		fAlphaViewZ = vDepth_AlphaGroup.y * g_fFar;
	float		fDiffuseViewZ = vDepth.y * g_fFar;
	float		fDepthGap = saturate(abs(fAlphaViewZ - fDiffuseViewZ));

	vAlphaGroup.a *= fDepthGap;

	if (vAlphaGroup.a > 0.f)
	{
		Out.vColor.rgb = saturate(vDiffuse.rgb * (1 - vAlphaGroup.a) + vAlphaGroup.rgb * vAlphaGroup.a + vGlow.rgb * 3.f);
	}
	else
	{
		Out.vColor = saturate(vDiffuse + vGlow * 2);
	}



	return Out;

}


PS_OUT PS_POSTPROCESSING_BEFOREUI_Blur(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	[flatten]
	if (g_bMotionBlur)
	{
		int			iNumBlurSample = 16;
		float4		Velocity = g_texMotionBlur.Sample(DefaultSampler, In.vTexUV);
		Velocity.xy /= (float)iNumBlurSample;

		int			iCnt = 1;
		float4		BColor;
		float4		Depth;
		for (int i = iCnt; i < iNumBlurSample; i++)
		{
			BColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV + Velocity.xy * (float)i);
			Out.vColor += BColor;
		}
		Out.vColor /= (float)iNumBlurSample;
	}

	[flatten]
	if (g_bDodge)
	{
		vector		vDodge = g_texDodge.Sample(DefaultSampler, In.vTexUV);
		Out.vColor = saturate(Out.vColor + vDodge);
	}
	

	return Out;
}



PS_OUT PS_POSTPROCESSING_BEFOREUI_DISTORTION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	[flatten]
	if (g_bDistortion)
	{
		float2		vDistortionUV = In.vTexUV + g_fTimeDelta;

		vDistortionUV.x = In.vTexUV.x;
		vDistortionUV.y = clamp(vDistortionUV.y, -1.f, 1.f);

		vector		vDistortionVector = g_texDistortion.Sample(DefaultSampler, vDistortionUV);

		float2		vTexUV = In.vTexUV + vDistortionVector.rg * 0.0135f;

		Out.vColor = g_texDiffuse.Sample(DefaultSampler, vTexUV);

		vTexUV.x = clamp(vTexUV.x, -1.f, 1.f);
		vTexUV.y = clamp(vTexUV.y, -1.f, 1.f);
	}

	[flatten]
	if (g_bGrayDistortion)
	{
		float2		vDistortionUV = In.vTexUV + g_fTimeDelta;
		vDistortionUV.x = In.vTexUV.x;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
		vDistortionUV.y = clamp(vDistortionUV.y, -1.f, 1.f);

		vector		vDistortionVector = g_texDistortion.Sample(DefaultSampler, vDistortionUV);

		float2		vTexUV = In.vTexUV + vDistortionVector.rg * 0.0135f;

		vTexUV.x = clamp(vTexUV.x, -1.f, 1.f);
		vTexUV.y = clamp(vTexUV.y, -1.f, 1.f);

		Out.vColor = g_texDiffuse.Sample(DefaultSampler, vTexUV);

		float		fGrayScale = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;

		Out.vColor.rgb = fGrayScale;
	}
	

	return Out;
}


cbuffer tagDodgeTexDesc {
	float2		vLT_TexCoord;
	float2		vRT_TexCoord;
	float2		vRB_TexCoord;
	float2		vLB_TexCoord;
};

struct VS_OUT_DODGE {
	float4		vPosition : SV_POSITION;
	float2		vTexUV0 : TEXCOORD0;
	float2		vTexUV1 : TEXCOORD1;
	float2		vTexUV2 : TEXCOORD2;
	float2		vTexUV3 : TEXCOORD3;
};

struct PS_IN_DODGE {
	float4		vPosition : SV_POSITION;
	float2		vTexUV0 : TEXCOORD0;
	float2		vTexUV1 : TEXCOORD1;
	float2		vTexUV2 : TEXCOORD2;
	float2		vTexUV3 : TEXCOORD3;
};

VS_OUT_DODGE VS_MAIN_DODGE(VS_IN In)
{
	VS_OUT_DODGE Out = (VS_OUT_DODGE)0;
	Out.vPosition = vector(In.vPosition, 1.f);
	
	Out.vTexUV0 = In.vTexUV + vLT_TexCoord;
	Out.vTexUV1 = In.vTexUV + vRT_TexCoord;
	Out.vTexUV2 = In.vTexUV + vRB_TexCoord;
	Out.vTexUV3 = In.vTexUV + vLB_TexCoord;

	Out.vTexUV0.x = clamp(Out.vTexUV0.x, 0.f, 1.f);
	Out.vTexUV0.y = clamp(Out.vTexUV0.y, 0.f, 1.f);
	Out.vTexUV1.x = clamp(Out.vTexUV1.x, 0.f, 1.f);
	Out.vTexUV1.y = clamp(Out.vTexUV1.y, 0.f, 1.f);
	Out.vTexUV2.x = clamp(Out.vTexUV2.x, 0.f, 1.f);
	Out.vTexUV2.y = clamp(Out.vTexUV2.y, 0.f, 1.f);
	Out.vTexUV3.x = clamp(Out.vTexUV3.x, 0.f, 1.f);
	Out.vTexUV3.y = clamp(Out.vTexUV3.y, 0.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_DODGE(PS_IN_DODGE In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vLeftTopColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV0);
	float4 vRightTopColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV1);
	float4 vRightBottomColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV2);
	float4 vLeftBottomColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV3);

	Out.vColor = saturate(vLeftTopColor + vRightTopColor + vRightBottomColor + vLeftBottomColor);

	return Out;
}

PS_OUT PS_POSTPROCESSING_BEFOREUI_DOWNSAMPLING(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_POSTPROCESSING_BEFOREUI_GAUSSIANBLUR_X(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vTexUV = In.vTexUV;
	float2 UV = 0;
	float tU = 1 / g_fScreenWidth;

	[unroll]
	for (int i = -6; i < 6; i++) {
		UV = vTexUV + float2(tU * i, 0);
		Out.vColor += GaussianWeights[6 + i] 
			* g_texDiffuse.Sample(DefaultSampler, UV);
	}

	Out.vColor /= GaussianTotal;

	return Out;
}


PS_OUT PS_POSTPROCESSING_BEFOREUI_GAUSSIANBLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vTexUV = In.vTexUV;
	float2 UV = 0;
	float tV = 1 / g_fScreenHeight;

	[unroll]
	for (int i = -6; i < 6; i++) {
		UV = vTexUV + float2(0, tV * i);
		Out.vColor += GaussianWeights[6 + i]
			* g_texDiffuse.Sample(DefaultSampler, UV);
	}

	Out.vColor /= GaussianTotal;

	return Out;
}

PS_OUT PS_POSTPROCESSING_BEFOREUI_DOF(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float	fCamDepth = g_texDepth.Sample(DefaultSampler, In.vTexUV).b * g_fFar;

	float	fDistance = (fDofDistance >= vDofParams.y + 15.f) ? fDofDistance : vDofParams.y + 15.f;
	
	if (fCamDepth >= fDistance)
	{
		float		discRadius, discRadiusLow, CenterDepth;
		vector	vDepth = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
		CenterDepth = vDepth.a;

		discRadius = abs(vDepth.a * vMaxCoC.y - vMaxCoC.x);
		discRadiusLow = discRadius * fRadiusScale;


		[unroll]
		for (int i = 0; i < 8; i++)
		{
			float2	coordLow	= In.vTexUV + (vPixelSizeLow  * Poisson[i] * discRadiusLow);
			float2	coordHigh	= In.vTexUV + (vPixelSizeHigh * Poisson[i] * discRadius);

			float4	PoissonLow		= g_texDiffuse.Sample(DefaultSampler, coordLow);
			float4	PoissonHigh	= g_texDiffuse.Sample(DefaultSampler, coordHigh);

			float	PoissonBlur = abs(PoissonHigh.a * 2.0 - 1.0);

			float4	LerpColor = lerp(PoissonHigh, PoissonLow, PoissonBlur);

			LerpColor.a = (LerpColor.a >= CenterDepth) ? 1.0 : LerpColor.a;

			Out.vColor.rgb += LerpColor.rgb * LerpColor.a;
			Out.vColor.a += LerpColor.a;
		}

		Out.vColor.rgb = Out.vColor.rgb / Out.vColor.a;

	}
	else
	{
		Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	}

	Out.vColor.rgb = RomBinDaHouseToneMap(Out.vColor.rgb);

	return Out;
}

struct VS_OUT_GODRAY {
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_IN_GODRAY {
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT_GODRAY VS_MAIN_GODRAY(VS_IN In)
{
	VS_OUT_GODRAY Out = (VS_OUT_GODRAY)0;

	Out.vPosition = vector(In.vPosition, 1.f);
	Out.vTexUV = In.vTexUV;

	vector vProj = mul(g_vLightPosition, g_matView);
	vProj = mul(vProj, g_matProj);

	Out.vProjPos = vProj;

	return Out;
}

PS_OUT PS_POSTPROCESSING_GODRAY(PS_IN_GODRAY In)
{
	PS_OUT Out = (PS_OUT)0;

	//int		SHAFTSAMPLE = 64;

	//float2	vDirUV = (In.vTexUV - In.vProjPos.xy);
	//
	//vDirUV *= 1.f / SHAFTSAMPLE * g_vLightShaft.x;

	//float3	vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	//float	IlluminationDecay = 1.0f;

	//for (int i = 0; i < SHAFTSAMPLE; ++i)
	//{
	//	In.vTexUV -= vDirUV;

	//	float3	vRay = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	//	vRay *= IlluminationDecay * g_vLightShaft.z;

	//	vColor += vRay;

	//	IlluminationDecay *= g_vLightShaft.y;
	//}

	//Out.vColor.rgb = (vColor * g_vLightShaft.w);
	//Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_OUT_FINAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	if (g_bHalfRender == false)
		Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	else
	{
		if (In.vTexUV.x < 0.5f)
			Out.vColor = g_texHalf.Sample(DefaultSampler, In.vTexUV);
		else if (In.vTexUV.x >= 0.5f)
			Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	}
	return Out;
}

RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockWise = false;
};

RasterizerState RS_NoCulling {
	FillMode = Solid;
	CullMode = None;
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

	pass SSAO {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN_SSAO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SSAO();
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

	pass Light_Spot {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_SPOT();
	}

	pass Blend {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass OutLine {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass Glow_Horizontal_19 {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HORIZONTAL_GLOW_19();
	}

	pass Glow_Vertical_19 {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_VERTICAL_GLOW_19();
	}

	pass Glow_Horizontal_9 {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HORIZONTAL_GLOW_9();
	}

	pass Glow_Vertical_9 {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_VERTICAL_GLOW_9();
	}

	pass Glow_Horizontal_41 {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HORIZONTAL_GLOW_41();
	}

	pass Glow_Vertical_41 {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_VERTICAL_GLOW_41();
	}

	pass PostProcessing_BeforeUI {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI();
	}

	pass PostProcessing_BeforeUI_Blur {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI_Blur();
	}

	pass PostProcessing_BeforeUI_Distortion {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI_DISTORTION();
	}

	pass Dodge {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN_DODGE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DODGE();
	}

	pass PostProcessing_BeforeUI_DownSampling {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI_DOWNSAMPLING();
	}

	pass PostProcessing_BeforeUI_GaussianBlurX {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI_GAUSSIANBLUR_X();
	}

	pass PostProcessing_BeforeUI_GaussianBlur {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI_GAUSSIANBLUR();
	}

	pass PostProcessing_BeforeUI_DOF {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTPROCESSING_BEFOREUI_DOF();
	}

	pass FinalColor{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OUT_FINAL();
	}

};