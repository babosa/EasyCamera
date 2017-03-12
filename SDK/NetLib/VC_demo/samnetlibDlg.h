// samnetlibDlg.h : header file
//

#if !defined(AFX_SAMNETLIBDLG_H__31199BAD_C965_4AAC_B570_6DC66547358B__INCLUDED_)
#define AFX_SAMNETLIBDLG_H__31199BAD_C965_4AAC_B570_6DC66547358B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "CHI_AI_MM.h"
#include "CHI_AO_MM.h"
#include "CHI_ADEC_DEC.h"
#include "CHI_COMMON_RingBuffer.h"
/////////////////////////////////////////////////////////////////////////////
// CSamnetlibDlg dialog
#define HI_RESOLUTION_VGA		0
#define HI_RESOLUTION_QVGA		1
#define HI_RESOLUTION_QQVGA		2
#define HI_RESOLUTION_D1		3
#define HI_RESOLUTION_CIF		4
#define HI_RESOLUTION_QCIF		5
#define HI_RESOLUTION_720P		6
#define HI_RESOLUTION_Q720		7
#define HI_RESOLUTION_QQ720		8
#define HI_RESOLUTION_UXGA		9
#define HI_RESOLUTION_960H		10
#define HI_RESOLUTION_Q960H		11
#define HI_RESOLUTION_QQ960H	12
#define HI_RESOLUTION_1080P		13
#define HI_RESOLUTION_960P		14

class CSamnetlibDlg : public CDialog
{
// Construction
public:
	CSamnetlibDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSamnetlibDlg)
	enum { IDD = IDD_SAMNETLIB_DIALOG };
	CComboBox	m_combRec;
	CComboBox	m_combType;
	CString	m_strHost;
	UINT	m_uiPort;
	CString	m_strPword;
	CString	m_strUname;
	CComboBox	m_combPtzType;
	CComboBox	m_combSpeed;
	CComboBox	m_combLcbr;
	CComboBox	m_combAuSwitch;
	CComboBox	m_combParity;
	CComboBox	m_combProtocol;
	CComboBox	m_combStopBit;
	CComboBox	m_combDataBit;
	CComboBox	m_combBaud;
	CComboBox	m_combInput;
	CComboBox	m_combFormat;
	CComboBox	m_combFrameRate;
	CComboBox	m_combQuality;
	CComboBox	m_combResolution;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSamnetlibDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSamnetlibDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnCnt();
	afx_msg void OnBtnStart();
	virtual void OnCancel();
	afx_msg void OnBtnRecord();
	afx_msg void OnBtnStartTalk();
	afx_msg void OnBtnProduct();
	afx_msg void OnBtnGetVideo();
	afx_msg void OnBtnSetVideo();
	afx_msg void OnBtnGetAudio();
	afx_msg void OnBtnSetAudio();
	afx_msg void OnBtnGetMic();
	afx_msg void OnBtnSetMic();
	afx_msg void OnBtnGetPtz();
	afx_msg void OnBtnSetPtz();
	afx_msg void OnBtnUp();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnLeft();
	afx_msg void OnBtnRight();
	afx_msg void OnBtnPtzstop();
	afx_msg void OnBtnPtzZoomIn();
	afx_msg void OnBtnPtzZoomOut();
	afx_msg void OnBtnPreset();
	afx_msg void OnBtnPreCall();
	afx_msg void OnBtnPtzTrans();
	afx_msg void OnBtnGetRes();
	afx_msg void OnBtnSetRes();
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnIrisbig();
	afx_msg void OnBtnIrissmall();
	afx_msg void OnBtnFocusout();
	afx_msg void OnBtnFocusin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	UINT m_uiHandle;

public:
	PLAYHANDLE m_hPlay;
	bool m_bTalk;
	bool m_bRecord;
	bool m_bStreamFlag;
	bool m_bFirstFrame;
	FILE *m_phFile;
	CHI_AI_MM* m_pAI_MM;
	CHI_AO_MM *m_pAO_MM;
	HI_S_SysHeader *m_pFileHeader;
	HI_S32 NewAI();
	void SaveFile(HI_U8* pu8Buffer, HI_U32 u32Length, HI_S32 u32DataType);
	void OnSendData(HI_U8 *pBuf, HI_S32 s32Size, HI_U64 u64TimeStamp);
	void StartAudio(HI_U32 u32Type);
	void StreamStop();
	void SetCamAttr(HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Format);

	HI_U32 m_u32Width, m_u32Height;
	HI_U32 m_u32Format;
	HI_BOOL m_bStream;

	CHI_COMMON_RingBuffer m_RingBuffer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMNETLIBDLG_H__31199BAD_C965_4AAC_B570_6DC66547358B__INCLUDED_)
