#pragma once
#ifndef __ENGINE_STRUCT_H__
#define __ENGINE_STRUCT_H__

namespace Engine {

	typedef struct tagKeyFrame {
		tagKeyFrame() {
			ZeroMemory(this, sizeof(tagKeyFrame));
		}
		XMFLOAT4			vScale;
		XMFLOAT4			vRotation;
		XMFLOAT4			vPosition;
		_double				dTime;
	} KEYFRAME;

	typedef struct tagMeshMaterials {
		class CTexture*		pMaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIAL;

	typedef struct tagLightDesc {
		tagLightDesc() {
			ZeroMemory(this, sizeof(tagLightDesc));
			bLightOn = true;
			eState = LIGHTSTATE::ALWAYS;
		}

		enum class TYPE : unsigned int {
			DIRECTIONAL = 0,
			POINT			= 1,
			SPOT				= 2,
			NONE			= 3
		};

		TYPE				eType = TYPE::NONE;
		bool				bLightOn;

		// 색상
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

		// 방향성 광원
		XMFLOAT4		vDirection;
		// 점 광원
		XMFLOAT4		vPosition;
		float				fRange;
		// 스포트라이트
		XMFLOAT4		vTargetPosition;
		float				fInnerDegree;
		float				fOuterDegree;
		float				fLength;

		LIGHTSTATE	eState;
		double			dDuration;

	}LIGHTDESC;


	typedef struct tagLightDepthDescription
	{
		tagLightDepthDescription() {
			ZeroMemory(this, sizeof(tagLightDepthDescription));
		}
		bool			bDepth;

		// For View Matrix
		XMFLOAT4		vEye;
		XMFLOAT4		vFixedDistance;
		XMFLOAT4		vAt;
		XMFLOAT4		vAxis;
		XMFLOAT4X4		m_matView;

		// For Proj Matrix
		float			fWidth;
		float			fHeigth;
		float			fFOV;
		float			fNear;
		float			fFar;
		XMFLOAT4X4		m_matProj;

	} LIGHTDEPTHDESC;

	typedef struct tagMaterialDesc {
		tagMaterialDesc() {
			ZeroMemory(this, sizeof(tagMaterialDesc));
		}
		XMFLOAT4			vDiffsue;
		XMFLOAT4			vAmbient;
		XMFLOAT4			vSpecular;
		XMFLOAT4			vEmissive;
		float				fPower;
	}MATERIALDESC;

	typedef struct tagVertexMeshes {
		tagVertexMeshes() {
			ZeroMemory(this, sizeof(tagVertexMeshes));
		}
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
		XMUINT4				vBlendIndex;
		XMFLOAT4			vBlendWeight;
	}VTXMESH;

	typedef struct tagVertexRectTexture {
		tagVertexRectTexture() {
			ZeroMemory(this, sizeof(tagVertexRectTexture));
		}
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXRECTTEX;

	typedef struct tagVertexRectNormalTextre {
		tagVertexRectNormalTextre() {
			ZeroMemory(this, sizeof(tagVertexRectNormalTextre));
		}
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
	}VTXRECTNORMTEX;

	typedef struct tagVertexCubeTextre {
		tagVertexCubeTextre() {
			ZeroMemory(this, sizeof(tagVertexCubeTextre));
		}
		XMFLOAT3			vPosition;
		XMFLOAT3			vTexUVW;
	}VTXCUBETEX;

	typedef struct tagVertex_point_UV
	{
		XMFLOAT3			vPosition;
		float				fSize;
		XMFLOAT4			vUV;
	}VTXPOINTUV;

	typedef struct tagVertexPoint {
		tagVertexPoint() {
			ZeroMemory(this, sizeof(tagVertexPoint));
		}
		XMFLOAT3			vPosition;
		float				fSize;
	}VTXPOINT;

	typedef struct tagVertexNavigationColor {
		tagVertexNavigationColor() {
			ZeroMemory(this, sizeof(tagVertexNavigationColor));
		}
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;
	}VTXNAVCOL;

	typedef struct tagVertexNavigationTexture {
		tagVertexNavigationTexture() {
			ZeroMemory(this, sizeof(tagVertexNavigationTexture));
		}
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXNAVTEX;

	typedef struct tagVertexSphereColor {
		tagVertexSphereColor() {
			ZeroMemory(this, sizeof(tagVertexSphereColor));
		}
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;
	}VTXSPHERECOL;

	typedef struct tagVertexMatrix {
		tagVertexMatrix() {
			ZeroMemory(this, sizeof(tagVertexMatrix));
		}
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
	}VTXMATRIX;

	typedef struct tagVertex_Matrix_UV
	{
		tagVertex_Matrix_UV() {
			ZeroMemory(this, sizeof(tagVertex_Matrix_UV));
		}
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
		XMFLOAT4			vUV;
	}VTXMATRIX_UV;

	typedef struct tagVertex_Matrix_UV_Fade
	{
		tagVertex_Matrix_UV_Fade() {
			ZeroMemory(this, sizeof(tagVertex_Matrix_UV_Fade));
		}
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
		XMFLOAT4			vUV;
		XMFLOAT4			vFade;
	}VTXMATRIX_UV_FADE;

	typedef struct tagVertex_Matrix_UV_Fade_Direction
	{
		tagVertex_Matrix_UV_Fade_Direction() {
			ZeroMemory(this, sizeof(tagVertex_Matrix_UV_Fade_Direction));
		}
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
		XMFLOAT4			vUV;
		XMFLOAT4			vFade;
		XMFLOAT4			vDirection;
	}VTXMATRIX_UV_FADE_DIR;

	typedef struct tagFaceIndices16 {
		tagFaceIndices16() {
			ZeroMemory(this, sizeof(tagFaceIndices16));
		}
		unsigned short		_1, _2, _3;
	}FACEINDICES16;

	typedef struct tagFaceIndices32 {
		tagFaceIndices32() {
			ZeroMemory(this, sizeof(tagFaceIndices32));
		}
		unsigned long		_1, _2, _3;
	}FACEINDICES32;

	typedef struct tagLineStripIndices {
		tagLineStripIndices() {
			ZeroMemory(this, sizeof(tagLineStripIndices));
		}
		unsigned short		_1, _2, _3, _4;
	}LINESTRIPFACE16;

	typedef struct tagPacket {
		const unsigned long	checksum;
		void*				pData;
		const unsigned long end;

		tagPacket(const unsigned long& _checksum, void* _pData)
			: checksum(_checksum), pData(_pData), end(0)
		{
		}
	}PACKET;


	typedef struct tagImportParamGeneral
	{
		_uint	iMeshType = 0;	// Anim -> HasAnimation() 
		_uint	iNumVertices = 0;
		_uint	iNumIndices = 0;
		_uint	iNumMeshes = 0;	// IPMeshContainer -> CMeshContainer::Create() -> push_back
		_uint	iNumMaterials = 0;	//  IPMaterial-> new MESHMATERIAL
		_uint	iNumHierachyNodes = 0;	// IPHNode -> CHierarchyNode::Create() -> push_back
		_uint	iNumAnimations = 0;	// HNode, Create_Animation
	}IPGeneral;

	typedef struct tagImportParamVertex
	{
		void*	pVertices;	// 
		void*	pIndices;	//
	}IPVertex;	// Create_SkinnedMesh() memcpy()

	typedef struct tagImportParamMeshContainer
	{
		_uint	iNumVertices = 0;	// MC -> Create_VIBuffer()
		_uint	iStartFaceIndex = 0;
		_uint	iNumFaces = 0;
		_uint	iStartVertexIndex = 0;
		_uint	iMaterialIndex = 0;

		_uint			iNumBones = 0;	// For SetUp_HierarchyNodeToMeshContainer()
		vector<char*>	vecBoneName;	
	}IPMeshContainer;

	typedef struct tagImportParamMaterial
	{
		typedef struct tagTextureDesc
		{
			_uint	iTextureType = AI_TEXTURE_TYPE_MAX;
			char	szMaterialTexturePath[MAX_PATH] = "";
		}TEXDESC;

		//TEXDESC*	pMaterialTextures[AI_TEXTURE_TYPE_MAX];

		vector<TEXDESC>	vecTexDesc;

	}IPMaterial;	// IPMaterial -> CTexture::Create() -> pMeshMaterial->pMaterialTextures[iMaterialType]

	typedef struct tagImportParamHierarchyNode
	{
		char	szParentName[MAX_PATH];
		char	szName[MAX_PATH];
		_matrix	TransformationMatrix;
		_matrix OffsetMatrix;
		_uint	iDepth;
		_uint	iNumChildren;
	}IPHNode;	// szParentName linking, iDepth sorting

	typedef struct tagImportParamAnimation
	{
		char	szName[MAX_PATH];		// For.CAnimation::Create()
		_double	iDuration;
		_double	iTicksPerSecond;
		_uint	iNumChannels;
	}IPAnim;

	typedef struct tagImportParamChannel
	{
		char	szName[MAX_PATH];		// For.CChannel::Create()
		char	szTargetName[MAX_PATH];
		vector<KEYFRAME*>	vecKeyFrames;

		//  pAnim pHNode Add_Channel
	}IPChannel;

	typedef struct tagNavigationInfo {
		tagNavigationInfo() {
			ZeroMemory(this, sizeof(tagNavigationInfo));
		}
		enum class OPTION : _uint {
			NONE = 0,
			FALL = 1,
			SLOW = 2,
			COUNT = 3
		};

		XMFLOAT3			vPoint[3];
		OPTION				eOption;

	}NAVINFO;

	typedef struct tagCollisionTag {
		tagCollisionTag() {
			ZeroMemory(this, sizeof(tagCollisionTag));
		}

		tagCollisionTag(unsigned int iFirstLevel, const wchar_t* pFirstTag, unsigned int iFirstType, unsigned int iSecondLevel, const wchar_t* pSecondTag, unsigned int iSecondType) {
			Set_CollisionValue(iFirstLevel, pFirstTag, iFirstType, iSecondLevel, pSecondTag, iSecondType);
		}

		

		// First  
		unsigned	int			iFirstLevel;
		const		wchar_t*	pFirstTag;
		unsigned	int			iFirstType;
		// Second 
		unsigned	int			iSecondLevel;
		const		wchar_t*	pSecondTag;
		unsigned	int			iSecondType;

		void	Set_CollisionValue(unsigned int iFirstLevel, const wchar_t* pFirstTag, unsigned int iFirstType, unsigned int iSecondLevel, const wchar_t* pSecondTag, unsigned int iSecondType) {
			this->iFirstLevel = iFirstLevel;
			this->pFirstTag = pFirstTag;
			this->iFirstType = iFirstType;
			this->iSecondLevel = iSecondLevel;
			this->pSecondTag = pSecondTag;
			this->iSecondType = iSecondType;
		}

	}COLLISION_TAG;

	typedef struct tVIBuffer_PointInstance_Description
	{
		_float3		vDirection;
		_float		fDensity = 0.f;
		_float		fLifeTime = 0.f;
		_int		iLifeTimeDensity = 0;
		_float		fLifeTimeDensityEnd = 1.f;
		_float		fDirectionDensity = 0.f;

		_int		iNumber = 1;
		_float		fSpeed = 0.f;
		_float		fSpeedDensity = 0.f;
		_int		iWidth = 0;
		_int		iHeight = 0;
		_float		fSpriteInterval = 0.1f;

		_bool		bRepeat = false;
		_bool		bFadeIn = false;
		_float		fFadeInTime = 0.f;
		_bool		bFadeOut = false;
		_float		fFadeOutTime = 0.f;
		_float		fFadeOutEndTime = 0.f;

		_float		fPSize = 1.f;
	} POINTINST_DESC;

	typedef struct tagShaderName
	{
		TCHAR szName[MAX_PATH];
	} SHADERNAME;

	typedef struct tagParticleShaderDescription
	{
		vector<SHADERNAME*> vecShaderName;
	}PARTICLE_SHADER_DESC;


	typedef struct tVIBuffer_PointInstance_Description333
	{
		_float3		vDirection;
		_float		fDensity = 0.f;
		_float		fLifeTime = 0.f;
		_int		iLifeTimeDensity = 0;
		_float		fLifeTimeDensityEnd = 0.f;
		_float		fDirectionDensity = 0.f;

		_int		iNumber = 1;
		_float		fSpeed = 0.f;
		_float		fSpeedDensity = 0.f;
		_int		iWidth = 0;
		_int		iHeight = 0;
		_float		fSpriteInterval = 0.1f;

		_bool		bRepeat = false;
		_bool		bFadeIn = false;
		_float		fFadeInTime = 0.f;
		_bool		bFadeOut = false;
		_float		fFadeOutTime = 0.f;
		_float		fFadeOutEndTime = 0.f;
	} POINTINST_DESC333;

	typedef struct tagVertexTrails {
		tagVertexTrails() {
			ZeroMemory(this, sizeof(tagVertexTrails));
		}
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VERTEXTRAILS;


	typedef struct tParticleDescription
	{
		TCHAR					szParticleName[MAX_PATH];

		_float3					vPosition;
		_float3					vScale;
		_float3					vRotation;

		_int					iTextureNumber = 1;
		TCHAR					szTextureName[MAX_PATH];
		TCHAR					szTextureProto[MAX_PATH];
		TCHAR					szTexturePath[MAX_PATH];
		_float					fMultiTextureInterval = 0.1f;
		_bool					bMultiTextureRepeat = false;

		PARTICLE_SHADER_DESC	tParticleShaderDesc;
		POINTINST_DESC			tPointInst_Desc;
	} PARTICLEDESC;

	typedef struct MyShaderResourceView
	{
		char pConstantTextureName[MAX_PATH];
		ID3D11ShaderResourceView * pShaderResourceView;
	} MYSRV;

	typedef struct tagObjectStatusDesc
	{
		tagObjectStatusDesc() { ZeroMemory(this, sizeof(tagObjectStatusDesc)); }
		_float			fHp;
		_float			fMaxHp;
		_uint			iAtt;
		_uint			iDef;
	}OBJSTATUSDESC;

	typedef struct tagCellSortDesc
	{
		tagCellSortDesc() : fDist(0.f), pCell(nullptr) {}
		_float					fDist;
		class CCell*			pCell;
	}CELLSORTDESC;

	typedef struct tagItemDesc
	{
		_uint			iItemCategory = 0;
		_uint			iItemRarity = 0;
		_uint			iItemCode = 0;
		_uint			iItemCount = 0;
		const _tchar*	szIconElemKey = TEXT("");
		const _tchar*	szTextElemKey = TEXT("");
		const _tchar*	szDetailElemKey = TEXT("");
	}ITEMDESC;
}


#endif // !__ENGINE_STRUCT_H__