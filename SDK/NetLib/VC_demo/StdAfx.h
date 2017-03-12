// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7B02951F_B2A2_4D1C_B148_5769E21360E1__INCLUDED_)
#define AFX_STDAFX_H__7B02951F_B2A2_4D1C_B148_5769E21360E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define SAFE_DELETEA(p) { if((p) != NULL) { delete[] (p);   (p) = NULL; } }   //Delete Arrary
#define HI_ERR_NULL_POINTER	 0xA1888007

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
#define PLAY_AUDIO_FRAME_SIZE           7680//96K 双声道20ms的数据长度
#define PLAY_AUDIO_PACKAGE_NUM          30  //glping //60 //15 //l53171 
#define PLAY_AUDIO_PACKAGE_NUM_NORMAL   4   // add by glping 


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"Winmm")


#include "hi_type.h"
#include "hi_net_dev_sdk.h"
#include "hi_net_dev_errors.h"
#include "hi_voice_api.h"
#include "HsPlayer.h"

#pragma comment(lib, "NetLib.lib")
#pragma comment(lib, "HIPlayer.lib")
#pragma comment(lib, "hisi_voice_engine.lib")

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7B02951F_B2A2_4D1C_B148_5769E21360E1__INCLUDED_)
