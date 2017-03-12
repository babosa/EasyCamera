#ifndef _CHI_AO_MM_H_
#define _CHI_AO_MM_H_

#include "CHI_COMMON_RingBuffer.h"
#include "HI_AI_Buffer.h"
#include "CHI_ADEC_DEC.h"

#define PLAY_AUDIO_BUF_NUM 5

class CHI_AO_MM
{
public:
    CHI_AO_MM();
    virtual ~CHI_AO_MM();
    //the A/V module call the function to get the time-stamp
    HRESULT HI_AO_SetOutFormat(PLAYER_ATTR_AUDIO_S* pstruAStreamParam);
    HRESULT HI_AO_GetOutFormat(PLAYER_ATTR_AUDIO_S *pAStreamParam);
    HRESULT HI_AO_Play(HI_U32 u32Frequency);
    HRESULT HI_AO_Stop(HI_VOID);
    HRESULT HI_AO_SetMute(HI_BOOL bMute);
    HRESULT HI_AO_GetMuteState(HI_BOOL *pMuteState);
    HI_S32 HI_AO_ClearRb(HI_VOID);
	HRESULT HI_ADEC_InPutAudioData(HI_U8 *pBuffer, HI_S32 s32DataLen, HI_U64 u64TimeStamp);

public:
    //output audio
    HI_S32 AudioStart(HI_VOID);
    HI_S32 AudioOut(HWAVEOUT hwo, CHI_AO_MM *pAudioOut, WAVEHDR *pWaveHdr);
    HI_S32 AudioStop(HI_VOID);
    //we should update the time-stamp value when audio out
    HI_S32 SetAudioPTS(HI_U8 **pU8Data);
    HI_S32 SetAudioPTSVal(HI_VOID);
    HI_S32 AddNewHeader(HWAVEOUT hwo, HI_AI_Buffer *buffer, HI_S32 s32Index);
    HI_S32 AddNewHeader(HWAVEOUT hwo, HI_AI_Buffer *buffer, WAVEHDR *pWaveHead);
    //HI_S32      HI_AoOutput_GetCtrl(CHsPlayerCtrl *pCtrl);

    HI_U32 m_u32TSArray[PLAY_AUDIO_BUF_NUM];   /*时间戳数组*/
    HI_U32	m_u32ReadLen;       /*每次从Rb中读取的音频数据长度*/
    HI_S32 m_s32RightNum;       /*同步参数上限*/
    HI_S32 m_s32LeftNum;        /*同步参数下限*/
    HI_U64 m_u64TimeStamp;      /*当前播放的音频数据块的时间戳*/
	HI_U64 m_u64CurTimeStamp;
	HI_S32 m_s32NeedPkNum;		/*通过视频RB计算音频中RB个数*/
    HI_S32 m_s32SavePackage;    /*音频Rb预存储的音频包个数*/
    HI_S32 m_s32PackageNum;     /*每次从Rb中返回的音频包个数*/
    HI_S32 m_s32Number;         /*时间戳计数，即当前时间戳在时间戳数组里面的序号*/
    HI_BOOL m_bMuteState;       /*静音状态*/
    HI_U8 *m_pAudioData;        /*每次从Rb中读取的音频时间戳*/
    UINT m_uTimerID;            /*音频包个数检查线程ID*/
    MMRESULT	m_mmr;
    volatile HI_BOOL m_bPlaying;  /*当前是否为播放状态*/
    HANDLE m_hEvent;              //事件信号
    CRITICAL_SECTION m_critSection;  //临界
    WAVEHDR	*m_pWaveHead;       /*音频数据头，数组*/
    CHI_COMMON_RingBuffer m_COMMONRingBuffer; /*音频Rb对象*/
	CHI_ADEC_DEC *m_pAudioDec;
//    CHI_AO_Initial m_AOInitial;        /*初始化类对象*/
    //	FILE*	m_pDumpFIle;
private:
    HI_S32	CloseData(HI_VOID);
    HI_S32	CloseDevice(HI_VOID);
    HI_S32	UnperpareBuffer(HI_VOID);
    HI_S32	OpenDevice(HI_VOID);
    HI_S32	PrepareHeader(HI_VOID);
    WAVEOUTCAPS* GetDeviceCap(HI_VOID);

    DWORD m_dwVolume;         /*当前音频播放音量*/
    HI_BOOL	m_bBuffer;
    HI_BOOL	m_bDevOpen;         /*播放设备是否打开*/

    WAVEFORMATEX m_pWaveFormat;     /*音频输出格式*/
    WAVEOUTCAPS	m_struWaveCaps;
    HWAVEOUT m_hWaveOut;         /*输出设备句柄*/

    HI_S32 m_s32PlayStamp;     /*播放的时间戳序号*/
    HI_BOOL m_bFistLostPack;
    UINT m_uAccuracy;
    //HI_S32			    m_s32IndexWaveHdr;  /*数据块索引号*/

    //暂停与恢复播放标识
    HI_BOOL m_bPause;
	HI_BOOL m_bStream;

    /**
     * 以下将 CHI_AO_MM 的默认拷贝构造和 "=" 操作符重载,
     * 并将访问权限设置为private类型,防止用户使用默认拷贝构造和 "=" 操作符.
     * @see PROTECTMIC
     */
    PROTECTMIC(CHI_AO_MM);
};


#endif //_CHI_AO_MM_H_
