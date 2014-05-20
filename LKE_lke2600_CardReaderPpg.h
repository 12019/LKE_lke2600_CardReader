#if !defined(AFX_LKE_LKE2600_CARDREADERPPG_H__9BF49E89_F16F_44BF_9BB2_51CFF8E7120B__INCLUDED_)
#define AFX_LKE_LKE2600_CARDREADERPPG_H__9BF49E89_F16F_44BF_9BB2_51CFF8E7120B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LKE_lke2600_CardReaderPpg.h : Declaration of the CLKE_lke2600_CardReaderPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderPropPage : See LKE_lke2600_CardReaderPpg.cpp.cpp for implementation.

class CLKE_lke2600_CardReaderPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CLKE_lke2600_CardReaderPropPage)
	DECLARE_OLECREATE_EX(CLKE_lke2600_CardReaderPropPage)

// Constructor
public:
	CLKE_lke2600_CardReaderPropPage();

// Dialog Data
	//{{AFX_DATA(CLKE_lke2600_CardReaderPropPage)
	enum { IDD = IDD_PROPPAGE_LKE_LKE2600_CARDREADER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CLKE_lke2600_CardReaderPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LKE_LKE2600_CARDREADERPPG_H__9BF49E89_F16F_44BF_9BB2_51CFF8E7120B__INCLUDED)
