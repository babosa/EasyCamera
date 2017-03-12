#include "StdAfx.h"
#include "CHI_AENC_ENC.h"

CHI_AENC_ENC::CHI_AENC_ENC()
{
    m_pAEncHandle = NULL;
    memset((HI_VOID*)&g711_dec_state, 0, sizeof(g711_dec_state));
    memset((HI_VOID*)&adpcm_dec_state, 0, sizeof(adpcm_dec_state));
    memset((HI_VOID*)&g726_dec_state, 0, sizeof(g726_dec_state));
}

CHI_AENC_ENC::~CHI_AENC_ENC()
{
    m_pAEncHandle = NULL;
}

HRESULT CHI_AENC_ENC::HI_AENC_Init(PLAYER_ATTR_AUDIO_S *pAudioAttr)
{
    HI_S32 s32Rel;
    HI_U16 u16CodeType;

    if (NULL == pAudioAttr)
    {
        return HI_ERR_NULL_POINTER;
    }

    if (PLAYER_AUDIO_CODEC_FORMAT_G711A == pAudioAttr->eAEncode)
    {
        m_pAEncHandle = &g711_dec_state;
        u16CodeType = G711_A;
    }
    else if (PLAYER_AUDIO_CODEC_FORMAT_G711U == pAudioAttr->eAEncode)
    {
        m_pAEncHandle = &g711_dec_state;
        u16CodeType = G711_U;
    }
    else if (PLAYER_AUDIO_CODEC_FORMAT_G726 == pAudioAttr->eAEncode)
    {
        m_pAEncHandle = &g726_dec_state;
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
    s32Rel = HI_VOICE_EncReset(m_pAEncHandle, u16CodeType);

    return (s32Rel == HI_SUCCESS) ? HI_SUCCESS : HI_FAILURE;
}


HRESULT CHI_AENC_ENC::HI_AENC_DeInit()
{
    return HI_SUCCESS;
}

HRESULT CHI_AENC_ENC::HI_AENC_Encoder(HI_U8 *pInBuf,
                                       HI_S32 s32InLen,
                                       HI_U8 *pOutBuf,
                                       HI_S32 *pOutLen)
{
    if ((NULL == pInBuf)
        || (NULL == pOutBuf)
        || NULL == pOutLen
        || NULL == m_pAEncHandle)
    {
        return HI_ERR_NULL_POINTER;
    }
    if (s32InLen <= 0 || s32InLen > PLAY_AUDIO_FRAME_SIZE)
    {
        return HI_ERR_INVALID_ARGUMENTS;
    }

    HI_S32 s32Rel = HI_VOICE_EncodeFrame((HI_VOID*)m_pAEncHandle,
                                         (HI_S16*)pInBuf,
                                         (HI_S16*)pOutBuf,
                                         (HI_S16)s32InLen/2);
    if (HI_SUCCESS == s32Rel)
    {
        //码流数据加上海思帧结构头
        *pOutLen = (pOutBuf[2])*2 + 4;
        return HI_SUCCESS;
    }
    else
    {
        *pOutLen = 0;
        return HI_ERR_ABORTED_OPERATION;
    }
}

