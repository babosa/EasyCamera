// samnetlib.h : main header file for the SAMNETLIB application
//

#if !defined(AFX_SAMNETLIB_H__D6A43FC8_9336_4C97_97DE_E7663F491F32__INCLUDED_)
#define AFX_SAMNETLIB_H__D6A43FC8_9336_4C97_97DE_E7663F491F32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSamnetlibApp:
// See samnetlib.cpp for the implementation of this class
//

class CSamnetlibApp : public CWinApp
{
public:
	CSamnetlibApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSamnetlibApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSamnetlibApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMNETLIB_H__D6A43FC8_9336_4C97_97DE_E7663F491F32__INCLUDED_)
