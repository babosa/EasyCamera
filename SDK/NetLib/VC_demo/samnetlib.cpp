// samnetlib.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "samnetlib.h"
#include "samnetlibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSamnetlibApp
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

BEGIN_MESSAGE_MAP(CSamnetlibApp, CWinApp)
	//{{AFX_MSG_MAP(CSamnetlibApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSamnetlibApp construction

CSamnetlibApp::CSamnetlibApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSamnetlibApp object

CSamnetlibApp theApp;
int g_audioType = 0;
CString	g_strPath = "";

/////////////////////////////////////////////////////////////////////////////
// CSamnetlibApp initialization

BOOL CSamnetlibApp::InitInstance()
{
	AfxEnableControlContainer();

	GetModuleFileName(NULL, g_strPath.GetBufferSetLength(MAX_PATH+1), MAX_PATH);
	g_strPath.ReleaseBuffer();
	int pos = g_strPath.ReverseFind('\\');
	g_strPath = g_strPath.Left(pos);

	HI_NET_DEV_Init();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CSamnetlibDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//“英文字符串”转换为“当前语言类型的字符串”
CString ConvertString(CString strText)
{
	char *val = new char[200];
	CString strIniPath,strRet;
	
	if(g_strPath != "")
		strIniPath = g_strPath + "\\langchn.ini";
	else
		strIniPath = "./langchn.ini";
	memset(val,0,200);
	GetPrivateProfileString("String", strText, "",
		val, 200, strIniPath);
	//printf("%s\n", val);
	strRet = val;
	if(strRet.GetLength()==0)
	{
		//如果ini文件中不存在对应的字符串，设置为默认值（英文）
		strRet=strText;
	}
	delete[] val;
	return strRet;
}
//设置对话框中的静态文本(英文-->当前语言)   
void g_SetWndStaticText(CWnd * pWnd)
{
	CString strCaption,strText;
	
	//设置主窗口的标题
	pWnd->GetWindowText(strCaption);
	if(strCaption.GetLength()>0)
	{
		strText=ConvertString(strCaption);
		pWnd->SetWindowText(strText);
	}
	
	//设置子窗口的标题
	CWnd * pChild=pWnd->GetWindow(GW_CHILD);
	CString strClassName;
	while(pChild)
	{
		//////////////////////////////////////////////////////////////////////////		
		//Added by Jackbin 2005-03-11
		strClassName = ((CRuntimeClass*)pChild->GetRuntimeClass())->m_lpszClassName;
		if(strClassName == "CEdit")
		{
			//下一个子窗口
			pChild=pChild->GetWindow(GW_HWNDNEXT);
			continue;
		}
		
		//////////////////////////////////////////////////////////////////////////	
		
		//设置子窗口的当前语言文本
		pChild->GetWindowText(strCaption);
		strText=ConvertString(strCaption);
		pChild->SetWindowText(strText);
		
		//下一个子窗口
		pChild=pChild->GetWindow(GW_HWNDNEXT);
	}
}

int CSamnetlibApp::ExitInstance() 
{
	HI_NET_DEV_DeInit();
	
	return CWinApp::ExitInstance();
}
