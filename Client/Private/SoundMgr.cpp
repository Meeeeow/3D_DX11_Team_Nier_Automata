#include "stdafx.h"
#include "../Public/SoundMgr.h"
#include <io.h>

CSoundMgr* CSoundMgr::m_pInstance = nullptr;
CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr; 
}


CSoundMgr::~CSoundMgr()
{
	Release(); 
}

void CSoundMgr::Initialize()
{
	FMOD_System_Create(&m_pSystem,FMOD_VERSION);
	
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile(); 
}
void CSoundMgr::Release()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear(); 
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

void CSoundMgr::PlaySound(TCHAR * pSoundKey, CHANNELID eID)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter; 

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter) 
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		VOIDMSG("CSoundMgr::PlaySound - iter == m_mapSound.end");

	FMOD_BOOL bPlay = FALSE; 
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem,iter->second,nullptr, FALSE, &m_pChannelArr[eID]);
	}
	FMOD_SOUND;
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(TCHAR * pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;
	FMOD_System_PlaySound(m_pSystem,iter->second,nullptr, FALSE, &m_pChannelArr[BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0 ; i < MAXCHANNEL ; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::StopPlaySound(TCHAR * pSoundKey, CHANNELID eID)
{
	StopSound(eID);
	PlaySound(pSoundKey, eID);
}

void CSoundMgr::PlayLoopSound(TCHAR * pSoundKey, CHANNELID eID)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PauseBGM(bool _bPause)
{
	FMOD_Channel_SetPaused(m_pChannelArr[BGM], _bPause);
}

void CSoundMgr::Set_Volume(CHANNELID eID, _float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
}

void CSoundMgr::FadeOut_Volume(CHANNELID eID, _float* fVolume)
{
	if (nullptr == fVolume)
	{
		FMOD_Channel_GetVolume(m_pChannelArr[eID], &m_fFadeEffectCrntVolume);

		if (0.f < m_fFadeEffectCrntVolume)
		{
			m_fFadeEffectCrntVolume -= 0.01f;

			if (m_fFadeEffectCrntVolume < 0.f)
			{
				m_fFadeEffectCrntVolume = 0.f;
			}
			FMOD_Channel_SetVolume(m_pChannelArr[eID], m_fFadeEffectCrntVolume);
		}
	}
	else
	{
		FMOD_Channel_GetVolume(m_pChannelArr[eID], &m_fFadeEffectCrntVolume);

		if (*fVolume <= m_fFadeEffectCrntVolume)
		{
			*fVolume -= 0.01f;

			if (*fVolume < 0.f)
			{
				*fVolume = 0.0f;
			}
			FMOD_Channel_SetVolume(m_pChannelArr[eID], *fVolume);
		}
	}
}

void CSoundMgr::FadeIn_Volume(CHANNELID eID, _float* fVolume)
{
	if (nullptr == fVolume)
	{
		FMOD_Channel_GetVolume(m_pChannelArr[eID], &m_fFadeEffectCrntVolume);

		if (m_fFadeEffectCrntVolume < 1.f)
		{
			m_fFadeEffectCrntVolume += 0.01f;

			if (1.f < m_fFadeEffectCrntVolume)
			{
				m_fFadeEffectCrntVolume = 1.f;
			}
			FMOD_Channel_SetVolume(m_pChannelArr[eID], m_fFadeEffectCrntVolume);
		}
	}
	else
	{
		FMOD_Channel_GetVolume(m_pChannelArr[eID], &m_fFadeEffectCrntVolume);

		if (m_fFadeEffectCrntVolume <= *fVolume)
		{
			m_fFadeEffectCrntVolume += 0.01f;

			if (*fVolume < m_fFadeEffectCrntVolume)
			{
				m_fFadeEffectCrntVolume = *fVolume;
			}
			FMOD_Channel_SetVolume(m_pChannelArr[eID], m_fFadeEffectCrntVolume);
		}
	}
}

const _float CSoundMgr::Get_CrntVolume(CHANNELID eID)
{
	_float	fCrntVolume = 0.f;

	FMOD_Channel_GetVolume(m_pChannelArr[eID], &fCrntVolume);

	return fCrntVolume;
}

void CSoundMgr::LoadSoundFile()
{
	_finddata_t fd; 

	__int64 handle = _findfirst("../bin/Resources/Sound/*.*", &fd);

	if (handle == 0)
		return; 

	int iResult = 0; 

	char szCurPath[128] = "../bin/Resources/Sound/";
	char szFullPath[128] = ""; 

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath); 
		strcat_s(szFullPath, fd.name);
		FMOD_SOUND* pSound = nullptr; 
		
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = (int)strlen(fd.name) + 1; 

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _findnext(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}
