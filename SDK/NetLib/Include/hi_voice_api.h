#ifndef __HI_VOICE_ENGINE_H__
#define __HI_VOICE_ENGINE_H__
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#ifndef HI_S8
  #define HI_S8  char
#endif

#ifndef HI_S16
  #define HI_S16 short
#endif

#ifndef HI_S32
  #define HI_S32 int
#endif

#ifndef HI_U8
  #define HI_U8  unsigned char
#endif

#ifndef HI_U16 
  #define HI_U16 unsigned short
#endif

#ifndef HI_U32
  #define HI_U32 unsigned int
#endif

#ifndef HI_VOID
  #define HI_VOID void
#endif

#ifndef HI_RESULT
   #define  HI_RESULT int
#endif

#ifndef NULL
   #define NULL   (HI_S32)0x0L
#endif

#ifndef HI_VOICE_API
#ifdef WIN32
	#define HI_VOICE_API     __declspec(dllexport)
#else
	#define HI_VOICE_API
#endif	
#endif

/* State for G711 encoder and decoder */
typedef struct hiVOICE_G711_STATE{
    HI_S32 G711StateBuf[2];
} hiVOICE_G711_STATE_S;

/* State for ADPCM encoder and decoder */
typedef struct hiVOICE_ADPCM_STATE{
    HI_S32 AdpcmStateBuf[4];
} hiVOICE_ADPCM_STATE_S;

/* State for G726 encoder and decoder */
typedef struct hiVOICE_G726_STATE
{
    HI_S32 G726StateBuf[32];
} hiVOICE_G726_STATE_S;

/* State for TransCode */
typedef struct hiVOICE_TRANSCODE_STATE
{
    HI_S32 TransStateBuf[0x400];
} hiVOICE_TRANSCODE_STATE_S;


/* max decode/encode frame size, 60ms at 8k samperate or 10ms at 48k samplerate */
#define HI_VOICE_MAX_FRAME_SIZE       (480+1)    /* dont change it */

/* HISI_VOICE codec type */

/* Real-time transport protocol(RTP) */
#define G711_A                        0x01  /* 64kbps G.711 A, see RFC3551.txt  4.5.14 PCMA */
#define G711_U                        0x02  /* 64kbps G.711 U, see RFC3551.txt  4.5.14 PCMU */
#define ADPCM_DVI4                    0x03  /* 32kbps ADPCM(DVI4) for RTP; see DVI4 diiffers in three 
                                               respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */
#define G726_16KBPS                   0x04  /* 16kbps G.726, see RFC3551.txt  4.5.4 G726-16 */
#define G726_24KBPS                   0x05  /* 24kbps G.726, see RFC3551.txt  4.5.4 G726-24 */
#define G726_32KBPS                   0x06  /* 32kbps G.726, see RFC3551.txt  4.5.4 G726-32 */
#define G726_40KBPS                   0x07  /* 40kbps G.726, see RFC3551.txt  4.5.4 G726-40 */

/* IMA ADPCM wave type  */	
#define ADPCM_IMA                     0x23  /* 32kbps ADPCM(IMA) */

/*
An example of the packing scheme for G726-32 codewords is as shown, and bit A3 is the least significant bit of the first codeword: 
RTP G726-32:
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|B B B B|A A A A|D D D D|C C C C| ...
|0 1 2 3|0 1 2 3|0 1 2 3|0 1 2 3|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

MEDIA G726-32:
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|A A A A|B B B B|C C C C|D D D D| ...
|3 2 1 0|3 2 1 0|3 2 1 0|3 2 1 0|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/
#define MEDIA_G726_16KBPS             0x24  /* G726 16kbps for ASF ... */
#define MEDIA_G726_24KBPS             0x25  /* G726 24kbps for ASF ... */
#define MEDIA_G726_32KBPS             0x26  /* G726 32kbps for ASF ... */
#define MEDIA_G726_40KBPS             0x27  /* G726 40kbps for ASF ... */


/* ORIGINE VERSION CODEC */
#define G711_ORG_A                    0x41  /* original version 64kbps G.711 A */
#define G711_ORG_U                    0x42  /* original version 64kbps G.711 U */
#define ADPCM_ORG_DVI4                0x43  /* original version 32kbps ADPCM */
#define G726_ORG_16KBPS               0x44  /* original version(DVS1.0) 16kbps G.726 */


/* TRANSFORM CODE */
#define ORG_G711A_TRANSF2_G711A               0x61  /* transform original version 64kbps G.711 A to G711_A*/
#define ORG_G711U_TRANSF2_G711U               0x62  /* transform original version 64kbps G.711 U to G711_U*/
#define ORG_ADPCM_DVI4_TRANSF2_ADPCM_DVI4     0x63  /* transform original version 32kbps ADPCM to ADPCM_DVI4*/
#define ORG_G726_16KBPS_TRANSF2_G726_16KBPS   0x64  /* transform original version(DVS1.0) 16kbps G.726 to G726_16KBPS*/


/* HISI_VOICE FRAME  type */
#define HI_NORMAL_FRAME               0x01


/* HISI_VOICE right code */
#define HI_SUCCESS                    0

#define HI_ERR_VOICE_PREFIX           0xA1220000

/* HISI_VOICE error code */
#define HI_ERR_VOICE_ENC_TYPE         (HI_ERR_VOICE_PREFIX | 0x0001)
#define HI_ERR_VOICE_ENC_FRAMESIZE    (HI_ERR_VOICE_PREFIX | 0x0002)

#define HI_ERR_VOICE_DEC_TYPE         (HI_ERR_VOICE_PREFIX | 0x0011)
#define HI_ERR_VOICE_DEC_FRAMESIZE    (HI_ERR_VOICE_PREFIX | 0x0012)
#define HI_ERR_VOICE_DEC_FRAMETYPE    (HI_ERR_VOICE_PREFIX | 0x0013)


#define HI_ERR_VOICE_INVALID_DEVICE   (HI_ERR_VOICE_PREFIX | 0x0101)
#define HI_ERR_VOICE_INVALID_INBUF    (HI_ERR_VOICE_PREFIX | 0x0102)
#define HI_ERR_VOICE_INVALID_OUTBUF   (HI_ERR_VOICE_PREFIX | 0x0103)

#define HI_ERR_VOICE_TRANS_DEVICE     (HI_ERR_VOICE_PREFIX | 0x1001)
#define HI_ERR_VOICE_TRANS_TYPE       (HI_ERR_VOICE_PREFIX | 0x1002)

/********************************************************************************
|                                                                               |
|	   HISI VOCIE FRAME Description                                             | 
|	                                                                            |
|	   Paramater 	    bit        Description                                  |
|	   HI_S16 0         15:8       Frame Flag                                   |
|	                               01B  :  normal voice frame                   |
|	                               other:  invalid                              |
|	   HI_S16 0         07:0       reseverd                                     |
|	                                                                            |
|	   HI_S16 1         15:8       Frame Counter                                |
|	                               00 ~ 255                                     |
|	   HI_S16 1         07:0       Raw Frame Data Length(HI_S16)                |
|	                                                                            |
|	   HI_S16 2         15:0       Raw data 0                                   |
|      HI_S16 3         15:0       Raw data 1                                   |
|	     .                                                                      |
|		 .                                                                      |
|		 .                                                                      |
|       HI_S16 2+n       15:0       Raw data n   n==Raw Frame Data Length       |
|                                                                               |
|                                                                               |
********************************************************************************/

/***************************************************************************
* Function:      HI_VOICE_EncReset
* Description:   encoder reset  
* Input:         * pEncState:     encoder device handle
*                  nCoder:         encoder typeption 
* Output:        none
* Return:        HI_SUCCESS                          success
*                HI_ERR_VOICE_INVALID_DEVICE         invalid encoder device handle
*                HI_ERR_VOICE_ENC_TYPE               invalid encoder typeption 
******************************************************************************/
HI_VOICE_API HI_RESULT  HI_VOICE_EncReset(HI_VOID *pEncState, HI_S16 nCoder);

/***************************************************************************
* Function:      HI_VOICE_DecReset
* Description:   decoder reset  
* Input:         * pDecState:     decoder device handle
*                  nCoder:         decoder typeption 
* Output:        none
* Return:        HI_SUCCESS                          success
*                HI_ERR_VOICE_INVALID_DEVICE         invalid decoder device handle
*                HI_ERR_VOICE_DEC_TYPE               invalid decoder typeption 
******************************************************************************/
HI_VOICE_API HI_RESULT  HI_VOICE_DecReset(HI_VOID *pDecState, HI_S16 nCoder);

/***************************************************************************
* Function:      HI_VOICE_EncodeFrame
* Description:   encode one frame speech data  
* Input:         * pEncState:     encoder device handle
*                * pInputBuf:     input speech data addr 
*                  len:           input speech data count(unit: HI_S16) 
                                  len = n*80 (n*80>0 && n*80<MAX_FRAME_SIZE ),   nCoder!=ADPCM_A
                                  len = n*80+1 (n*80>0 && n*80<MAX_FRAME_SIZE ), nCoder==ADPCM_A
*                  nCoder:         encoder typeption 
* Output:        * pOutputBuf     output addr of compressed speech data buffer
* Return:        HI_SUCCESS                          success
*                HI_ERR_VOICE_INVALID_DEVICE         invalid encoder device handle
*                HI_ERR_VOICE_INVALID_INBUF          invalid input speech data addr
*                HI_ERR_VOICE_INVALID_OUTBUF         invalid output addr
*                HI_ERR_VOICE_ENC_FRAMESIZE          invalid encoder FrameSize
*                HI_ERR_VOICE_ENC_TYPE               invalid encoder typeption 
******************************************************************************/
HI_VOICE_API HI_RESULT  HI_VOICE_EncodeFrame(HI_VOID *pEncState, HI_S16 *pInputBuf, HI_S16 *pOutputBuf, HI_S16 len);

/***************************************************************************
* Function:      HI_VOICE_DecodeFrame
* Description:   decode one frame compressed speech data  
* Input:         * pDecState:     decoder device handle
*                * pInputBuf:     input addr of one frame compress speech data buffer 
*                  nCoder:        encoder typeption 
* Output:        * pOutputBuf£º   output addr of decompressed speech data buffer
                 * pLen:          output speech data count(unit: HI_S16) 
                                  *pPen = n*80   (n*80>0 && n*80<MAX_FRAME_SIZE ), nCoder!=ADPCM_A 
                                  *pPen = n*80+1 (n*80>0 && n*80<MAX_FRAME_SIZE ), nCoder==ADPCM_A
* Return:        HI_SUCCESS                          success
*                HI_ERR_VOICE_INVALID_DEVICE         invalid decoder device handle
*                HI_ERR_VOICE_INVALID_INBUF          invalid input addr of one frame compress 
                                                     speech data buffer
*                HI_ERR_VOICE_INVALID_OUTBUF         invalid utput addr of decompressed speech data buffer
*                HI_ERR_VOICE_DEC_FRAMESIZE          invalid decoder FrameSize
*                HI_ERR_VOICE_DEC_FRAMETYPE          invalid frame of compress speech data 
******************************************************************************/
HI_VOICE_API HI_RESULT  HI_VOICE_DecodeFrame(HI_VOID *pDecState, HI_S16 *pInputBuf,  HI_S16 *pOutputBuf, HI_S16 *pLen);

/***************************************************************************
* Function:      HI_VOICE_EncReset
* Description:   encoder reset  
* Input:         * pTransState:   transform device handle
*                  nCoder:        transform typeption
* Output:        none
* Return:        HI_SUCCESS                          success
*                HI_ERR_VOICE_TRANS_DEVICE           invalid transform device handle
*                HI_ERR_VOICE_TRANS_TYPE             invalid transform typeption
******************************************************************************/
HI_VOICE_API HI_RESULT  HI_VOICE_TransCodeReset(HI_VOID *pTransState, HI_S16 nCoder);


/***************************************************************************
* Function:      HI_VOICE_TransCode
* Description:   transform the old format date to new format data
* Input:         * pTransState:   transform device handle
*                * pInputBuf:     input addr of origion frame compress speech data buffer 
* Output:        * pInputBuf:     output addr of new frame compress speech data buffer 
* Return:        HI_SUCCESS                          success
*                HI_ERR_VOICE_TRANS_DEVICE           invalid transform device handle
*                HI_ERR_VOICE_INVALID_INBUF          invalid input addr of one frame compress 
                                                     speech data buffer
*                HI_ERR_VOICE_TRANS_TYPE             invalid transform typeption
******************************************************************************/
HI_VOICE_API HI_RESULT  HI_VOICE_TransCodeFrame(HI_VOID *pTransState,HI_S16 *pInputBuf);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif 
