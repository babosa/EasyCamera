
#ifndef _HI_HSPLAYER_H_
#define _HI_HSPLAYER_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#if defined(HSPLAYER_EXPORTS)
#define HSPLAYER_API      __declspec( dllexport )
#define HSPLAYER_APICALL  __stdcall

#elif	defined(HSPLAYER_LIB)
#define HSPLAYER_API      
#define HSPLAYER_APICALL  

#else 
#define HSPLAYER_API      __declspec( dllimport )
#define HSPLAYER_APICALL  __stdcall
#endif

#ifndef __HI_TYPE_H__
#include "hi_type.h"
#else

#endif

#define PLAYHANDLE void*
#define HSPLAYER_CALLBACK  __stdcall

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
    typedef long HRESULT;
#endif

#define PLAYER_FORMAT_NAME_LEN          256
#define PLAYER_MAX_FILE_NAME            512

#define HI3511 1 // 090708
#ifdef HI3511// 090708
#define PLAYER_MAX_WIDTH                1920
#define PLAYER_MAX_HEIGHT               1080
#else
#define PLAYER_MAX_WIDTH                1920
#define PLAYER_MAX_HEIGHT               1080
#endif

#define PLAYER_MIN_BMPBUF_SIZE          (PLAYER_MAX_WIDTH*PLAYER_MAX_HEIGHT*3+54)
#define PLAYER_MIN_JPEGBUF_SIZE         (PLAYER_MAX_WIDTH*PLAYER_MAX_HEIGHT*3>>1)

#define HI_ERR_VERSION	                0xA1888006  //version erro
#define HI_ERR_NULL_POINTER	            0xA1888007  //input pointer is null
#define HI_ERR_INVALID_ARGUMENTS        0xA1888008  //input parameter is invalid
#define HI_ERR_OVERFLOW_MEMORY	        0xA1888009  //memory overlow
#define HI_ERR_INVALID_HANDLE	        0xA188800A  //handle is erro
#define HI_ERR_ABORTED_OPERATION        0xA188800B  //operation failed
#define HI_ERR_INVALID_OPERATION        0xA188800C  //operation invalid
#define HI_ERR_NOT_INIT	                0xA188800D  //not init
#define HI_ERR_RESEND_STREAM	        0xA188800E  //send stream again
#define HI_ERR_INVALID_STATE            0Xa188800F

#define HI_ERR_AUDIOCODEC_UNSUPPORT     0xA1888011
#define HI_ERR_VIDEOCODEC_UNSUPPORT     0xA1888012
#define HI_ERR_INVALID_FILE             0xA1888013
#define HI_ERR_INTERIOR                 0xA1888014
#define HI_ERR_OUTOFSEEKTIME            0xA1888015
#define HI_ERR_INVALIDINDEXOBJ          0xA1888016

#define HI_ERR_OPEN_FILE				0xA1888017
#define HI_ERR_ENDFILE					0xA1888018
#define HI_ERR_READ_FRAME				0xA1888019
#define HI_ERR_INVALID_STREAM           0xA188801a
#define HI_ERR_UNSUPPORT_CODEC          0xA188801B
#define HI_ERR_NEEDMOREDATA             0xA188801c

#define HI_ERR_VIDEOOUT_FAILURE                     0xA1888031  //erro inside
#define HI_ERR_VIDEOOUT_NULL_POINTER                0xA1888032  //input point null
#define HI_ERR_VIDEOOUT_INVALID_ARGUMENTS           0xA1888033  //input para is erro
#define	HI_ERR_VIDEOOUT_NO_HARDWARE                 0xA1888034  //no hardware support
#define	HI_ERR_VIDEOOUT_GET_SNAPDATA                0xA1888035  //get snap data failed
#define	HI_ERR_VIDEOOUT_TIME_OUT                    0xA1888036  //over time
#define HI_ERR_VIDEOOUT_DRAWYUV_WINITHWND           0xA1888037  //DrawYUV handle is null and not init
#define	HI_ERR_VIDEOOUT_DRAWYUV_BUFNULL             0xA1888038  //DrawYUV surface is null
#define	HI_ERR_VIDEOOUT_INITDDRAW_CREATEDRAW        0xA1888039  //create surface failed
#define	HI_ERR_VIDEOOUT_INITDDRAW_SETCOPLEVEL       0xA188803A  //set draw modle is failed
#define	HI_ERR_VIDEOOUT_INITDDRAW_GETCAPS           0xA188803B  //failed get caps of hardware
#define	HI_ERR_VIDEOOUT_INITDDRAW_CREATEMAINSURFACE 0xA188803C  //create main surface failed
#define	HI_ERR_VIDEOOUT_INITDDRAW_GETBACKSURFACE    0xA188803D  //failed get the attr of surface
#define	HI_ERR_VIDEOOUT_INITDDRAW_CREATEBACKSURFACE 0xA188803E  //failed create back surface
#define	HI_ERR_VIDEOOUT_INITDDRAW_CREATEOFFSURFACE  0xA188803F  //failed create off surface
#define	HI_ERR_VIDEOOUT_INITDDRAW_CREATECLIPPER     0xA1888040  //faied create clipper
#define	HI_ERR_VIDEOOUT_INITDDRAW_BLT               0xA1888041  //clear surface failed
#define	HI_ERR_VIDEOOUT_DRAWPCOSD_GETDC             0xA1888042  //get the draw handle failed
#define HI_ERR_VIDEOOUT_DRAWYUV_LOACK               0xA1888043  //loack off surface failed

#define HI_ERR_PB_STOPPLAY							0xA1888050  
#define HI_ERR_VDEC_NOPIC							0xA1888051
#define HI_ERR_FILE_END								0xA1888052
	
#define HI_NET_DEV_VIDEO_FRAME_FLAG					0x46565848
#define HI_NET_DEV_AUDIO_FRAME_FLAG					0x46415848

#define HI_STREAM_OVER								0x5245564F

#define HI_STREAM_REALTIME	0
#define HI_STREAM_FILE		1

	typedef struct 
	{
		HI_U32 u32AVFrameFlag;  /* 音频帧标志*/
		HI_U32 u32AVFrameLen;   /* 音频帧的长度*/
		HI_U32 u32AVFramePTS;	/* 时间戳 */
		HI_U32 u32VFrameType;   /* 视频的类型，I帧或P帧*/
	} HI_S_AVFrame;
	
	typedef struct
	{
		HI_U32 u32Width;
		HI_U32 u32Height;
	} HI_S_VideoHeader;
	
	typedef struct
	{
		HI_U32 u32Format;       /*音频格式*/
	} HI_S_AudioHeader;
	
	typedef struct 
	{
		HI_U32 u32SysFlag;
		HI_S_VideoHeader struVHeader;
		HI_S_AudioHeader struAHeader;
	} HI_S_SysHeader;

    //video encoding format
    typedef enum hiPLAYER_VIDEO_FORMAT_E
    {
        PLAYER_VENC_FORMAT_H261 = 0,       // H261
        PLAYER_VENC_FORMAT_H263,           // H263
        PLAYER_VENC_FORMAT_MPEG2,          // MPEG2
        PLAYER_VENC_FORMAT_MPEG4,          // MPEG4
        PLAYER_VENC_FORMAT_H264,           // H264
        PLAYER_VENC_FORMAT_MJPEG,          // MJPEG
        PLAYER_VENC_FORMAT_BUTT            // reserve
    }PLAYER_VIDEO_FORMAT_E, VIDEO_FORMAT_E;

    //audio encoding format
    typedef enum hiPLAYER_AUDIO_FORMAT_E
    {
        PLAYER_AUDIO_CODEC_FORMAT_G711A = 1,      //G.711 A
        PLAYER_AUDIO_CODEC_FORMAT_G711U,          //G.711 U
        PLAYER_AUDIO_CODEC_FORMAT_ADPCM,          //ADPCM
        PLAYER_AUDIO_CODEC_FORMAT_G726,           //G.726
        PLAYER_AUDIO_CODEC_FORMAT_AMR,            //AMR
        PLAYER_AUDIO_CODEC_FORMAT_AMRDTX,         //AMR encoder formant and VAD1 enable
        PLAYER_AUDIO_CODEC_FORMAT_AAC,            //aac
        PLAYER_AUDIO_CODEC_FORMAT_WAV,            //wav
        PLAYER_AUDIO_CODEC_FORMAT_MP3,            //MP3
        PLAYER_AUDIO_CODEC_FORMAT_BUTT
    }PLAYER_AUDIO_FORMAT_E, AUDIO_FORMAT_E;

    //OutPut image format
    typedef enum hiPLAYER_DRAW_FORMAT_E
    {
        PLAYER_DDRAW_AUTO = 0,    //auto detect draw format of displayer
        PLAYER_DDRAW_YUYV422,     //use yuyv422 image format to draw
        PLAYER_DDRAW_UYVY422,     //use uyvy422 image format to draw
        PLAYER_DDRAW_RGB,         //use RGB image format to draw
        PLAYER_GDI                //use graphics device interface to draw
    }PLAYER_DRAW_FORMAT_E;


    //Video & Audio Record Flag
    typedef enum hiPLAYER_REC_AVFLAG_E
    {
        PLAYER_REC_VIDEO_AUDIO = 0,     //support video and audio
        PLAYER_REC_VIDEO,               //Only video
        PLAYER_REC_AUDIO,               //Only audio
        PLAYER_REC_BUTT
    }PLAYER_REC_AVFLAG_E;

    //File Format
    typedef enum hiPLAYER_FILE_FORMAT_E
    {
        PLAYER_FILE_YUV = 0,            //YUV data after decoding
        PLAYER_FILE_NUDE_STREAM = 1,    //Nude stream before decoding
        PLAYER_FILE_ASF = 2,            //ASF
        PLAYER_FILE_MP4 = 3,            //MP4
        PLAYER_FILE_AVI = 4,            //AVI
		PLAYER_FILE_RAW = 5,
        PLAYER_FILE_BUTT
    }PLAYER_FILE_FORMAT_E;

    //ID of Player state
    typedef enum hiPLAYER_STATE_ID_E
    {
        PLAYER_STATE_PLAY = 0,
        PLAYER_STATE_REC,
        PLAYER_STATE_TALK,
        PLAYER_STATE_BUTT
    }PLAYER_STATE_ID_E;

    typedef enum hiPLAYER_STATE_E
    {
        PLAYER_STATE_READY	= 0,
        PLAYER_STATE_SPLAY	= 1,
        PLAYER_STATE_FPAUSE	= 2,
        PLAYER_STATE_FPLAY	= 3,
        PLAYER_STATE_FSTOP	= 4,
		PLAYER_STATE_SPAUSE = 5,
		PLAYER_STATE_SEND	= 6,
		PLAYER_STATE_FEND	= 7,
		PLAYER_STATE_SSTOP	= 8
    }PLAYER_STATE_E;

    //media attr type
    typedef enum hiPLAYER_MEDIAATTR_TYPE_E
    {
        PLAYER_ATTR_VIDEO_STREAM = 0,     //video stream attr
        PLAYER_ATTR_VIDEO_OUTPUT = 1,     //video out attr
        PLAYER_ATTR_AUDIO_STREAM = 2,     //audio stream attr
        PLAYER_ATTR_AUDIO_ENCODE = 3,     //audio encoding attr
        PLAYER_BUTT_ATTR_TYPE
    }PLAYER_MEDIAATTR_TYPE_E;
    //Audio Direction
    typedef enum hiPLAYER_AUDIO_DIRECT_E
    {
        PLAYER_AUDIO_OUT = 0,  //Audio input direction
        PLAYER_AUDIO_IN = 1    //audio output direction
    }PLAYER_AUDIO_DIRECT_E;

	typedef enum hiPLAYER_DISPLAYMODE_E
	{
		PLAYER_DPY_D3D = 0,
		PLAYER_DPY_DDRAW,
	}PLAYER_DISPLAYMODE_E;

    //the caps of video decoding
    typedef struct hiPLAYER_CAPS_VDEC_S
    {
        HI_BOOL bH261;   //support h261
        HI_BOOL bH263;   //support h263
        HI_BOOL bMPEG2;  //support MPEG2
        HI_BOOL bMPEG4;  //support MPEG4
        HI_BOOL bH264;   //support H264
        HI_BOOL bMJPEG;  //support MJPEG
    }
    PLAYER_CAPS_VDEC_S;

    //the caps of audio
    typedef struct hiPLAYER_CAPS_AUDIO_S
    {
        HI_BOOL bG711A; //support G711A
        HI_BOOL bG711U; //support G711U
        HI_BOOL bADPCM; //support ADPCM
        HI_BOOL bG726;  //support G726
        HI_BOOL bAMR;   //support AMR
        HI_BOOL bAMRDTX;//support AMR
        HI_BOOL bAAC;   //support AAC
        HI_BOOL bWAV;   //support Wave
        HI_BOOL bMP3;   //support MP3
    }
    PLAYER_CAPS_AUDIO_S;

    typedef struct hiPLAYER_ATTR_VIDEO_STREAM_S
    {
        PLAYER_VIDEO_FORMAT_E eVEncode;  //Video encode format
        long lHeight;        //video height
        long lWidth;         //video width
        long lBitRate;       //video bit rate
        long lFrameRate;     //video frame rate
    }
    PLAYER_ATTR_VIDEO_STREAM_S;


    //Image flip
    typedef struct hiPLAYER_VIDEO_FLIP_S
    {
        HI_BOOL bHorizon;     //turn in the direction of horizon(turn left & right)
        HI_BOOL bVertical;    //turn in the direction of vertical (up & down)
    }
    PLAYER_VIDEO_FLIP_S;

    //the attr of video output
    typedef struct hiPLAYER_ATTR_VIDEO_OUTPUT_S
    {
        PLAYER_DRAW_FORMAT_E ePicFormat;  //the picture show format
        PLAYER_VIDEO_FLIP_S struFlip;      //image flip
        HI_BOOL bVerticalBlink;            //if vertical blank
        HI_BOOL bStill;                    //if still
    }
    PLAYER_ATTR_VIDEO_OUTPUT_S;

    //audio attr
    typedef struct hiPLAYER_ATTR_AUDIO_S
    {
        PLAYER_AUDIO_FORMAT_E eAEncode;  //audio encode format
        long lSamplesPerSec;         //audio's samples per second
        long lBitsPerSample;         //bits per sample
        long lBitRate;               //audio's bit rate
        long lBlockAlign;            //if block align
        long lChannels;              //audio's channels
        long lFrameFlag;             //audio's frame flag
        long length;                 //audio's size
        void *pReserved;
    }
    PLAYER_ATTR_AUDIO_S;

    //duration time
    typedef struct hiPLAYER_DURATION_S
    {
        int s32Hours;
        int s32Mins;
        int s32Secs;
    }
    PLAYER_DURATION_S;

    //file attr
    typedef struct hiPLAYER_ATTR_FILE_S
    {
        unsigned char u8FormatName[PLAYER_FORMAT_NAME_LEN]; //file format,such as: .asf
        unsigned char u8FileName[PLAYER_MAX_FILE_NAME];     //file name
        PLAYER_DURATION_S struDuration;                     //file duration
        unsigned int u32StreamNum;                          //the number of stream num（mix stream,single audio,single video stream）
        int s32VideoIndex;                                  //video stream's index
        int s32AudioIndex;                                  //audio stream's index
        HI_BOOL bSeekEnable;                                //the file is whether can be seeked
    }
    PLAYER_ATTR_FILE_S;

    //frame image info
    typedef struct hiPLAYER_FRAME_INFO_S
    {
        HI_U8* pY;
        HI_U8* pU;
        HI_U8* pV;
        long nWidth;
        long nHeight;
        long nType;     //data type:YUV420
        long nYPich;
        long nUVPich;
        HI_U64 u64Pts;
    }
    PLAYER_FRAME_INFO_S;

    typedef HRESULT (HSPLAYER_CALLBACK *HI_PLAYER_StateCallBack)(PLAYHANDLE hHandle,
            PLAYER_STATE_ID_E eStateID,
            HI_U32 u32State,
            HI_VOID *pPara);

    typedef HRESULT (HSPLAYER_CALLBACK *HI_PLAYER_PostDrawCallBack)(PLAYHANDLE hHandle,
            HI_VOID *hDc,
            HI_S32 s32ImageWidth,
            HI_S32 s32ImageHeight,
            HI_S32 s32WndWidth,
            HI_S32 s32WndHeight,
            HI_U64 u64TimeStamp,
            HI_VOID *pPara);

	typedef HRESULT (HSPLAYER_CALLBACK *HI_PLAYER_PostDrawCallBackEx)(PLAYHANDLE hHandle,
			HI_VOID *hDc,
			HI_S32 s32ImageWidth,
			HI_S32 s32ImageHeight,
			HI_S32 s32WndWidth,
			HI_S32 s32WndHeight,
			HI_S32 s32Offx,
			HI_S32 s32Offy,
			HI_U64 u64TimeStamp,
            HI_VOID *pPara);

    typedef HRESULT(HSPLAYER_CALLBACK *HI_PLAYER_DecCallBack )(PLAYHANDLE hHandle,
            const PLAYER_FRAME_INFO_S *pFrameInfo,
            HI_VOID *pPara);

    typedef HRESULT(HSPLAYER_CALLBACK *HI_PLAYER_TalkCallBack)(PLAYHANDLE hHandle,
            HI_U8 *pBuf,
            HI_S32 s32Size,
            HI_U64 u64TimeStamp,
            HI_VOID *pPara);
    //Stream
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Initialize(PLAYHANDLE *pHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Uninitialize(PLAYHANDLE hHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetDrawWnd(PLAYHANDLE hHandle, HI_VOID *hWnd);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Reset(PLAYHANDLE hHandle);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_ResetAudio(PLAYHANDLE hHandle, HI_U32 u32AFormat);
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Play(PLAYHANDLE hHandle);
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Pause(PLAYHANDLE hHandle);
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Stop(PLAYHANDLE hHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_InputVideoData(PLAYHANDLE hHandle,
            HI_U8 *pBuf,
            HI_S32 s32Size,
            HI_U64 u64TimeStamp);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_InputVideoDataEx(PLAYHANDLE hHandle,
																	HI_U8 *pBuf,
																	HI_S32 s32Size,
																	HI_S32 s32KeyFrame,
																	HI_U64 u64TimeStamp);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetDecCallBack(PLAYHANDLE hHandle,
            HI_PLAYER_DecCallBack CallBack,
            HI_VOID *pPara);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetPostDrawCallBack(PLAYHANDLE hHandle,
            HI_PLAYER_PostDrawCallBack CallBack,
            HI_VOID *pPara);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetPostDrawCallBackEx(PLAYHANDLE hHandle,
			HI_PLAYER_PostDrawCallBackEx CallBack,
            HI_VOID *pPara);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_DisplayAll(PLAYHANDLE hHandle,
			HI_S32 s32Left = 0, 
			HI_S32 s32Top = 0, 
			HI_S32 s32Right = 0, 
			HI_S32 s32Bottom = 0, 
			HI_BOOL bDisplayAll = HI_TRUE);

    /*Talk*/
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_StartTalk(PLAYHANDLE hHandle,
            HI_PLAYER_TalkCallBack CallBack,
            HI_VOID *pPara);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_StopTalk(PLAYHANDLE hHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_InputAudioData(PLAYHANDLE hHandle,
            HI_U8 *pBuf,
            HI_S32 s32Size,
            HI_U64 u64TimeStamp);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetMute(PLAYHANDLE hHandle, PLAYER_AUDIO_DIRECT_E eAudioDirect, HI_BOOL bMute);
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetMute(PLAYHANDLE hHandle, PLAYER_AUDIO_DIRECT_E eAudioDirect, HI_BOOL *pMute);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetVolume(PLAYHANDLE hHandle, PLAYER_AUDIO_DIRECT_E eAudioDirect,
            HI_S32 s32LVolume, HI_S32 s32RVolume);
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetVolume(PLAYHANDLE hHandle, PLAYER_AUDIO_DIRECT_E eAudioDirect,
            HI_S32 *pLVolume, HI_S32 *pRVolume);

    /*File*/
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_OpenFile(PLAYHANDLE hHandle,
            HI_U8 *pFileName);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_CloseFile(PLAYHANDLE hHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetPlayeDirection(PLAYHANDLE hHandle,
            HI_BOOL bDirection);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetPlayPos(PLAYHANDLE hHandle, HI_S32 s32Pos);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetPlayPos(PLAYHANDLE hHandle, HI_S32 *pPos);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetPlayTime(PLAYHANDLE hHandle, HI_S32 *pTime);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Rate(PLAYHANDLE hHandle, HI_S32 rate);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetRate(PLAYHANDLE hHandle, HI_S32 rate);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetRate(PLAYHANDLE hHandle, HI_S32 *rate);
	
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Fast(PLAYHANDLE hHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_Slow(PLAYHANDLE hHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_OneByOne(PLAYHANDLE hHandle);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetFileAttr( PLAYHANDLE hHandle,
            PLAYER_ATTR_FILE_S *pFileAttr);

    /*Snap*/
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SnapBMP(PLAYHANDLE hHandle,
            HI_CHAR *pSnapPath);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SnapJPEG(PLAYHANDLE hHandle,
            HI_CHAR *pSnapPath,
            HI_S32 s32QValue);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SnapData(PLAYHANDLE hHandle, 
			HI_U8 *pData, 
			HI_U32 *pu32Size, 
			HI_U32 *pu32Width, 
			HI_U32 *pu32Height, 
			HI_S32 s32Type, 
			HI_S32 s32QValue);

    /*Attr、State and Caps*/
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetState(PLAYHANDLE hHandle,
            PLAYER_STATE_ID_E eStateID,
            HI_U32 *pState);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetStateCallBack(PLAYHANDLE hHandle,
            HI_PLAYER_StateCallBack CallBack,
            HI_VOID *pPara);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetMediaAttr(PLAYHANDLE hHandle,
            PLAYER_MEDIAATTR_TYPE_E eAttrType,
            HI_VOID *pAttr);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetMediaAttr(PLAYHANDLE hHandle,
            PLAYER_MEDIAATTR_TYPE_E eAttrType,
            HI_VOID *pAttr);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetMediaCaps(PLAYHANDLE hHandle,
            PLAYER_CAPS_VDEC_S *pVDecCaps,
            PLAYER_CAPS_AUDIO_S *pADecCaps,
            PLAYER_CAPS_AUDIO_S *pAEncCaps);

    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetVersion (HI_S8 **pVersion);
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_EnhanceImage(PLAYHANDLE hHandle,
            HI_S32 s32LumFlag,
            HI_S32 s32LumThred);
    HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetEnhanceState(PLAYHANDLE hHandle,
            HI_S32 *pLumFlag,
            HI_S32 *pLumThred);
	// 调整流播放缓冲 fengjf 090908 
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetNetType(PLAYHANDLE hHandle,
		HI_BOOL bLocalNet);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_PauseDecode(PLAYHANDLE hHandle);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_ResumeDecode(PLAYHANDLE hHandle);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetShowImage(PLAYHANDLE hHandle);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetPlayerBufNumber(PLAYHANDLE hHandle, HI_U32 u32BufNumber);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetAORBParam(PLAYHANDLE hHandle, HI_S32 s32MaxFrameRate, HI_S32 s32CurFrameRate);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetAutoAdjust(PLAYHANDLE hHandle);
	HSPLAYER_API HI_BOOL HSPLAYER_APICALL HI_PLAYER_GetAutoAdjust(PLAYHANDLE hHandle);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetStreamOpenMode(PLAYHANDLE hHandle, HI_U32 u32Mode = HI_STREAM_REALTIME);
	HSPLAYER_API HI_U32 HSPLAYER_APICALL HI_PLAYER_GetStreamOpenMode(PLAYHANDLE hHandle);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_OpenStream(PLAYHANDLE hHandle, HI_U8 *pFileHeadBuf, HI_U32 u32Size);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_InputData(PLAYHANDLE hHandle, HI_U8 *pBuf, HI_S32 s32Size);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_ResetSourceBuffer(PLAYHANDLE hHandle);
	HSPLAYER_API HI_S32 HSPLAYER_APICALL HI_PLAYER_GetBufferValue(PLAYHANDLE hHandle);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetCurrentPts(PLAYHANDLE hHandle, HI_U64 *pCurPts);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetStreamEnd(PLAYHANDLE hHandle);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_StopDisplay(PLAYHANDLE hHandle);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetDisplay(PLAYHANDLE hHandle, HI_BOOL bDisplay);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetImageAttr(PLAYHANDLE hHandle, HI_S32 s32Width, HI_S32 s32Height);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetStreamEnd(PLAYHANDLE hHandle);

	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetDisplayMode(PLAYHANDLE hHandle, PLAYER_DISPLAYMODE_E eDisplayMode);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_GetDisplayMode(PLAYHANDLE hHandle, PLAYER_DISPLAYMODE_E *pDisplayMode);
	HSPLAYER_API HRESULT HSPLAYER_APICALL HI_PLAYER_SetDrawCallBack(PLAYHANDLE hHandle, HI_BOOL bDrawCallback);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif   //_HI_HSPLAYER_H_
