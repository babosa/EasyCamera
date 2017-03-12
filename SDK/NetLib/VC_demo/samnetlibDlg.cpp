// samnetlibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "samnetlib.h"
#include "samnetlibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_audioType;
extern void g_SetWndStaticText(CWnd * pWnd);
extern CString ConvertString(CString strText);

HI_S32 NETSDK_APICALL OnEventCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32Event,      /* 事件 */
                                HI_VOID* pUserData  /* 用户数据*/
                                )
{
	CSamnetlibDlg *pSamnetlibDlg = (CSamnetlibDlg*)pUserData;
	//if(HI_NET_DEV_NORMAL_DISCONNECTED == u32Event)
	//	pSamnetlibDlg->AlartData();
	return HI_SUCCESS;
}


void SaveRecordFile(HI_CHAR* pPath, HI_U8* pu8Buffer, HI_U32 u32Length)
{
	FILE* fp;

	fp = fopen(pPath, "ab+");
	fwrite(pu8Buffer, 1, u32Length, fp);
	fclose(fp);
}

HI_U32 u32LastPts = 0;
HI_S32 NETSDK_APICALL OnStreamCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32DataType,     /* 数据类型，视频或音频数据或音视频复合数据 */
                                HI_U8*  pu8Buffer,      /* 数据包含帧头 */
                                HI_U32 u32Length,      /* 数据长度 */
                                HI_VOID* pUserData    /* 用户数据*/
                                )
{
	HI_S_AVFrame* pstruAV = HI_NULL;
	HI_S_SysHeader* pstruSys = HI_NULL;
	HI_S32 u32Len = 0;
	HI_U8  u8Buffer[1024] = {0};
	HI_S32 s32KeyFrame = 0;

	//SYSTEMTIME   dt; 
	//GetLocalTime(&dt);
	//printf("%d\n", dt.wSecond*1000+dt.wMilliseconds);
	
	//return 0;

	CSamnetlibDlg *pSamnetlibDlg = (CSamnetlibDlg*)pUserData;

	if (u32DataType == HI_NET_DEV_AV_DATA)
	{
		pstruAV = (HI_S_AVFrame*)pu8Buffer;

		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
		{
			//printf("%d\n", pstruAV->u32AVFramePTS - u32LastPts);
			//u32LastPts = pstruAV->u32AVFramePTS;
			//printf("%d\n", pstruAV->u32VFrameType);
			//SaveRecordFile("D://video1.h264", pu8Buffer+sizeof(HI_S_AVFrame), u32Length-sizeof(HI_S_AVFrame));
			if(NULL != pSamnetlibDlg->m_hPlay)
			{
				if(pstruAV->u32VFrameType == 1)
					s32KeyFrame = 1;
				else
					s32KeyFrame = 0;
				
				HI_PLAYER_InputVideoDataEx( pSamnetlibDlg->m_hPlay, 
					pu8Buffer+sizeof(HI_S_AVFrame),  
					pstruAV->u32AVFrameLen, 
					s32KeyFrame, 
					pstruAV->u32AVFramePTS);
				//printf("%u----\n", pstruAV->u32AVFramePTS);
			}	
		}
		else
		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_AUDIO_FRAME_FLAG)
		{
			if(NULL != pSamnetlibDlg->m_hPlay)
			{
				HI_PLAYER_InputAudioData(pSamnetlibDlg->m_hPlay, 
					pu8Buffer+sizeof(HI_S_AVFrame), 
					pstruAV->u32AVFrameLen, 
					pstruAV->u32AVFramePTS);

				//printf("%u\n", pstruAV->u32AVFramePTS);
			}

			if(pSamnetlibDlg->m_pAO_MM != NULL)
			{
				//pSamnetlibDlg->m_pAO_MM->HI_ADEC_InPutAudioData(pu8Buffer+sizeof(HI_S_AVFrame), 
				//														pstruAV->u32AVFrameLen, 
				//														pstruAV->u32AVFramePTS);
			}
		}
		/*if(NULL != pSamnetlibDlg->m_hPlay)
		{
			HI_PLAYER_InputData(pSamnetlibDlg->m_hPlay, pu8Buffer, u32Length);
			printf("%d, %d\n", 1280*768, u32Length);
		}*/
	}
	else
	if (u32DataType == HI_NET_DEV_SYS_DATA)
	{
		pstruSys = (HI_S_SysHeader*)pu8Buffer;
		pSamnetlibDlg->SetCamAttr(pstruSys->struVHeader.u32Width, 
								  pstruSys->struVHeader.u32Height, 
								  pstruSys->struAHeader.u32Format);
		pSamnetlibDlg->StartAudio(pstruSys->struAHeader.u32Format);
	} 

	//pSamnetlibDlg->SaveFile(pu8Buffer, u32Length, u32DataType);
	//SaveRecordFile("D://video1.hx", pu8Buffer, u32Length);

	return HI_SUCCESS;
}

HI_S32 NETSDK_APICALL OnDataCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32DataType,       /* 数据类型*/
                                HI_U8*  pu8Buffer,      /* 数据 */
                                HI_U32 u32Length,      /* 数据长度 */
                                HI_VOID* pUserData    /* 用户数据*/
                                )
{
	//CSamnetlibDlg *pSamnetlibDlg = (CSamnetlibDlg*)pUserData;
	
	//pSamnetlibDlg->AlartData();
	return HI_SUCCESS;
}


HI_S32 WINAPI OnAudioCallbackEx(HI_U8 *pBuf,
					   HI_S32 s32Size,
					   HI_U64 u64TimeStamp,
					   HI_VOID *pUserData)
{
	CSamnetlibDlg *pSamnetlibDlg = (CSamnetlibDlg*)pUserData;

	pSamnetlibDlg->OnSendData(pBuf, s32Size, u64TimeStamp);
	return HI_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
// CSamnetlibDlg dialog

CSamnetlibDlg::CSamnetlibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSamnetlibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSamnetlibDlg)
	m_strHost = _T("192.168.1.88");
	m_uiPort = 80;
	m_strPword = _T("admin");
	m_strUname = _T("admin");
	m_uiHandle = 0;
	m_hPlay = NULL;
	m_u32Width = 0;
	m_u32Height = 0;
	m_u32Format = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSamnetlibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSamnetlibDlg)
	DDX_Control(pDX, IDC_COMBO_REC, m_combRec);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combType);
	DDX_Text(pDX, IDC_E_HOST, m_strHost);
	DDX_Text(pDX, IDC_E_PORT, m_uiPort);
	DDV_MinMaxUInt(pDX, m_uiPort, 1, 65535);
	DDX_Text(pDX, IDC_E_PWORD, m_strPword);
	DDX_Text(pDX, IDC_E_UNAME, m_strUname);
	DDX_Control(pDX, IDC_COMBO_PTZTYPE, m_combPtzType);
	DDX_Control(pDX, IDC_COMBO_SPEED, m_combSpeed);
	DDX_Control(pDX, IDC_COMBO_LCBR, m_combLcbr);
	DDX_Control(pDX, IDC_COMBO_AUSWITCH, m_combAuSwitch);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_combParity);
	DDX_Control(pDX, IDC_COMBO_PROTOCOL, m_combProtocol);
	DDX_Control(pDX, IDC_COMBO_STOPBIT, m_combStopBit);
	DDX_Control(pDX, IDC_COMBO_DATABIT, m_combDataBit);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_combBaud);
	DDX_Control(pDX, IDC_COMBO_AUINPUT, m_combInput);
	DDX_Control(pDX, IDC_COMBO_AUFORMAT, m_combFormat);
	DDX_Control(pDX, IDC_COMBO_FRAMERATE, m_combFrameRate);
	DDX_Control(pDX, IDC_COMBO_QUALITY, m_combQuality);
	DDX_Control(pDX, IDC_COMBO_RESOLUTION, m_combResolution);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSamnetlibDlg, CDialog)
	//{{AFX_MSG_MAP(CSamnetlibDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CNT, OnBtnCnt)
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_RECORD, OnBtnRecord)
	ON_BN_CLICKED(IDC_BTN_STARTTALK, OnBtnStartTalk)
	ON_BN_CLICKED(IDC_BTN_PRODUCT, OnBtnProduct)
	ON_BN_CLICKED(IDC_BTN_GETVIDEO, OnBtnGetVideo)
	ON_BN_CLICKED(IDC_BTN_SETVIDEO, OnBtnSetVideo)
	ON_BN_CLICKED(IDC_BTN_GETAUDIO, OnBtnGetAudio)
	ON_BN_CLICKED(IDC_BTN_SETAUDIO, OnBtnSetAudio)
	ON_BN_CLICKED(IDC_BTN_GETMIC, OnBtnGetMic)
	ON_BN_CLICKED(IDC_BTN_SETMIC, OnBtnSetMic)
	ON_BN_CLICKED(IDC_BTN_GETPTZ, OnBtnGetPtz)
	ON_BN_CLICKED(IDC_BTN_SETPTZ, OnBtnSetPtz)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_LEFT, OnBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, OnBtnRight)
	ON_BN_CLICKED(IDC_BTN_PTZSTOP, OnBtnPtzstop)
	ON_BN_CLICKED(IDC_BTN_PTZZOOMIN, OnBtnPtzZoomIn)
	ON_BN_CLICKED(IDC_BTN_PTZZOOMOUT, OnBtnPtzZoomOut)
	ON_BN_CLICKED(IDC_BTN_PRESET, OnBtnPreset)
	ON_BN_CLICKED(IDC_BTN_PRECALL, OnBtnPreCall)
	ON_BN_CLICKED(IDC_BTN_PTZTRANS, OnBtnPtzTrans)
	ON_BN_CLICKED(IDC_BTN_GETRES, OnBtnGetRes)
	ON_BN_CLICKED(IDC_BTN_SETRES, OnBtnSetRes)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(IDC_BTN_IRISBIG, OnBtnIrisbig)
	ON_BN_CLICKED(IDC_BTN_IRISSMALL, OnBtnIrissmall)
	ON_BN_CLICKED(IDC_BTN_FOCUSOUT, OnBtnFocusout)
	ON_BN_CLICKED(IDC_BTN_FOCUSIN, OnBtnFocusin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSamnetlibDlg message handlers

BOOL CSamnetlibDlg::OnInitDialog()
{
	g_SetWndStaticText(this);
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	int i = 0;
	m_uiHandle = 0;
	m_bStreamFlag = FALSE;
	m_phFile = NULL;
	m_bRecord = FALSE;
	m_bTalk = FALSE;
	m_bFirstFrame = TRUE;
	m_pAI_MM = NULL;
	m_pAO_MM = NULL;
	
	//m_pFileHeader = new HI_S_SysHeader;
	//memset(&m_pFileHeader, 0, sizeof(HI_S_SysHeader));

	m_combType.InsertString(0, ConvertString("First Stream"));
	m_combType.InsertString(1, ConvertString("Second Stream"));
	m_combType.InsertString(2, ConvertString("Third Stream"));
	m_combType.SetCurSel(0);

	m_combPtzType.InsertString(0, ConvertString("Continuous"));
	m_combPtzType.InsertString(1, ConvertString("Step"));
	m_combPtzType.SetCurSel(0);

	// 初始化编码参数
	// 码流
	m_combResolution.InsertString(0, "VGA");
	m_combResolution.InsertString(1, "QVGA");
	m_combResolution.InsertString(2, "QQVGA");
	m_combResolution.InsertString(3, "D1");
	m_combResolution.InsertString(4, "CIF");
	m_combResolution.InsertString(5, "QCIF");
	m_combResolution.InsertString(6, "720P");
	m_combResolution.InsertString(7, "Q720");
	m_combResolution.InsertString(8, "QQ720");
	m_combResolution.InsertString(9, "UXGA");
	m_combResolution.InsertString(10, "960H");
	m_combResolution.InsertString(11, "Q960H");
	m_combResolution.InsertString(12, "QQ960H");

	m_combResolution.SetCurSel(0);

	//m_combResolution.setIt

	CString strBuf;
	//m_nMaxFrame = 30;
	for(i=0; i<30; i++)
	{
		strBuf.Format("%d", i+1);
		if(i < 6)
		{
			m_combQuality.InsertString(i, strBuf);
		}

		m_combFrameRate.InsertString(i, strBuf);
	}

	for(i=0; i<64; i++)
	{
		strBuf.Format("%d", i);
		m_combSpeed.InsertString(i, strBuf);
	}
	m_combSpeed.SetCurSel(40);

	m_combFrameRate.SetCurSel(0);
	m_combQuality.SetCurSel(0);

	m_combLcbr.InsertString(0, ConvertString("CBR"));//"可变码率"
	m_combLcbr.InsertString(1, ConvertString("VBR"));
	m_combLcbr.SetCurSel(0);

	m_combFormat.InsertString(0, "G711");
	m_combFormat.InsertString(1, "G726");
	m_combFormat.InsertString(2, "AMR");
	m_combFormat.SetCurSel(0);

	m_combAuSwitch.InsertString(0, ConvertString("OFF"));
	m_combAuSwitch.InsertString(1, ConvertString("ON"));
	m_combAuSwitch.SetCurSel(0);

	m_combInput.InsertString(0, ConvertString("Line in"));
	m_combInput.InsertString(1, ConvertString("MIC"));
	
	
	
	m_combInput.SetCurSel(0);

	m_combProtocol.InsertString(0, "PELCO-D");
	m_combProtocol.InsertString(1, "PELCO-P");
	m_combProtocol.SetCurSel(0);

	m_combBaud.InsertString(0, "110");
	m_combBaud.InsertString(1, "300");
	m_combBaud.InsertString(2, "1200");
	m_combBaud.InsertString(3, "2400");
	m_combBaud.InsertString(4, "4800");
	m_combBaud.InsertString(5, "9600");
	m_combBaud.InsertString(6, "19200");
	m_combBaud.InsertString(7, "38400");
	m_combBaud.InsertString(8, "57600");
	m_combBaud.SetCurSel(0);

	m_combDataBit.InsertString(0, "5");
	m_combDataBit.InsertString(1, "6");
	m_combDataBit.InsertString(2, "7");
	m_combDataBit.InsertString(3, "8");
	m_combDataBit.SetCurSel(0);

	m_combStopBit.InsertString(0, "1");
	m_combStopBit.InsertString(1, "2");
	m_combStopBit.SetCurSel(0);

	m_combParity.InsertString(0, ConvertString("None"));
	m_combParity.InsertString(1, ConvertString("Odd"));
	m_combParity.InsertString(2, ConvertString("Even"));
	m_combParity.SetCurSel(0);

	m_combRec.InsertString(0, "AVI");
	m_combRec.InsertString(1, "ASF");
	m_combRec.InsertString(2, "264");
	m_combRec.SetCurSel(0);

	SetDlgItemText(IDC_EDIT_PTZDATA, "FF010008001F28");

	m_pAI_MM = new CHI_AI_MM();
	m_pAO_MM = new CHI_AO_MM();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSamnetlibDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSamnetlibDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSamnetlibDlg::OnBtnCnt() 
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_BOOL bStream = HI_TRUE;

	if(m_uiHandle == 0)
	{
		UpdateData();
		s32Ret = HI_NET_DEV_Login(&m_uiHandle, (LPCTSTR)m_strUname, (LPCTSTR)m_strPword,
						(LPCTSTR)m_strHost, m_uiPort);
		if(HI_SUCCESS == s32Ret)
		{
			HI_NET_DEV_SetReconnect(m_uiHandle, 5000);
			SetDlgItemText(IDC_BTN_CNT, ConvertString("Disconnect"));
			//HI_NET_DEV_SetReconnect(m_uiHandle, 0);
		}
		else
		{
			MessageBox(m_strHost+ConvertString("Connect Failure!"));
			m_uiHandle = 0;
		}

		/*if(!m_bStreamFlag)
		{
			HI_S_STREAM_INFO struStreamInfo;
			HI_NET_DEV_SetEventCallBack(m_uiHandle, OnEventCallback, (HI_VOID*)this);
			HI_NET_DEV_SetStreamCallBack(m_uiHandle, OnStreamCallback, (HI_VOID*)this);
			HI_NET_DEV_SetDataCallBack(m_uiHandle, OnDataCallback, (HI_VOID*)this);
			
			int videoType = m_combType.GetCurSel();
			if(videoType == 1)
				bStream = HI_FALSE;
			struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
			struStreamInfo.blFlag = bStream;
			struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
			struStreamInfo.u8Type = HI_NET_DEV_STREAM_ALL;
			s32Ret = HI_NET_DEV_StartStream(m_uiHandle, &struStreamInfo);
			if (s32Ret != HI_SUCCESS)
			{
				HI_NET_DEV_Logout(m_uiHandle);
				m_uiHandle = 0;
				return;
			}
			
			SetDlgItemText(IDC_BTN_START, ConvertString("Stop Stream"));
			m_bStreamFlag = TRUE;
		}*/

		UpdateData(FALSE);
	}
	else
	{
		StreamStop();

		s32Ret = HI_NET_DEV_Logout(m_uiHandle);
		if(HI_SUCCESS == s32Ret)
		{
			m_uiHandle = 0;
			SetDlgItemText(IDC_BTN_CNT, ConvertString("Connect"));
		}
	}
}

void CSamnetlibDlg::OnBtnStart() 
{
	// TODO: Add your control notification handler code here
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S_STREAM_INFO_EXT struStreamInfo;

	if (m_uiHandle == 0)
		return;

	if(!m_bStreamFlag)
	{
		HI_NET_DEV_SetEventCallBack(m_uiHandle, OnEventCallback, (HI_VOID*)this);
		HI_NET_DEV_SetStreamCallBack(m_uiHandle, OnStreamCallback, (HI_VOID*)this);
		HI_NET_DEV_SetDataCallBack(m_uiHandle, OnDataCallback, (HI_VOID*)this);
		
		int videoType = m_combType.GetCurSel();
		if(videoType == 0)
			struStreamInfo.u32Stream = HI_NET_DEV_STREAM_1;
		else if(videoType == 1)
			struStreamInfo.u32Stream = HI_NET_DEV_STREAM_2;
		else
			struStreamInfo.u32Stream = HI_NET_DEV_STREAM_3;

		struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
		struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
		struStreamInfo.u8Type = HI_NET_DEV_STREAM_ALL;
		s32Ret = HI_NET_DEV_StartStreamExt(m_uiHandle, &struStreamInfo);
		if (s32Ret != HI_SUCCESS)
		{
			printf("%x\n", s32Ret);
			//HI_NET_DEV_Logout(m_uiHandle);
			//m_uiHandle = 0;
			return;
		}

		SetDlgItemText(IDC_BTN_START, ConvertString("Stop Stream"));
		m_bStreamFlag = TRUE;
	}
	else
	{
		StreamStop();
	}
}

void CSamnetlibDlg::StreamStop() 
{
	// TODO: Add your control notification handler code here
	HI_S32 s32Ret = HI_SUCCESS;

	if (m_uiHandle == 0)
		return;

	if(m_bStreamFlag)
	{
		if(m_pAO_MM != NULL)
		{
			m_pAO_MM->HI_AO_Stop();
		}
		
		s32Ret = HI_NET_DEV_StopStream(m_uiHandle);
		if (s32Ret != HI_SUCCESS)
		{
			MessageBox("FAILURT!");
			return;
		}
		
		SetDlgItemText(IDC_BTN_START, ConvertString("Start Stream"));
		m_bStreamFlag = FALSE;
	}

	if(m_hPlay != NULL)
	{
		HI_PLAYER_Stop(m_hPlay);
		HI_PLAYER_Uninitialize(m_hPlay);
		m_hPlay = NULL;
		SetDlgItemText(IDC_BTN_PLAY, "播放");
	}
}

void CSamnetlibDlg::OnCancel() 
{
	if(m_phFile != NULL){
		fclose(m_phFile);
		m_phFile = NULL;
	}

	if(m_pAI_MM != NULL)
	{
		delete m_pAI_MM;
		m_pAI_MM = NULL;
	}
	
	if(m_pAO_MM != NULL)
	{
		m_pAO_MM->HI_AO_Stop();
		delete m_pAO_MM;
		m_pAO_MM = NULL;
	}
	
	CDialog::OnCancel();
}

void CSamnetlibDlg::SaveFile(HI_U8* pu8Buffer, HI_U32 u32Length, HI_S32 u32DataType)
{
	if(NULL == m_phFile){
		return;
	}

	if(m_bFirstFrame == TRUE){
		if(u32DataType != HI_NET_DEV_AV_DATA)
			return;

		HI_S_AVFrame* pstruAV = (HI_S_AVFrame*)pu8Buffer;
		if(pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG){
			if(pstruAV->u32VFrameType == 1){
				m_bFirstFrame = FALSE;
			}else{
				return;
			}
		}else{
			return;
		}
	}

	if(m_bRecord)
		fwrite(pu8Buffer+sizeof(HI_S_AVFrame), 1, u32Length-sizeof(HI_S_AVFrame), m_phFile);
}

void CSamnetlibDlg::OnBtnRecord() 
{
	HI_S32 s32Ret = HI_SUCCESS;
	s32Ret = HI_NET_DEV_GetRecordState(m_uiHandle);
	if(s32Ret == HI_SUCCESS)
	{
		/*if(m_phFile != NULL){
			fclose(m_phFile);
			m_phFile = NULL;
		}*/
		HI_NET_DEV_StopRecord(m_uiHandle);
		m_bRecord = FALSE;
	}else{
		/*m_phFile = fopen("d://video1.hx", "ab+");
		if(NULL == m_phFile)
			return;

		HI_S_SysHeader sSysHeader;
		sSysHeader.struAHeader.u32Format = m_u32Format;
		sSysHeader.struVHeader.u32Width = m_u32Width;
		sSysHeader.struVHeader.u32Height = m_u32Height;
		fwrite(&sSysHeader, 1, sizeof(HI_S_SysHeader), m_phFile);
		m_bFirstFrame = TRUE;*/
		int nType = m_combRec.GetCurSel();
		CString strPath = "";
		if(nType == 0)
			strPath = "D:\\123.avi";
		else if(nType == 1)
			strPath = "D:\\123.asf";
		else if(nType == 2)
			strPath = "D:\\123.264";

		HI_NET_DEV_StartRecord(m_uiHandle, strPath.GetBuffer(0), nType, 0);
		m_bRecord = TRUE;
	}
	
}


void CSamnetlibDlg::OnBtnStartTalk() 
{
	HRESULT hr;
	if(m_pAI_MM == NULL)
	{
		return;
	}

	if(m_bTalk)
	{
		HI_NET_DEV_StopVoice(m_uiHandle);
		m_pAI_MM->HI_AI_Stop();
		m_bTalk = FALSE;
		SetDlgItemText(IDC_BTN_STARTTALK, ConvertString("Talk"));
	}
	else
	{
		hr = NewAI();
		if (HI_SUCCESS != hr)
		{
			return;
		}
		
		if(g_audioType == 1)
			hr = HI_NET_DEV_StartVoice(m_uiHandle, 4); //1-G711, 4-G726
		else
			hr = HI_NET_DEV_StartVoice(m_uiHandle, 1);
		if (HI_SUCCESS != hr)
		{
			MessageBox(ConvertString("Open Talk Failure!"));
			return;
		}

		hr = m_pAI_MM->HI_AI_Start(OnAudioCallbackEx, this);
		if (HI_SUCCESS != hr)
		{
			return;
		}

		m_bTalk = TRUE;
		SetDlgItemText(IDC_BTN_STARTTALK, ConvertString("Stop Talk"));
	}
}

HI_S32 CSamnetlibDlg::NewAI()
{
	PLAYER_ATTR_AUDIO_S audioAttr;

	audioAttr.lSamplesPerSec = 8000;
	audioAttr.lChannels = 1;
	audioAttr.lBitsPerSample = 16;
	audioAttr.lBlockAlign = audioAttr.lChannels * 2;
	audioAttr.lBitRate  = audioAttr.lBlockAlign * audioAttr.lSamplesPerSec * 8;
	audioAttr.length = 0;
	audioAttr.lFrameFlag = 0;
	audioAttr.pReserved = NULL;
	audioAttr.eAEncode = PLAYER_AUDIO_CODEC_FORMAT_WAV;
	
    HRESULT hr = HI_SUCCESS;
    hr = m_pAI_MM->HI_AI_SetInFormat(audioAttr);
	
    return HI_SUCCESS;
}

void CSamnetlibDlg::OnSendData(HI_U8 *pBuf,
							   HI_S32 s32Size,
							   HI_U64 u64TimeStamp)
{
	HI_S32 s32Ret = HI_SUCCESS;

	if (m_uiHandle == 0)
		return;

	printf("%d\n", s32Size);
	s32Ret = HI_NET_DEV_SendVoiceData(m_uiHandle, (char*)pBuf, s32Size, u64TimeStamp);
}

void CSamnetlibDlg::StartAudio(HI_U32 u32Type)
{
	if(NULL != m_pAO_MM)
	{
		PLAYER_ATTR_AUDIO_S audioAttr;
		
		if(u32Type == 1)
		{
			//G726
			audioAttr.eAEncode = PLAYER_AUDIO_CODEC_FORMAT_G726;
			audioAttr.lBitRate = 16000;
			audioAttr.lBitsPerSample = 2;
			audioAttr.lBlockAlign = 1;
			g_audioType = 1;
		}
		else
		{
			//G711
			audioAttr.eAEncode = PLAYER_AUDIO_CODEC_FORMAT_G711A;
			audioAttr.lBitRate = 64000;
			audioAttr.lBitsPerSample = 16;
			audioAttr.lBlockAlign = 2;
			g_audioType = 0;
		}
		audioAttr.lChannels = 1;
		audioAttr.length = 0;
		audioAttr.lFrameFlag = 0;
		audioAttr.lSamplesPerSec = 8000;
		audioAttr.pReserved = 0;
		m_pAO_MM->HI_AO_SetOutFormat(&audioAttr);
		m_pAO_MM->HI_AO_Play(10);
	}
}

void CSamnetlibDlg::OnBtnProduct() 
{
	HI_S32 s32Ret = HI_FAILURE; 

	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}


	HI_S_ProductVendor sProduct;
	s32Ret = HI_NET_DEV_GetConfig(m_uiHandle, 
								  HI_NET_DEV_GET_PRODUCT_VENDOR, 
								  &sProduct, 
								  sizeof(HI_S_ProductVendor));

	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain Info Failure!"));
		return;
	}

	SetDlgItemText(IDC_EDIT_DEVTYPE, sProduct.sProduct);
	SetDlgItemText(IDC_EDIT_VENDOR, sProduct.sVendor);
	
	/*char *sData = (char*)malloc(1024*1024);
	int nSize = 0;
	s32Ret = HI_NET_DEV_SnapJpeg(m_uiHandle, (HI_U8*)sData, 1024*1024, &nSize);
	if(s32Ret == HI_SUCCESS)
	{
	FILE *fp = fopen("D:\\photo.jpg", "wb+");
	if( !fp )
	free(sData);
	
	  fwrite((const char*)sData, 1, nSize, fp);
	  fclose( fp );
	  }
	  free(sData);
	sData = NULL;*/
}

void CSamnetlibDlg::OnBtnGetVideo() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}

	HI_S_Video sVideo;
	// 注：u32Channel和blFlag与HI_S_STREAM_INFO一致
	sVideo.u32Channel = HI_NET_DEV_CHANNEL_1;	
	sVideo.blFlag = HI_TRUE;
	int videoType = m_combType.GetCurSel();
	if(videoType == 1)
		sVideo.blFlag = HI_FALSE;
	s32Ret = HI_NET_DEV_GetConfig(	m_uiHandle,
									HI_NET_DEV_CMD_VIDEO_PARAM, 
									&sVideo, 
									sizeof(HI_S_Video));

	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain Info Failure!"));
		return;
	}

	CString strBuf;
	strBuf.Format("%d", sVideo.u32Bitrate);
	SetDlgItemText(IDC_EDIT_BITRATE, strBuf);
	
	HI_U32 u32Frame = sVideo.u32Frame;
	if(u32Frame <= 0) u32Frame = 1;
	m_combFrameRate.SetCurSel(u32Frame-1);
	
	strBuf.Format("%d", sVideo.u32Iframe);
	SetDlgItemText(IDC_EDIT_IFRAME, strBuf);
	
	m_combLcbr.SetCurSel(sVideo.blCbr);
	
	HI_U32 u32Quality = sVideo.u32ImgQuality;
	if(u32Quality <= 0) u32Quality = 1;
	m_combQuality.SetCurSel(u32Quality-1);
}

void CSamnetlibDlg::OnBtnSetVideo() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S_Video sVideo;
	// 注：u32Channel和blFlag与HI_S_STREAM_INFO一致
	sVideo.u32Channel = HI_NET_DEV_CHANNEL_1;	
	sVideo.blFlag = HI_TRUE;
	int videoType = m_combType.GetCurSel();
	if(videoType == 1)
		sVideo.blFlag = HI_FALSE;
	CString strBuf;
	int nValue = 0;
	
	GetDlgItemText(IDC_EDIT_BITRATE, strBuf);
	nValue = atoi(strBuf);
	sVideo.u32Bitrate = nValue;
	
	GetDlgItemText(IDC_EDIT_IFRAME, strBuf);
	nValue = atoi(strBuf);
	sVideo.u32Iframe = nValue;
	
	sVideo.u32Frame = m_combFrameRate.GetCurSel()+1;
	sVideo.u32ImgQuality = m_combQuality.GetCurSel()+1;
	
	HI_BOOL bLcbr = HI_FALSE;
	if(1 == m_combLcbr.GetCurSel())
		bLcbr = HI_TRUE;
	sVideo.blCbr = bLcbr;
	s32Ret = HI_NET_DEV_SetConfig(	m_uiHandle,
									HI_NET_DEV_CMD_VIDEO_PARAM, 
									&sVideo, 
									sizeof(HI_S_Video));
	
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Setting Video Failure!"));
	}
}

void CSamnetlibDlg::OnBtnGetAudio() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}

	HI_S_Audio sAudio;
	// 注：u32Channel和blFlag与HI_S_STREAM_INFO一致
	sAudio.u32Channel = HI_NET_DEV_CHANNEL_1;	
	sAudio.blFlag = HI_TRUE;
	int videoType = m_combType.GetCurSel();
	if(videoType == 1)
		sAudio.blFlag = HI_FALSE;
	s32Ret = HI_NET_DEV_GetConfig(	m_uiHandle,  			// HI_SDK_GetConfig
									HI_NET_DEV_CMD_AUDIO_PARAM, 
									&sAudio, 
									sizeof(HI_S_Audio));
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain Info Failure!"));
		return;
	}

	int nAuType = 0;
	m_combAuSwitch.SetCurSel(sAudio.blEnable);
	
	switch(sAudio.u32Type)
	{
	case HI_NET_DEV_AUDIO_TYPE_G711:
		break;
	case HI_NET_DEV_AUDIO_TYPE_G726:
		nAuType = 1;
		break;
	case HI_NET_DEV_AUDIO_TYPE_AMR:
		nAuType = 2;
		break;
	default:
		break;
	}
	m_combFormat.SetCurSel(nAuType);
}

void CSamnetlibDlg::OnBtnSetAudio() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}

	HI_U32 u32Format[3] = {HI_NET_DEV_AUDIO_TYPE_G711, HI_NET_DEV_AUDIO_TYPE_G726, HI_NET_DEV_AUDIO_TYPE_AMR};
	int nInput = m_combInput.GetCurSel();
	
	int nValue = 0;
	HI_S_Audio sAudio;
	sAudio.u32Channel = HI_NET_DEV_CHANNEL_1;
	sAudio.blFlag = HI_TRUE;
	int videoType = m_combType.GetCurSel();
	if(videoType == 1)
		sAudio.blFlag = HI_FALSE;
	nValue = m_combFormat.GetCurSel();
	sAudio.u32Type = u32Format[nValue];
	HI_BOOL bEnable = HI_FALSE;
	if(1 == m_combAuSwitch.GetCurSel())
		bEnable = HI_TRUE;
	sAudio.blEnable = bEnable;

	s32Ret = HI_NET_DEV_SetConfig(	m_uiHandle,
									HI_NET_DEV_CMD_AUDIO_PARAM, 
									&sAudio, 
									sizeof(HI_S_Audio));
	
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Setting Audio Failure!"));
		return;
	}

	MessageBox(ConvertString("Settings Success,Must reboot before enable it!"));
}

void CSamnetlibDlg::OnBtnGetMic() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 audioInput = AUDIO_INPUT_MIC;
	s32Ret = HI_NET_DEV_GetConfig(	m_uiHandle,  			// HI_SDK_GetConfig
									HI_NET_DEV_CMD_AUDIO_INPUT, 
									&audioInput, 
									sizeof(HI_S32));
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain Info Failure!"));
		return;
	}
	
	int nInput = 0;
	switch(audioInput)
	{
	case AUDIO_INPUT_MIC:
	nInput = 1;
		break;
	case AUDIO_INPUT_LINE:	
		break;
	default:
		break;
	}
	m_combInput.SetCurSel(nInput);
}

void CSamnetlibDlg::OnBtnSetMic() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}

	HI_U32 u32Input[2] = {AUDIO_INPUT_MIC, AUDIO_INPUT_LINE};
	int nInput = m_combInput.GetCurSel();

	//s32Ret = HI_NET_DEV_SetConfig(	m_uiHandle,
	//								HI_NET_DEV_CMD_AUDIO_INPUT, 
	//								&u32Input[nInput], 
	//								sizeof(HI_U32));
	s32Ret = HI_NET_DEV_SetConfig(	m_uiHandle,
									HI_NET_DEV_CMD_AUDIO_INPUT, 
									&nInput, 
									sizeof(HI_U32));
	
	
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Setting Audio Input Failure!"));
	}
}

void CSamnetlibDlg::OnBtnGetPtz() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}

	HI_S_PTZ sPtz;
	s32Ret = HI_NET_DEV_GetConfig(	m_uiHandle,  			// HI_SDK_GetConfig
									HI_NET_DEV_CMD_PTZ_PARAM, 
									&sPtz, 
									sizeof(HI_S_PTZ));

	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain Info Failure!"));
		return;
	}

	CString strBuf;
	strBuf.Format("%d", sPtz.u32Address);
	SetDlgItemText(IDC_EDIT_ADDRESS, strBuf);

	int nProtocol = 0;
	switch(sPtz.u32Protocol)
	{
	case HI_NET_DEV_PTZ_PRO_PELCOD:
		break;
	case HI_NET_DEV_PTZ_PRO_PELCOP:
		nProtocol = 1;
	    break;
	default:
	    break;
	}
	m_combProtocol.SetCurSel(nProtocol);

	int nBaud = 0;
	switch(sPtz.u32Baud)
	{
	case HI_NET_DEV_PTZ_B110:
		nBaud = 0;
		break;
	case HI_NET_DEV_PTZ_B300:
		nBaud = 1;
		break;
	case HI_NET_DEV_PTZ_B1200:
		nBaud = 2;
	    break;
	case HI_NET_DEV_PTZ_B2400:
		nBaud = 3;
	    break;
	case HI_NET_DEV_PTZ_B4800:
		nBaud = 4;
		break;
	case HI_NET_DEV_PTZ_B9600:
		nBaud = 5;
		break;
	case HI_NET_DEV_PTZ_B19200:
		nBaud = 6;
	    break;
	case HI_NET_DEV_PTZ_B38400:
		nBaud = 7;
		break;
	case HI_NET_DEV_PTZ_B57600:
		nBaud = 8;
	    break;
	default:
	    break;
	}
	m_combBaud.SetCurSel(nBaud);

	int nDataBit = 0;
	switch(sPtz.u32DataBit)
	{
	case HI_NET_DEV_PTZ_DATA_5:
		nDataBit = 0;
		break;
	case HI_NET_DEV_PTZ_DATA_6:
		nDataBit = 1;
	    break;
	case HI_NET_DEV_PTZ_DATA_7:
		nDataBit = 2;
		break;
	case HI_NET_DEV_PTZ_DATA_8:
		nDataBit = 3;
		break;
	default:
	    break;
	}
	m_combDataBit.SetCurSel(nDataBit);

	int nStopBit = 0;
	switch(sPtz.u32StopBit)
	{
	case HI_NET_DEV_PTZ_STOP_1:
		nStopBit = 0;
		break;
	case HI_NET_DEV_PTZ_STOP_2:
		nStopBit = 1;
	    break;
	default:
	    break;
	}
	m_combStopBit.SetCurSel(nStopBit);

	int nParity = 0;
	switch(sPtz.u32Parity)
	{
	case HI_NET_DEV_PTZ_PARITY_NONE:
		nParity = 0;
		break;
	case HI_NET_DEV_PTZ_PARITY_ODD:
		nParity = 1;
		break;
	case HI_NET_DEV_PTZ_PARITY_EVEN:
		nParity = 2;
		break;
	default:
		break;
	}
	m_combParity.SetCurSel(nParity);
}

void CSamnetlibDlg::OnBtnSetPtz() 
{
	HI_S32 s32Ret = HI_FAILURE; 
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}

	HI_U32 u32Protocol[2]	= {HI_NET_DEV_PTZ_PRO_PELCOD, HI_NET_DEV_PTZ_PRO_PELCOP};
	HI_U32 u32Baud[9]		= {	HI_NET_DEV_PTZ_B110, HI_NET_DEV_PTZ_B300, HI_NET_DEV_PTZ_B1200,
								HI_NET_DEV_PTZ_B2400, HI_NET_DEV_PTZ_B4800, HI_NET_DEV_PTZ_B9600,
								HI_NET_DEV_PTZ_B19200, HI_NET_DEV_PTZ_B38400, HI_NET_DEV_PTZ_B57600};
	HI_U32 u32DataBit[4]	= {HI_NET_DEV_PTZ_DATA_5, HI_NET_DEV_PTZ_DATA_6, HI_NET_DEV_PTZ_DATA_7, HI_NET_DEV_PTZ_DATA_8};
	HI_U32 u32StopBit[2]	= {HI_NET_DEV_PTZ_STOP_1, HI_NET_DEV_PTZ_STOP_2};
	HI_U32 u32Parity[3]		= {HI_NET_DEV_PTZ_PARITY_NONE, HI_NET_DEV_PTZ_PARITY_ODD, HI_NET_DEV_PTZ_PARITY_EVEN};
	
	int nValue = 0;
	CString strBuf;
	HI_S_PTZ sPtz;
	
	GetDlgItemText(IDC_EDIT_ADDRESS, strBuf);
	nValue = atoi(strBuf);
	if(nValue<0 || nValue>255){
		MessageBox(ConvertString("Address range is 0 to 255!"));
		return;
	}
	
	sPtz.u32Address = nValue;
	
	nValue = m_combProtocol.GetCurSel();
	sPtz.u32Protocol = u32Protocol[nValue];
	nValue = m_combBaud.GetCurSel();
	sPtz.u32Baud = u32Baud[nValue];
	nValue = m_combDataBit.GetCurSel();
	sPtz.u32DataBit = u32DataBit[nValue];
	nValue = m_combStopBit.GetCurSel();
	sPtz.u32StopBit = u32StopBit[nValue];
	nValue = m_combParity.GetCurSel();
	sPtz.u32Parity = u32Parity[nValue];
	
	s32Ret = HI_NET_DEV_SetConfig(	m_uiHandle,
									HI_NET_DEV_CMD_PTZ_PARAM, 
									&sPtz, 
									sizeof(HI_S_PTZ));
	
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain Info Failure!"));
	}
}

void CSamnetlibDlg::OnBtnGetRes() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S_Resolution sResolution;
	sResolution.u32Channel = HI_NET_DEV_CHANNEL_1;
	sResolution.blFlag = HI_TRUE;
	int videoType = m_combType.GetCurSel();
	if(videoType == 1)
		sResolution.blFlag = HI_FALSE;
	
	s32Ret = HI_NET_DEV_GetConfig(	m_uiHandle,
									HI_NET_DEV_CMD_RESOLUTION, 
									&sResolution, 
									sizeof(HI_S_Resolution));
	
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain Info Failure!"));
		return;
	}

	//2014/03/26
	HI_DEVICE_INFO DevInfo;
	memset(&DevInfo, 0, sizeof(HI_DEVICE_INFO));
	s32Ret = HI_NET_DEV_GetConfig(m_uiHandle,
								  HI_NET_DEV_CMD_DEVICE_INFO, 
								  &DevInfo,
								  sizeof(HI_DEVICE_INFO));
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Obtain device info Failure!"));
		return;
	}

	CString strReslution[15] = {"VGA", "QVGA", "QQVGA", "D1", "CIF", "QCIF", "720P", "Q720", "QQ720", "UXQA", "960H", "Q960H", "QQ960H", "1080P", "960P"};
	int index = 0;

	m_combResolution.ResetContent();
	int nItem = 0;
	if(strstr(DevInfo.aszServerModel, "Sc") != NULL || strstr(DevInfo.aszServerModel, "Sf") != NULL)
	{
		if(index == 0)
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_720P]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_720P);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_Q720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_Q720);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_960P]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_960P);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_VGA]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_VGA);
		}
		else if(index == 1)
		{
			if(sResolution.u32Resolution == HI_RESOLUTION_960P||sResolution.u32Resolution == HI_RESOLUTION_VGA)
			{
				nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_VGA]);
				m_combResolution.SetItemData(nItem, HI_RESOLUTION_VGA);
				nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QVGA]);
				m_combResolution.SetItemData(nItem, HI_RESOLUTION_QVGA);
				
			}
			else
			{
				nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_Q720]);
				m_combResolution.SetItemData(nItem, HI_RESOLUTION_Q720);
				nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QQ720]);
				m_combResolution.SetItemData(nItem, HI_RESOLUTION_QQ720);
			}
		}
		else
		{
			if(sResolution.u32Resolution == HI_RESOLUTION_960P||sResolution.u32Resolution == HI_RESOLUTION_VGA)
			{
				nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QVGA]);
				m_combResolution.SetItemData(nItem, HI_RESOLUTION_QVGA);
			}
			else
			{
				nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QQ720]);
				m_combResolution.SetItemData(nItem, HI_RESOLUTION_QQ720);
			}
		}
	}
	/************************************************************************/
	/* 1080P-720P IMX122-Sg,OV2715-Si,MN34041-Sj                            */
	/************************************************************************/
	else if(strstr(DevInfo.aszServerModel, "Sg") != NULL||strstr(DevInfo.aszServerModel, "Si") != NULL||strstr(DevInfo.aszServerModel, "Sj") != NULL)
	{
		
		if(index == 0)
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_1080P]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_1080P);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_720P]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_720P);
		}
		else if(index == 1)
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_Q720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_Q720);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QQ720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_QQ720);
		}
		else
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QQ720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_QQ720);
		}	
	}
	/************************************************************************/
	/* 720P OV8712-Se,OV10633-Sh				                            */
	/************************************************************************/
	else if(strstr(DevInfo.aszServerModel, "Se") != NULL||strstr(DevInfo.aszServerModel, "Sh") != NULL)
	{
		
		if(index == 0)
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_720P]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_720P);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_Q720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_Q720);
		}
		else if(index == 1)
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_Q720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_Q720);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QQ720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_QQ720);
		}
		else
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_QQ720]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_QQ720);
		}	
	}
	/************************************************************************/
	/* TVP5150 |Sk| -D1/CIF						                            */
	/************************************************************************/
	else if(strstr(DevInfo.aszServerModel, "Sk") != NULL)
	{
		if(index == 0)
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_D1]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_D1);
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_CIF]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_CIF);
		}
		else if(index == 1)
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_CIF]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_CIF);
		}
		else
		{
			nItem = m_combResolution.AddString(strReslution[HI_RESOLUTION_CIF]);
			m_combResolution.SetItemData(nItem, HI_RESOLUTION_CIF);
		}
	}
	else
	{
		int width = 0, height=0;
		switch(sResolution.u32Resolution)
		{
		case HI_RESOLUTION_VGA:
			width = 640; 
			height = 480;
			break;
		case HI_RESOLUTION_QVGA:
			width = 320;
			height = 240;
			break;
		case HI_RESOLUTION_QQVGA:	
			width = 160;
			height = 120;
			break;
		case HI_RESOLUTION_D1:
			width = 704;
			height = 576;
			break;
		case HI_RESOLUTION_CIF:
			width = 352;
			height = 288;
			break;
		case HI_RESOLUTION_QCIF:
			width = 176; 
			height = 144;
			break;
		case HI_RESOLUTION_720P:
			width = 1280;
			height = 720;
			break;
		case HI_RESOLUTION_Q720:
			width = 640;
			height = 352;
			break;
		case HI_RESOLUTION_QQ720:		
			width = 320;
			height = 176;
			break;
		case HI_RESOLUTION_UXGA:
			width = 1600;
			height = 1200;
			break;
		case HI_RESOLUTION_960H:
			width = 928;
			height = 576;
			break;
		case HI_RESOLUTION_Q960H:
			width = 464;
			height = 288;
			break;
		case HI_RESOLUTION_QQ960H:
			width = 224;
			height = 144;
			break;
		case HI_RESOLUTION_1080P:
			width = 1920;
			height = 1080;
			break;
		case HI_RESOLUTION_960P:
			width = 1280;
			height = 960;
			break;
		}
		unsigned int uRes = 0;
		for(int i=0; i<15; i++)
		{
			if(uRes & (0x00000001<<i))
			{
				index = m_combResolution.AddString(strReslution[i]);
				m_combResolution.SetItemData(index, i);
			}
		}
	}


	int nNum = sizeof(strReslution)/sizeof(CString);
	CString strTemp="";
	strTemp.Empty();
	for(int i=0; i<nNum; i++)
	{
		m_combResolution.GetLBText(i, strTemp);
		if(strTemp == strReslution[sResolution.u32Resolution])
		{
			m_combResolution.SetCurSel(i);
			break;
		}
	}


	//m_combResolution.SetCurSel(sResolution.u32Resolution);
}

void CSamnetlibDlg::OnBtnSetRes() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S_Resolution sResolution;
	sResolution.u32Channel = HI_NET_DEV_CHANNEL_1;
	sResolution.blFlag = HI_TRUE;
	int videoType = m_combType.GetCurSel();
	if(videoType == 1)
		sResolution.blFlag = HI_FALSE;


	//sResolution.u32Resolution = m_combResolution.GetCurSel();
	//2014/03/26
	CString strReslution[15] = {"VGA", "QVGA", "QQVGA", "D1", "CIF", "QCIF", "720P", "Q720", "QQ720", "UXQA", "960H", "Q960H", "QQ960H", "1080P", "960P"};
	int nNum = sizeof(strReslution)/sizeof(CString);
	CString strTemp="";
	strTemp.Empty();
	m_combResolution.GetLBText(m_combResolution.GetCurSel() , strTemp);
	for(int j=0; j<nNum; j++)
	{
		if(strTemp == strReslution[j])
			sResolution.u32Resolution = j;
	}



	s32Ret = HI_NET_DEV_SetConfig(	m_uiHandle,
									HI_NET_DEV_CMD_RESOLUTION, 
									&sResolution, 
									sizeof(HI_S_Resolution));
	
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Setting Resolution Failure!"));
	}
}

void CSamnetlibDlg::OnBtnUp() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_UP);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_UP, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnDown() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_DOWN);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_DOWN, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnLeft() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_LEFT);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_LEFT, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnRight() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_RIGHT);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_RIGHT, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnPtzstop() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_STOP);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_STOP, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnPtzZoomIn() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_ZOOMIN);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_ZOOMIN, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnPtzZoomOut() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_ZOOMOUT);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_ZOOMOUT, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnPreset() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	CString strPresetNum = "";
	DWORD dwPresetNum=0;
	
	GetDlgItemText(IDC_EDIT_PRESET, strPresetNum);
	dwPresetNum=atoi(strPresetNum);
	if ((dwPresetNum<0) || (dwPresetNum>256))
	{
		MessageBox(ConvertString("Preset point range is 0 to 255!"));
		return;
	}
	
	s32Ret = HI_NET_DEV_PTZ_Ctrl_Preset(m_uiHandle, HI_NET_DEV_CTRL_PTZ_SET_PRESET, dwPresetNum);
}

void CSamnetlibDlg::OnBtnPreCall() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	CString strPresetNum = "";
	DWORD dwPresetNum=0;
	
	GetDlgItemText(IDC_EDIT_PRESET, strPresetNum);
	dwPresetNum=atoi(strPresetNum);
	
	if ((dwPresetNum<0) || (dwPresetNum>256))
	{
		MessageBox(ConvertString("Preset point range is 0 to 255!"));
		return;
	}
	
	s32Ret = HI_NET_DEV_PTZ_Ctrl_Preset(m_uiHandle, HI_NET_DEV_CTRL_PTZ_GOTO_PRESET, dwPresetNum);
}

void CSamnetlibDlg::OnBtnPtzTrans() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	CString strPtzData="";
	
	GetDlgItemText(IDC_EDIT_PTZDATA, strPtzData);
	
	s32Ret = HI_NET_DEV_PTZ_Fully_Trans(m_uiHandle, strPtzData.GetBuffer(0), strlen(strPtzData));
	if(HI_SUCCESS != s32Ret)
	{
		MessageBox(ConvertString("Setting Trans Failure!"));
	}
}



void CSamnetlibDlg::OnBtnPlay() 
{
	if(NULL == m_hPlay)
	{
		if(m_uiHandle == 0)
			return;

		if(m_u32Width == 0 ||
			m_u32Height == 0)
			return;

		HI_S32 s32Ret = HI_FAILURE;
		s32Ret = HI_PLAYER_Initialize(&m_hPlay);
		if(s32Ret != HI_SUCCESS)
			return;

		s32Ret = HI_PLAYER_SetDrawWnd(m_hPlay, GetDlgItem(IDC_VIEW)->GetSafeHwnd());
		if(s32Ret != HI_SUCCESS)
			return;

		HI_S_SysHeader sSysHeader;
		sSysHeader.struAHeader.u32Format = m_u32Format;
		sSysHeader.struVHeader.u32Width = m_u32Width;
		sSysHeader.struVHeader.u32Height = m_u32Height;

		s32Ret = HI_PLAYER_OpenStream(m_hPlay, (HI_U8*)&sSysHeader, sizeof(HI_S_SysHeader));
		/*PLAYER_ATTR_VIDEO_STREAM_S stVStreamAttr;
		PLAYER_ATTR_AUDIO_S stAStreamAttr;

		stVStreamAttr.eVEncode   = PLAYER_VENC_FORMAT_H264;
		stVStreamAttr.lHeight    = m_u32Height;
		stVStreamAttr.lWidth     = m_u32Width;
		stVStreamAttr.lBitRate   = 20000000;
		stVStreamAttr.lFrameRate = 25;

		s32Ret = HI_PLAYER_SetMediaAttr(m_hPlay,
			PLAYER_ATTR_VIDEO_STREAM, &stVStreamAttr);
		
		if (HI_SUCCESS != s32Ret)
		{
			return;
		}

		HI_U32 u32Format = m_u32Format;
		if(u32Format == HI_NET_DEV_AUDIO_TYPE_G711){
			stAStreamAttr.eAEncode = PLAYER_AUDIO_CODEC_FORMAT_G711A;
			stAStreamAttr.lBitRate			= 64000;
			stAStreamAttr.lBitsPerSample	= 8;
		}else if(u32Format == HI_NET_DEV_AUDIO_TYPE_G726){
			stAStreamAttr.eAEncode = PLAYER_AUDIO_CODEC_FORMAT_G726;
			stAStreamAttr.lBitRate			= 16000;
			stAStreamAttr.lBitsPerSample	= 2;
		}

		stAStreamAttr.lSamplesPerSec = 8000;
		stAStreamAttr.lChannels      = 1;
		stAStreamAttr.lBlockAlign    = 1;
		stAStreamAttr.lFrameFlag     = 0;
		stAStreamAttr.length         = 0;
		stAStreamAttr.pReserved      = NULL;

		s32Ret = HI_PLAYER_SetMediaAttr(m_hPlay,
			PLAYER_ATTR_AUDIO_STREAM, &stAStreamAttr);
		
		if (HI_SUCCESS != s32Ret)
		{
			return;
		}*/

		HI_PLAYER_Play(m_hPlay);

		SetDlgItemText(IDC_BTN_PLAY, ConvertString("Stop"));
		HI_U32 u32Channel = 11;
		if(!m_bStream)
			u32Channel = 12;
		
		HI_NET_DEV_MakeKeyFrame(m_uiHandle, u32Channel);
	}
	else
	{
		HI_PLAYER_Stop(m_hPlay);
		HI_PLAYER_Uninitialize(m_hPlay);
		m_hPlay = NULL;
		SetDlgItemText(IDC_BTN_PLAY, ConvertString("Play"));
		Invalidate();
	}
}

void CSamnetlibDlg::SetCamAttr(HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Format)
{
	m_u32Width = u32Width;
	m_u32Height = u32Height;
	m_u32Format = u32Format;
}

void CSamnetlibDlg::OnBtnIrisbig() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_APERTUREIN);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_APERTUREIN, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnIrissmall() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_APERTUREOUT);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_APERTUREOUT, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnFocusout() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_FOCUSOUT);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_FOCUSOUT, m_combSpeed.GetCurSel());
}

void CSamnetlibDlg::OnBtnFocusin() 
{
	HI_S32 s32Ret = HI_FAILURE;
	
	if(0 == m_uiHandle)
	{
		MessageBox(ConvertString("No connection!"));
		return;
	}
	
	HI_S32 s32Type = m_combPtzType.GetCurSel();
	if(s32Type == 1)
		HI_NET_DEV_PTZ_Ctrl_StandardEx(m_uiHandle, HI_NET_DEV_CTRL_PTZ_FOCUSIN);
	else
		HI_NET_DEV_PTZ_Ctrl_Standard(m_uiHandle, HI_NET_DEV_CTRL_PTZ_FOCUSIN, m_combSpeed.GetCurSel());
}

