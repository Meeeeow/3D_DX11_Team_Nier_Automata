#pragma once
#ifndef __CLIENT_SOUND_MGR_H__
#define __CLIENT_SOUND_MGR_H__

#include "fmod.h"
#include "Client_Defines.h"

BEGIN(Client)
class CSoundMgr
{
public:
	static CSoundMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CSoundMgr; 

		return m_pInstance; 
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance; 
			m_pInstance = nullptr; 
		}
	}
public:
	enum CHANNELID {BGM, BGM2
		, BOMB, BOMB2, BOMB3, BOMB4
		, HACKING, VOICE, VOICE2, SCREAM, WALK, PUNCH, P_WEAPONSOUND , PLAYER , POD, HACKING_FINISHED , BULLET_1 , BULLET_2 , BULLET_3
		, MISSILE , MISSILE2 , MISSILE3 , PLAYER_EFFECT
		, BOSS , BOSS_EFFECT , ENVIRONMENT_SOUND , MONSTER1, MONSTER2, MONSTER3 , BRIDGE
		, UI
		, MAXCHANNEL};


private:
	CSoundMgr();
	~CSoundMgr();

public:
	void Initialize(); 
	void Release(); 

public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID);
	void PlayBGM(TCHAR* pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();
	void StopPlaySound(TCHAR* pSoundKey, CHANNELID eID);
	void PlayLoopSound(TCHAR* pSoundKey, CHANNELID eID);
	void PauseBGM(bool _bPause);

	void Set_Volume(CHANNELID eID, _float fVolume);
	void FadeOut_Volume(CHANNELID eID, _float* fVolume = nullptr);
	void FadeIn_Volume(CHANNELID eID, _float* fVolume = nullptr);

	const _float	Get_CrntVolume(CHANNELID eID);

private:
	void LoadSoundFile(); 

private:
	static CSoundMgr* m_pInstance; 

	map<TCHAR*, FMOD_SOUND*> m_mapSound; 

	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL]; 

	FMOD_SYSTEM* m_pSystem; 

	_float	m_fFadeEffectCrntVolume = 0.f;
	_float	m_fFadeEffectMaxVolume = 1.f;
};
END
#endif // !__CLIENT_SOUND_MGR_H__
