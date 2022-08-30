#pragma once
#ifndef __CLIENT_STRUCT_H__
#define __CLIENT_STRUCT_H__

#include "GameInstance.h"

namespace Client {
	typedef struct tagSceneInfo {
		wchar_t		szSceneName[32];
		wchar_t		szParentName[32];
	}SCENE_INFO;

	typedef struct tagAtlasDescription
	{
		_float	fLeft;
		_float	fRight;
		_float	fTop;
		_float	fBottom;
	}ATLASDESC;

	typedef struct tagModelObjectInfo {
		tagModelObjectInfo() {
			ZeroMemory(this, sizeof(tagModelObjectInfo));
			XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
		}
		wchar_t		szModelObjectName[32]; // Model Object's Prototype Name;
		wchar_t		szModelComName[32];

		XMFLOAT4X4	m_matWorld;

		FXMVECTOR	Get_State(void* pTransformType) {
			_uint i = *(_uint*)pTransformType;
			return XMLoadFloat4((XMFLOAT4*)&m_matWorld.m[i][0]);
		}

		XMFLOAT4X4	Get_World() {
			return m_matWorld;
		}

		XMMATRIX	Get_WorldMatrix() {
			return XMLoadFloat4x4(&m_matWorld);
		}

		void		Set_State(void* pTransformType, FXMVECTOR vState) {
			_uint i = *(_uint*)pTransformType;
			XMFLOAT4	vStateDesc;
			XMStoreFloat4(&vStateDesc, vState);

			memcpy((XMFLOAT4*)&m_matWorld.m[i][0], &vStateDesc, sizeof(XMFLOAT4));
		}

		void		Set_Rotation_Radian(float fRadian, FXMVECTOR vAxis = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f)) {

			CTransform::STATE eRight, eUp, eLook;
			eRight = CTransform::STATE::RIGHT; eUp = CTransform::STATE::UP; eLook = CTransform::STATE::LOOK;

			XMVECTOR	vRight = Get_State((void*)&eRight);
			XMVECTOR	vUp = Get_State((void*)&eUp);
			XMVECTOR	vLook = Get_State((void*)&eLook);

			XMMATRIX	matRotation = XMMatrixRotationAxis(vAxis, fRadian);

			vRight = XMVector3TransformNormal(vRight, matRotation);
			vUp = XMVector3TransformNormal(vUp, matRotation);
			vLook = XMVector3TransformNormal(vLook, matRotation);

			Set_State((void*)&eRight, vRight);
			Set_State((void*)&eUp, vUp);
			Set_State((void*)&eLook, vLook);
		}

	}MO_INFO;


	typedef struct ShaderVariables
	{
		_float	fDissolve = 0.f;
	}SHADERVARIABLES;

	typedef struct PatternVariables
	{
		_float3 vPattern_Position_Current;
		_float3 vPattern_Rotation_Current;
		_float3 vPattern_Scale_Current;

		_float fPattern_Position_LifeTime = 0.f;
		_float fPattern_Rotation_LifeTime = 0.f;
		_float fPattern_Scale_LifeTime = 0.f;
	} PATTERNVARIABLES;

	typedef struct TextureVariables
	{
		_float	fDefaultTextureElpasedTime = 0.f;
		_float	fAlphaOneTextureElpasedTime = 0.f;
		_float	fMaskTextureElpasedTime = 0.f;

		_int	iDefaultTextureIndex = 0;
		_int	iAlphaOneTextureIndex = 0;
		_int	iMaskTextureIndex = 0;
	} TEXTUREVARIABLES;

	typedef struct CreateDeathVariables
	{
		_float2				vCreateDeathTime;
		_bool				bCreateDeathRepeat;
	}CREATEDEATHVARIABLES;

	typedef struct FadeVariables
	{
		_float				fCurrentFade = 0.f;
		_float				fFadeVal = 0.f;
	}FADEVARIABLES;

	typedef struct UVAnimationVariables
	{
		_float2				vAnimValue = _float2(0.f, 0.f);
	}UV_ANIM_VAR;



	typedef struct EffectMeshVariables
	{
		_float	fLifeTime = 0.f;
		_bool	bPlay = false;
		_bool	bRenderOn = false;

		PATTERNVARIABLES		tPatternVariables;
		TEXTUREVARIABLES		tTextureVaribles;
		SHADERVARIABLES			tShaderVariables;
		CREATEDEATHVARIABLES	tCreateDeathVariables;
		FADEVARIABLES			tFadeInVariables;
		FADEVARIABLES			tFadeOutVariables;
		UV_ANIM_VAR				tUVAnimVar;
	} EFFECTMESHVARIABLES;
	//typedef struct tShaderName
	//{
	//	TCHAR szName[MAX_PATH];
	//} SHADERNAME;

	typedef struct EffectMeshDescription
	{
		enum class TEXTURE_KIND : int
		{
			DEFAULT_TEX = 0,
			ONEALPHA_TEX = 1,
			MASK_TEX = 2,
			COUNT = 3
		};


		enum class MESH_KIND : int
		{
			MESH = 0,
			IMAGELESS_MESH = 1,
			RECT = 2,
			COUNT = 3
		};

		typedef struct EffectMeshInfo
		{
			_tchar	szEffectMeshName[MAX_PATH];

			//	0 : Mesh, 1 : Imageless, 2 : Rect
			_int	iMeshKind;
			_tchar	szMeshName[MAX_PATH];
			_tchar	szMeshPrototypeName[MAX_PATH];

			_float3 vPosition;
			_float3 vScale;
			_float3 vRotation;


		} EFFECTMESHINFO;

		typedef struct EffectMeshPattern
		{
			// Pattern
			// After Create, Start Time & EndTime
			// Scale
			_bool	bPattern_Scale;
			_float2 vPattern_Scale_StartEndTime;
			//_float3 vPattern_Scale_Init;
			_float3 vPattern_Scale_Goal;
			_bool	bPattern_Scale_Repeat;
			
			// Rotation
			_bool	bPattern_Rotation;
			_float2 vPattern_Rotation_StartEndTime;
			//_float3 vPattern_Rotation_Init;
			_float3 vPattern_Rotation_Goal;
			_bool	bPattern_Rotation_Repeat;
			
			// Position
			_bool	bPattern_Position;
			_float2 vPattern_Position_StartEndTime;
			//_float3 vPattern_Position_Init;
			_float3 vPattern_Position_Goal;
			_bool	bPattern_Position_Repeat;
			
			_bool	bPattern_Position_Projectile;
			_float	fPattern_Position_Projectile_Length_Minimum;
			_float	fPattern_Position_Projectile_Length_Maximum;
			_float	fPattern_Position_Projectile_Length_Result;

			_float	fPattern_Position_Projectile_Angle_Minimum;
			_float	fPattern_Position_Projectile_Angle_Maximum;
			_float	fPattern_Position_Projectile_Angle_Result;

			_float4	vPattern_Position_Projectile_Power;


			// Results
			_float3	vPositionDirection;
			_float	fPositionSpeed = 0.f;

			_float3	vRotationSpeed;
			_float3	vScaleSpeed;



		} EFFECTMESHPATTERN;

		typedef struct UV_Sprite
		{
			_bool	bUVSprite;
			_int	iWidth;
			_int	iHeight;
			_float	fSpriteInterval;
			_bool	bRepeat;
		} UV_SPRITE;


		typedef struct UV_Animation
		{
			_bool	bUVAnimation;
			_float2 vUV_StartEndTime;
			_float2	vUV_Val;
		} UV_ANIMATION;

		typedef struct Fade_InOut
		{
			// Fade In 
			_bool	bFadeIn;
			_float2 vFadeIn_StartEndTime;
			_float	fFadeIn_Degree;
			// Fade Out 
			_bool	bFadeOut;
			_float2 vFadeOut_StartEndTime;
			_float	fFadeOut_Degree;
		} FADE_INOUT;

		typedef struct EffectMeshTexture
		{
			_bool	bDefaultTexture;
			_tchar	szDefaultTextureName[MAX_PATH];
			_tchar	szDefaultTexturePrototypeName[MAX_PATH];
			_int	iDefaultTextureNumber = 1;
			_float	fDefaultTextureInterval = 0.1f;
			_bool	bDefaultTextureSpriteRepeat = false;

			_bool	bAlphaOneTexture;
			_tchar	szAlphaOneTextureName[MAX_PATH];
			_tchar	szAlphaOneTexturePrototypeName[MAX_PATH];
			_int	iAlphaOneTextureNumber = 1;
			_float	fAlphaOneTextureInterval = 0.1f;
			_bool	bAlphaOneTextureSpriteRepeat = false;

			_bool	bMaskTexture;
			_tchar	szMaskTextureName[MAX_PATH];
			_tchar	szMaskTexturePrototypeName[MAX_PATH];
			_int	iMaskTextureNumber = 1;
			_float	fMaskTextureInterval = 0.1f;
			_bool	bMaskTextureSpriteRepeat = false;
		} EFFECTMESHTEXTURE;

		typedef struct EffectMeshShader
		{
			vector<SHADERNAME*> vecShaderName;
			_float	fDissolveSpeed = 0.f;
		}EFFECTMESHSHADER;


		//~EffectMeshDescription()
		//{
		//	for (auto& pIter : tEffectMeshShader.vecShaderName)
		//		Safe_Delete(pIter);
		//	tEffectMeshShader.vecShaderName.clear();
		//}
		EffectMeshInfo tInfo;
		EFFECTMESHTEXTURE tTexture;
		EffectMeshPattern tPattern;
		
		Fade_InOut	tFade_InOut;

		UV_SPRITE tUV_Sprite;
		UV_ANIMATION tUV_Animation;
		EFFECTMESHSHADER tEffectMeshShader;


	} EFFECTMESHDESC;


	typedef struct EffectGroupDescription
	{
		_tchar	szGroupName[MAX_PATH];
		list<EFFECTMESHDESC*> listEffectMeshDesc;

		// CreateTime, DeathTime
		_bool	bCreateDeathRepeat = false;
		_float2 vCreateDeathTime = { 0.f, 0.f };
	} EFFECTGROUPDESC;

	typedef struct tGroupParticleDescription
	{
		list<PARTICLEDESC*> listParticleDesc;
	} PARTICLE_GROUP_DESC;

	typedef struct EffectParticleGroupDescription
	{
		EFFECTGROUPDESC			tEffectMeshGroupDesc;
		PARTICLE_GROUP_DESC		tParticleGroupDesc;
	} EFFECTPARICLEGROUPDESCRIPTION;

	typedef struct FireMissileDescription
	{
		_vector vPosition;
		_vector vDirection;
	} FIRE_MISSILE_DESC;

	//typedef struct tParticleDescription
	//{
	//	_float3 vPosition;
	//	_float3 vScale;
	//	_float3 vRotation;

	//	_int	iTextureNumber = 1;
	//	TCHAR	szTextureName[MAX_PATH];
	//	TCHAR	szTextureProto[MAX_PATH];

	//	_float3 vDirection;
	//	_float	fDensity = 0.f;
	//	_float  fLifeTime = 0.f;
	//	_int	iLifeTimeDensity = 0.f;
	//	_float	fDirectionDensity = 0.f;
	//	_int	iNumber = 1;
	//	_float	fSpeed = 0.f;
	//	_float	fSpeedDensity = 0.f;
	//	_int	iWidth = 0.f;
	//	_int	iHeight = 0.f;
	//	_float	fSpriteInterval = 0.f;
	//} PARTICLEDESC;

	//typedef struct tagItemDesc
	//{
	//	ITEMCATEGORY	iItemCategory	= ITEMCATEGORY::COUNT;
	//	ITEMRARITY		iItemRarity		= ITEMRARITY::COUNT;
	//	_uint			iItemCode		= 0;
	//	_uint			iItemCount		= 0;
	//}ITEMDESC;

	typedef struct tTextureProtoPath
	{
		TCHAR	szName[MAX_PATH];
		TCHAR	szProto[MAX_PATH];
		TCHAR	szPath[MAX_PATH];
		_int	iTextureNumber = 1;
	} TEXNAMEPROTOPATH;

	typedef struct tModelPathName
	{
		TCHAR szPath[MAX_PATH];
		TCHAR szName[MAX_PATH];
		TCHAR szProto[MAX_PATH];
	} MODELPATHNAME;

	typedef struct MonsterBulletInfo
	{
		_float4x4		  Matrix;
		_bool			  bLaser;
	}M_BULLETINFO;

	typedef struct Humanoid_Atk_Info
	{
		_float4x4			  Matrix;
		_uint				  iNumber;
		class CGameObjectModel*	  pMaker;
	}HUMANOID_ATKINFO;



	
	typedef struct tParticleVariables
	{
		_bool	bPlay = false;
		_float	fMultiTextureElapsedTime = 0.f;
		_int	iMultiTextureIndex = 0;
		_float	fLifeTime = 0.f;
	} PARTICLE_VAR;

	typedef struct EffectMovement
	{
		_float4			vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		_float3			vDirection = _float3(0.f, 0.f, 0.f);
		_float			fSpeed = 0.f;
		_float			fJumpPower = 0.f;
		_float			iJumpCount = 0;
	}EFFECT_MOVEMENT;

	typedef struct EffectPacket
	{
		unsigned long						ulID;
		LEVEL								eLevel;
		EFFECT_MOVEMENT						tEffectMovement;
		_float								fMaxLifeTime = -1.f;
		_uint								iAtt = 0;
		int									iNavIndex = -1;
		_float								fColliderRadius = 1.f;
		_bool								bForMiniGame = false;
	} EFFECT_PACKET;

	typedef struct EffectDescription
	{
		EffectParticleGroupDescription		tGroupDesc;
		EFFECT_PACKET						tEffectPacket;
	}EFFECT_DESC;





	typedef struct tagLoadDataFile {
		tagLoadDataFile() {
			eLevel = LEVEL::STATIC;
			eLayer = LAYER::COUNT;
			iSize = 0;
		}
		LEVEL				eLevel;
		LAYER				eLayer;
		size_t				iSize;
		vector<MO_INFO*>	vecInfo;
	}LOADDATA;

	typedef struct tagLoadModelFile {
		tagLoadModelFile() {
			eLevel = LEVEL::STATIC;
		}
		LEVEL				eLevel;
		const char*			pTag;
	}LOADMODELDATA;

	typedef struct tagLightDataDesc {
		LIGHTDESC			tLightDesc;
		LIGHTDEPTHDESC		tDepthDesc;
	}LIGHTDATA;

	typedef struct tagLengthInfo
	{
		class CGameObject*    pGameObject;
		_float                fLength;
	}LENGTHINFO;

	typedef struct tagUIDistanceInfo
	{
		class CUI*		pUI;
		_uint			iDistance;

		bool operator<(const tagUIDistanceInfo& tag) const {
			return this->iDistance > tag.iDistance;
		}
	}UIDISTANCEINFO;

	typedef struct tagEventColliderDesc {
		tagEventColliderDesc() {
			vPivot = XMFLOAT3(0.f, 0.f, 0.f);
			vScale = XMFLOAT3(0.f, 0.f, 0.f);
			vTransform = XMFLOAT3(0.f, 0.f, 0.f);
		}
		// Collider Type AABB
		XMFLOAT3			vPivot;
		XMFLOAT3			vScale;
		// Collider Transform
		XMFLOAT3			vTransform;
		// Collider 
		vector<MO_INFO*>	vecEventInfo;
	} EVENTINFO;

	typedef struct tagAndroidDataPacket {
		tagAndroidDataPacket() : bJump(false), bDoubleJump(false), bImmediately(false) {}

		_bool				bJump;
		_bool				bDoubleJump;
		_bool				bImmediately;
	}ANDROID_DATAPACKET;

	typedef struct EffectLoadInfo
	{
		vector<_tchar*>						vecTextureName;
		vector<_tchar*>						vecModelName;
		EffectParticleGroupDescription		tEffectGroupDesc;
	} EFFECT_LOAD_INFO;

}

#endif // !__CLIENT_STRUCT_H__