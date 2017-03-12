#include "StdAfx.h"
#include "CHI_ADEC_DEC.h"
//#include "hi_pcdefs.h"
#include <string.h> 
//#include <windows.h>
//#define HI_INVALIDE 1
#include <windows.h>
#include <vfw.h>
//#include <vfwext.h>
//#include <msacmdrv.h>
#if 0
#ifdef _DEBUG
void traceprintf(const /*TCHAR*/char * format, ...)
{
    char szBuff[1024];
	va_list argptr;
	
    va_start( argptr, format );
    wvsprintf( szBuff, format, argptr );
    va_end( argptr );
	OutputDebugString(szBuff);
}

#define		TRACE	traceprintf

#endif
#endif
CHI_ADEC_DEC::CHI_ADEC_DEC()
{
    m_pDecCodeState = NULL;
    memset((HI_VOID*)&g711_dec_state, 0, sizeof(g711_dec_state));
    memset((HI_VOID*)&g726_dec_state, 0, sizeof(g726_dec_state));
}

CHI_ADEC_DEC::~CHI_ADEC_DEC()
{
    m_pDecCodeState = NULL;
}

HRESULT CHI_ADEC_DEC::HI_ADEC_Decode(HI_U8 *pInBuf,
                                      HI_S32 s32InLen,
                                      HI_U8 *pOutBuf,
                                      HI_S32 *pOutLen)
{
    if ((NULL == pInBuf) || (NULL == pOutBuf) || NULL == pOutLen)
    {
        return HI_ERR_NULL_POINTER;
    }

    HI_S32 s32Rel = 0;
    HI_S16 w16DataLength = 0;
//	TRACE("HI_ADEC_Decode!frame len =%d!\r\n",s32InLen);
    s32Rel = AudioDecode((HI_S16 *)pInBuf, (HI_S16 *)pOutBuf, &w16DataLength);
    if (HI_SUCCESS == s32Rel)
    {
        *pOutLen = w16DataLength + w16DataLength;
        return HI_SUCCESS;
    }
    else
    {
        *pOutLen = 0;
        return HI_ERR_ABORTED_OPERATION;
    }
}


HRESULT CHI_ADEC_DEC::HI_ADEC_DeInit()
{
    return HI_SUCCESS;
}


HRESULT CHI_ADEC_DEC::HI_ADEC_Init(PLAYER_ATTR_AUDIO_S *pAudioAttr,
                                    PLAYER_ATTR_AUDIO_S *pOutPutAttr)
{
    if (NULL == pAudioAttr || NULL == pOutPutAttr)
    {
        return HI_ERR_NULL_POINTER;
    }

    HI_U16 u16CodeType;
    if (PLAYER_AUDIO_CODEC_FORMAT_G711A == pAudioAttr->eAEncode)
    {
        m_pDecCodeState = &g711_dec_state;
        u16CodeType = G711_A;
    }
    else if (PLAYER_AUDIO_CODEC_FORMAT_G711U == pAudioAttr->eAEncode)
    {
        m_pDecCodeState = &g711_dec_state;
        u16CodeType = G711_U;
    }
    else if (PLAYER_AUDIO_CODEC_FORMAT_G726 == pAudioAttr->eAEncode)
    {
        m_pDecCodeState = &g726_dec_state;
        if (16000 == pAudioAttr->lBitRate)
        {
            u16CodeType = MEDIA_G726_16KBPS;
        }
        else if (24000 == pAudioAttr->lBitRate)
        {
            u16CodeType = MEDIA_G726_24KBPS;
        }
        else if (32000 == pAudioAttr->lBitRate)
        {
            u16CodeType = MEDIA_G726_32KBPS;
        }
        else if (40000 == pAudioAttr->lBitRate)
        {
            u16CodeType = MEDIA_G726_40KBPS;
        }
        else
        {
            return HI_ERR_INVALID_ARGUMENTS;
        }
    }
    else
    {
        return HI_ERR_INVALID_ARGUMENTS;
    }

    /*重新初始化解码*/
    HI_S32 s32Rel = HI_VOICE_DecReset(m_pDecCodeState, u16CodeType);
    if (HI_SUCCESS != s32Rel)
    {
        return HI_FAILURE;
    }

    /*pOutPutAttr->eAEncode	= PLAYER_AUDIO_CODEC_FORMAT_WAV;
    pOutPutAttr->lBitsPerSample = 16;
    pOutPutAttr->lChannels	= pAudioAttr->lChannels;
    pOutPutAttr->lSamplesPerSec = pAudioAttr->lSamplesPerSec;

    pOutPutAttr->lBlockAlign	= pOutPutAttr->lBitsPerSample * pOutPutAttr->lChannels / 8;
    pOutPutAttr->lBitRate	= pAudioAttr->lSamplesPerSec * pOutPutAttr->lBlockAlign * 8;
    pOutPutAttr->length	= 0;
    pOutPutAttr->pReserved	= NULL;
    pOutPutAttr->lFrameFlag	= 0;

	HI_ADEC_Init(pAudioAttr, pOutPutAttr);*/

    return HI_SUCCESS;
}


HI_S32 CHI_ADEC_DEC::AudioDecode(HI_S16 *pInBuf, \
                                  HI_S16 *pOutBuf, HI_S16 *pBufLen)
{
    if ((NULL == pInBuf) || (NULL == pOutBuf) || NULL == pBufLen)
    {   //非法长度
        return HI_ERR_NULL_POINTER;
    }

	HI_S32 s32Rel = HI_SUCCESS;
    /*音频解码，输入解码句柄、音频数据、解码类型、数据长度，输出wave数据*/
    s32Rel = HI_VOICE_DecodeFrame(m_pDecCodeState, pInBuf, pOutBuf, pBufLen);
    if (HI_SUCCESS != s32Rel)
    {
//		TRACE("HI_VOICE_DecodeFrame err!frame len =%d!error code=%x!\r\n",*pBufLen,s32Rel);
        return HI_ERR_ABORTED_OPERATION;
    }
    else
    {
//		TRACE("HI_VOICE_DecodeFrame success!frame len =%d!",*pBufLen);
        return HI_SUCCESS;
    }
}

