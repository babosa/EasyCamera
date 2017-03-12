#ifndef _CHI_AENC_ENC_H_
#define _CHI_AENC_ENC_H_

#include "hi_voice_api.h"



class CHI_AENC_ENC
{
public:
    HRESULT HI_AENC_Init(PLAYER_ATTR_AUDIO_S *pAudioAttr);
    HRESULT HI_AENC_DeInit();
    HRESULT HI_AENC_Encoder(HI_U8 *pInBuf,
                                    HI_S32 s32InLen,
                                    HI_U8 *pOutBuf,
                                    HI_S32 *pOutLen);

public:
    CHI_AENC_ENC();
    virtual ~CHI_AENC_ENC();

private:
    HI_VOID *m_pAEncHandle;
    hiVOICE_G711_STATE_S g711_dec_state;
    hiVOICE_ADPCM_STATE_S adpcm_dec_state;
    hiVOICE_G726_STATE_S g726_dec_state;
};


#endif //_CHI_AENC_G726_H_
