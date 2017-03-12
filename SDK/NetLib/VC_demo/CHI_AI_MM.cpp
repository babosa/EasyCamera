#include "StdAfx.h"
#include "CHI_AI_MM.h"

extern int g_audioType;

#define PLAY_AUDIO_BUF_NUM 10

CHI_AI_MM::CHI_AI_MM(void)
{
    m_bPlaying	= HI_FALSE;
    m_hEvent	= NULL;
    
    memset(&m_waveCaps, 0, sizeof(m_waveCaps));
    memset(&m_Format, 0, sizeof(m_Format));
    m_mmr = 0;

	func_onAudioCallback = NULL;
    m_bDevOpen	= HI_FALSE;
    m_bStartData = HI_FALSE;
    m_bBuffer	= HI_FALSE;
    m_bMute = HI_FALSE;
    m_hWaveIn	= 0;
    m_pWaveHead = NULL;
	m_hFile = NULL;
	m_pAudio_ENC = NULL;
	m_pAudioAttr = NULL;
	m_pAudioAttr = (PLAYER_ATTR_AUDIO_S*)malloc(sizeof(PLAYER_ATTR_AUDIO_S));

    //Init Audio input Format
    m_Format.cbSize = 0; //sizeof(WAVEFORMATEX);
    m_Format.nChannels = 1;
    m_Format.nSamplesPerSec = 8000;
    m_Format.wBitsPerSample = 16;
    m_Format.wFormatTag = WAVE_FORMAT_PCM;
    m_Format.nBlockAlign = 2;
    m_Format.nAvgBytesPerSec = 16000;
	m_u64SampleTimeStamp = 0;

    InitializeCriticalSection(&m_critSection);
	InitializeCriticalSection(&m_callBack);
}

CHI_AI_MM::~CHI_AI_MM(void)
{
    try
    {
        (HI_VOID)Stop();
        m_pWaveHead = NULL;
        m_hWaveIn = NULL;

        if (NULL != m_hEvent)
        {
            CloseHandle(m_hEvent);
            m_hEvent = NULL;
        }

		m_pAudio_ENC = NULL;
		free(m_pAudioAttr);
		m_pAudioAttr = NULL;
		func_onAudioCallback = NULL;
        
        DeleteCriticalSection(&m_critSection);
		DeleteCriticalSection(&m_callBack);
    }
    catch (...)
    {}

}

HRESULT CHI_AI_MM::HI_AI_SetInFormat(const PLAYER_ATTR_AUDIO_S  struAStreamParam)
{
    m_Format.cbSize			 = 0; //sizeof(WAVEFORMATEX);
   	m_Format.nChannels       = (WORD)struAStreamParam.lChannels;
    m_Format.nSamplesPerSec  = (DWORD)struAStreamParam.lSamplesPerSec;
    m_Format.wBitsPerSample  = (WORD)struAStreamParam.lBitsPerSample;
    m_Format.wFormatTag      = WAVE_FORMAT_PCM;
   	m_Format.nBlockAlign     = (WORD)struAStreamParam.lBlockAlign;
   	m_Format.nAvgBytesPerSec = struAStreamParam.lBitRate/8 ;
    return HI_SUCCESS;
}


HRESULT CHI_AI_MM::HI_AI_GetInFormat(PLAYER_ATTR_AUDIO_S *pAStreamParam)
{
    if (NULL == pAStreamParam)
    {
        return HI_ERR_NULL_POINTER;
    }

   	pAStreamParam->lChannels      = m_Format.nChannels;
    pAStreamParam->lSamplesPerSec = m_Format.nSamplesPerSec;
    pAStreamParam->lBitsPerSample = m_Format.wBitsPerSample;
   	pAStreamParam->lBlockAlign    = m_Format.nBlockAlign;

    return HI_SUCCESS;
}

HRESULT CHI_AI_MM::HI_AI_Stop()
{
	if (NULL != m_pAudio_ENC)
    {
        m_pAudio_ENC->HI_AENC_DeInit();
        delete m_pAudio_ENC;
        m_pAudio_ENC = NULL;
    }

	EnterCriticalSection(&m_callBack);
	func_onAudioCallback = NULL;
	LeaveCriticalSection(&m_callBack);

    return Stop();
}


HRESULT CHI_AI_MM::HI_AI_Start(OnAudioCallback callBack, HI_VOID *pUserData)
{
	EnterCriticalSection(&m_callBack);
	func_onAudioCallback = callBack;
	LeaveCriticalSection(&m_callBack);

	if(m_pUserData != pUserData)
	{
		m_pUserData = pUserData;
	}

	m_pAudio_ENC = new CHI_AENC_ENC();
    if (NULL == m_pAudio_ENC)
    {
        return HI_ERR_NULL_POINTER;
    }

	PLAYER_ATTR_AUDIO_S audioAttr;

	if(g_audioType == 1)
	{
		//G726
		audioAttr.eAEncode = PLAYER_AUDIO_CODEC_FORMAT_G726;
		audioAttr.lBitRate = 16000;
		audioAttr.lBitsPerSample = 2;
		audioAttr.lBlockAlign = 1;
	}
	else
	{
		//G711
		audioAttr.eAEncode = PLAYER_AUDIO_CODEC_FORMAT_G711A;
		audioAttr.lBitRate = 64000;
		audioAttr.lBitsPerSample = 16;
		audioAttr.lBlockAlign = 2;
	}
	audioAttr.lChannels = 1;
	audioAttr.length = 0;
	audioAttr.lFrameFlag = 0;
	audioAttr.lSamplesPerSec = 8000;
	audioAttr.pReserved = 0;
    HRESULT hr = m_pAudio_ENC->HI_AENC_Init(&audioAttr);
    if (HI_SUCCESS != hr)
    {
        (HI_VOID)m_pAudio_ENC->HI_AENC_DeInit();
        delete m_pAudio_ENC;
        m_pAudio_ENC = NULL;
        return hr;
    }

    return Start();
}


/************************************************
* Function Name	     : AudioInProc
* Description	     : Audio Callback Function 
* Return Type        : void CALLBACK 
* Parameters         : HWAVEIN hwi
* Parameters         : UINT uMsg
* Parameters         : DWORD dwInstance
* Parameters         : DWORD dwParam1
* Parameters         : DWORD dwParam2
* Last Modified      : 2006-4-30 9:17:15
************************************************/
HI_VOID CALLBACK AudioInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if (MM_WIM_DATA == uMsg)
    {
        WAVEHDR *pWaveHdr = (WAVEHDR *)dwParam1;
        CHI_AI_MM *pAudioIn = (CHI_AI_MM *)(pWaveHdr->dwUser);
        if (NULL == pAudioIn)
        {
            return ;
        }
        
        EnterCriticalSection(&pAudioIn->m_critSection);
        if (HI_TRUE == pAudioIn->m_bPlaying)
        {
            LeaveCriticalSection(&pAudioIn->m_critSection);
            if (pWaveHdr && hwi)
            {
                if ((pWaveHdr->dwFlags & WHDR_DONE) == WHDR_DONE)
                {
                    if (pAudioIn->IsError(waveInUnprepareHeader(hwi, pWaveHdr, sizeof(WAVEHDR))))
                    {
                        return;
                    }
                    if (pWaveHdr->dwBytesRecorded > 0)
                    {
                        HI_AI_Buffer buff(pWaveHdr->lpData, pWaveHdr->dwBytesRecorded);

                        pAudioIn->HI_AI_Encoder((HI_U8*)pWaveHdr->lpData,
												pWaveHdr->dwBufferLength, 
												pAudioIn->GetTimeStamp());
                        //pWaveHdr->dwFlags = 0 ;
						//printf("%d, %u\n", pWaveHdr->dwBufferLength, pAudioIn->GetTimeStamp());
                        waveInPrepareHeader(hwi, pWaveHdr, sizeof(WAVEHDR));
                        waveInAddBuffer(hwi, pWaveHdr, sizeof(WAVEHDR));
                    }
                }
            }
        }
        else
        {
            LeaveCriticalSection(&pAudioIn->m_critSection);
            SetEvent(pAudioIn->m_hEvent);
        }
    }
}


HI_BOOL CHI_AI_MM::OpenDevice()
{
    if (HI_TRUE == m_bDevOpen )
    {
        return HI_FALSE;
    }

    m_mmr = waveInOpen(0, WAVE_MAPPER, &m_Format, 0, 0, WAVE_FORMAT_QUERY);
    if ( MMSYSERR_NOERROR != m_mmr )
    {
        return HI_FALSE;
    }

    m_mmr = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_Format, (DWORD)AudioInProc, 0, CALLBACK_FUNCTION);
    if ( MMSYSERR_NOERROR != m_mmr )
    {
        return HI_FALSE;
    }

    m_bDevOpen = HI_TRUE;

    return HI_TRUE;
}


HI_VOID CHI_AI_MM::CloseDevice()
{
    if (HI_FALSE == m_bDevOpen)
    {
        return ;
    }

    if (!m_hWaveIn)
    {
        return ;
    }

    m_mmr = waveInClose(m_hWaveIn);
    if ( MMSYSERR_NOERROR != m_mmr )
    {
        return ;
    }

    m_hWaveIn = 0;
    m_bDevOpen = HI_FALSE;
}


HI_BOOL CHI_AI_MM::PerpareBuffer()
{
    if (HI_TRUE == m_bBuffer)
    {
        return HI_FALSE;
    }

    m_mmr = waveInReset(m_hWaveIn);
    if ( MMSYSERR_NOERROR != m_mmr )
    {
        return HI_FALSE;
    }

    if (NULL == m_pWaveHead)
    {
        m_pWaveHead = new WAVEHDR[PLAY_AUDIO_BUF_NUM];
        if (NULL == m_pWaveHead)
        {
            return HI_FALSE;
        }
        for (int i = 0; i < PLAY_AUDIO_BUF_NUM; i++ )
        {
            ZeroMemory(&m_pWaveHead[i], sizeof(WAVEHDR));

            HI_AI_Buffer buff(320, HI_FALSE); //m_Format.nBlockAlign * m_BufferSize
            m_pWaveHead[i].lpData = buff.ptr.c;
            m_pWaveHead[i].dwBufferLength = buff.m_u32ByteLen;
            m_pWaveHead[i].dwFlags = 0;
            m_pWaveHead[i].dwUser = (DWORD)(void *)this;
            waveInPrepareHeader(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
            waveInAddBuffer(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
        }
    }
    else
    {
        for (int i = 0 ; i < PLAY_AUDIO_BUF_NUM ; i++ )
        {
            waveInPrepareHeader(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
            waveInAddBuffer(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
        }
    }
    m_bBuffer = HI_TRUE;
    return HI_TRUE;
}


HI_VOID CHI_AI_MM::UnperpareBuffer()
{
    if (HI_FALSE == m_bBuffer)
    {
        return ;
    }

    if (NULL == m_pWaveHead)
    {
        return ;
    }

    for (int i = 0 ; i < PLAY_AUDIO_BUF_NUM ; i++ )
    {
        m_mmr = waveInUnprepareHeader(m_hWaveIn, &m_pWaveHead[i], sizeof(WAVEHDR));
        if ( MMSYSERR_NOERROR != m_mmr )
        {
            continue;
        }
    }

    m_bBuffer = HI_FALSE;
}

HI_VOID CHI_AI_MM::FreeBuffer()
{
    if (NULL == m_pWaveHead)
    {
        return ;
    }

    for (int i = 0 ; i < PLAY_AUDIO_BUF_NUM ; i++ )
    {
        SAFE_DELETEA(m_pWaveHead[i].lpData);
    }

    delete[](m_pWaveHead);
    m_pWaveHead = NULL;
}


HI_S32 CHI_AI_MM::Start()
{
    if (HI_TRUE == m_bPlaying)
    {
        /*已经处于播放状态*/
        return HI_SUCCESS;
    }
    
    HI_S32 s32Ret = HI_FAILURE;
    if (HI_FALSE == OpenDevice())
    {
        return HI_FALSE;
    }
    if (HI_FALSE == PerpareBuffer())
    {
        CloseDevice();
        return HI_FALSE;
    }

    if (HI_SUCCESS != StartData())
    {
        UnperpareBuffer();
        CloseDevice();
        return HI_FALSE;
    }

    EnterCriticalSection(&m_critSection);
    m_bPlaying = HI_TRUE;
    LeaveCriticalSection(&m_critSection);
    if (NULL == m_hEvent)
    {
        m_hEvent = CreateEvent(
            NULL,               // no security attributes
            TRUE,               // manual-reset event
            FALSE,              // initial state is signaled
            NULL          // object name
            );
    }
    
    return HI_SUCCESS;
}

HI_S32 CHI_AI_MM::Stop()
{
    if (HI_FALSE == m_bPlaying)
    {
        /*没有开启播放，不能执行stop操作*/
        return HI_SUCCESS;
    }
    
    /*先将播放标志为置为false*/
    EnterCriticalSection(&m_critSection);
    m_bPlaying = HI_FALSE;
    LeaveCriticalSection(&m_critSection);
    WaitForSingleObject(m_hEvent, INFINITE);
    ResetEvent(m_hEvent);

    //关闭
    (HI_VOID)CloseData();
    Sleep(30);
    UnperpareBuffer();
    Sleep(10);
    CloseDevice();
    Sleep(10);
    FreeBuffer();
	m_u64SampleTimeStamp = 0;
    return HI_SUCCESS;
}

HI_S32 CHI_AI_MM::StartData()
{
    if (HI_TRUE == m_bStartData)
    {
        return HI_FAILURE;
    }

    if (!m_hWaveIn)
    {
        return HI_FAILURE;
    }

    m_mmr = waveInStart(m_hWaveIn);
    if ( MMSYSERR_NOERROR != m_mmr )
    {
        return HI_FAILURE;
    }

    m_bStartData = HI_TRUE;
    return HI_SUCCESS;
}

HI_S32 CHI_AI_MM::CloseData()
{
    if ( HI_FALSE == m_bStartData )
    {
        return HI_FAILURE;
    }

    if ( !m_hWaveIn )
    {
        return HI_FAILURE;
    }

    m_bStartData = HI_FALSE;   	 //important
    m_mmr = waveInReset(m_hWaveIn);  //m_mmr = waveInStop(m_hWaveIn);
    if ( MMSYSERR_NOERROR != m_mmr )
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_BOOL CHI_AI_MM::IsExistDevice()
{
    if ( waveInGetNumDevs() <= 0 )
    {
        return HI_FALSE;
    }
    return HI_TRUE;
}

WAVEINCAPS* CHI_AI_MM::GetDeviceCap()
{
    m_mmr = waveInGetDevCaps(WAVE_MAPPER, &m_waveCaps, sizeof(m_waveCaps));
    if ( MMSYSERR_NOERROR != m_mmr )
    {
        return &m_waveCaps;
    }
    return NULL;
}


HI_BOOL CHI_AI_MM::IsOpen(void)
{
    return m_bDevOpen;
}


HI_BOOL CHI_AI_MM::IsError(MMRESULT nResult)
{
    UINT m_nError = nResult;

    if ( MMSYSERR_NOERROR == m_nError )
    {
        return HI_FALSE;
    }
    else
    {
        return HI_TRUE;
    }
}

HI_U64  CHI_AI_MM::GetTimeStamp()
{
	 HI_U64 u64ts = m_u64SampleTimeStamp;
	 m_u64SampleTimeStamp += 20;
	 return u64ts;
}


HRESULT CHI_AI_MM::HI_AI_Encoder(HI_U8 *pInBuf, HI_S32 s32InLen, HI_U64 u64TimeStamp)
{
    HRESULT hr = HI_SUCCESS;
	
	HI_U8 pOutBuf[1024] = {0};
	HI_S32 s32OutLen = 0;
    hr = m_pAudio_ENC->HI_AENC_Encoder(pInBuf, s32InLen, pOutBuf, &s32OutLen);
    if (HI_SUCCESS != hr)
    {
        return hr;
    }

	EnterCriticalSection(&m_callBack);
	if(NULL != func_onAudioCallback)
	{
		func_onAudioCallback(pOutBuf, s32OutLen, u64TimeStamp, m_pUserData);
	}
	LeaveCriticalSection(&m_callBack);
		
	return HI_SUCCESS;
}
