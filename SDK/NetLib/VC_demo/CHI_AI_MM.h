
#ifndef _CHI_AI_MM_H_
#define _CHI_AI_MM_H_

#include "StdAfx.h"
#include "HI_AI_Buffer.h"
#include "CHI_AENC_ENC.h"


typedef HI_S32 (WINAPI* OnAudioCallback)(HI_U8 *pBuf,
										   HI_S32 s32Size,
										   HI_U64 u64TimeStamp,
										   HI_VOID *pUserData);


class CHI_AI_MM
{
    friend void CALLBACK AudioInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance,
                                     DWORD dwParam1, DWORD dwParam2);
public:
    CHI_AI_MM(void);
    virtual ~CHI_AI_MM(void);

    HRESULT HI_AI_Stop();
    HRESULT HI_AI_Start(OnAudioCallback callback, HI_VOID *pUserData);

    HRESULT HI_AI_SetInFormat(const PLAYER_ATTR_AUDIO_S struAStreamParam);
    HRESULT HI_AI_GetInFormat(PLAYER_ATTR_AUDIO_S *pAStreamParam);
	HRESULT HI_AI_Encoder(HI_U8 *pInBuf, HI_S32 s32InLen, HI_U64 u64TimeStamp);

private:
    //Audio Input parameter
    HI_BOOL IsError(MMRESULT nResult);
    HI_BOOL OpenDevice();
    HI_VOID CloseDevice();
    HI_BOOL PerpareBuffer();
    HI_VOID UnperpareBuffer();
    HI_S32 StartData();
    HI_S32 CloseData();
    HI_VOID FreeBuffer();
	HI_U64  GetTimeStamp();
    HI_BOOL	m_bDevOpen;
    HI_BOOL	m_bStartData;
    HI_BOOL	m_bBuffer;
    HI_BOOL	m_bMute;

    WAVEINCAPS m_waveCaps;
    HWAVEIN	m_hWaveIn;
    WAVEHDR	*m_pWaveHead;
    MMRESULT	m_mmr;

    WAVEFORMATEX m_Format;

    HI_S32 Start();
    HI_S32 Stop();
    HI_BOOL IsOpen();
    HI_BOOL IsExistDevice(void);
    WAVEINCAPS *GetDeviceCap(void);

    volatile HI_BOOL m_bPlaying;  /*当前是否为播放状态*/
    HANDLE m_hEvent;              //事件信号
    CRITICAL_SECTION m_critSection;  //临界
	CRITICAL_SECTION m_callBack;
    
public:
	HI_U64		m_u64SampleTimeStamp;
	FILE		*m_hFile;
	CHI_AENC_ENC *m_pAudio_ENC;
	PLAYER_ATTR_AUDIO_S* m_pAudioAttr;
	OnAudioCallback func_onAudioCallback;
	HI_VOID *m_pUserData;
};


#endif //_CHI_AI_MM_H_
