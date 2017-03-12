

#ifndef _CHI_ADEC_DEC_H_
#define _CHI_ADEC_DEC_H_

//#include "IHI_ADEC_Decoder.h"
#include "hi_voice_api.h"
/*
#ifdef _DEBUG
void		traceprintf(const TCHAR * format, ...);
#define		TRACE	traceprintf
#define		THIS_FILE          __FILE__
#define		ASSERT(f)	\
	if(!(f))		\
TRACE("*******Error At %s %d\n",__FILE__,__LINE__);	
#else
#define		TRACE
#define		ASSERT
#endif
*/

class CHI_Manager;

class CHI_ADEC_DEC// : public IHI_ADEC_Decoder
{
public:
    HRESULT HI_ADEC_Decode(HI_U8 *pInBuf,
                                   HI_S32 s32InLen,
                                   HI_U8 *pOutBuf,
                                   HI_S32 *pOutLen);
    HRESULT HI_ADEC_DeInit();
    HRESULT HI_ADEC_Init(PLAYER_ATTR_AUDIO_S *pAudioAttr,
                         PLAYER_ATTR_AUDIO_S *pOutPutAttr);

public:
    CHI_ADEC_DEC();
    virtual ~CHI_ADEC_DEC();

private:
    HI_S32 AudioDecode(HI_S16 *pInBuf, HI_S16 *pOutBuf, HI_S16 *pBufLen);
    hiVOICE_G711_STATE_S g711_dec_state;
    hiVOICE_G726_STATE_S g726_dec_state;
    HI_VOID *m_pDecCodeState;  /*Dec handle*/
};


#endif //_CHI_ADEC_DEC_H_
