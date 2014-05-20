#if !defined(AFX_LKE_LKE2600_CARDREADER_H__15622541_23CB_4FD6_BE35_C419292096C3__INCLUDED_)
#define AFX_LKE_LKE2600_CARDREADER_H__15622541_23CB_4FD6_BE35_C419292096C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LKE_lke2600_CardReader.h : main header file for LKE_LKE2600_CARDREADER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderApp : See LKE_lke2600_CardReader.cpp for implementation.

class CLKE_lke2600_CardReaderApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LKE_LKE2600_CARDREADER_H__15622541_23CB_4FD6_BE35_C419292096C3__INCLUDED)
