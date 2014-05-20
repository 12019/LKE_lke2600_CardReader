// LKE_lke2600_CardReaderPpg.cpp : Implementation of the CLKE_lke2600_CardReaderPropPage property page class.

#include "stdafx.h"
#include "LKE_lke2600_CardReader.h"
#include "LKE_lke2600_CardReaderPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CLKE_lke2600_CardReaderPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLKE_lke2600_CardReaderPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CLKE_lke2600_CardReaderPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CLKE_lke2600_CardReaderPropPage, "LKELKE2600CARDREADER.LKElke2600CardReaderPropPage.1",
	0x70105a9a, 0xa194, 0x4685, 0xa5, 0x62, 0xc5, 0xc, 0x64, 0xf7, 0x5a, 0xed)


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderPropPage::CLKE_lke2600_CardReaderPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CLKE_lke2600_CardReaderPropPage

BOOL CLKE_lke2600_CardReaderPropPage::CLKE_lke2600_CardReaderPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_LKE_LKE2600_CARDREADER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderPropPage::CLKE_lke2600_CardReaderPropPage - Constructor

CLKE_lke2600_CardReaderPropPage::CLKE_lke2600_CardReaderPropPage() :
	COlePropertyPage(IDD, IDS_LKE_LKE2600_CARDREADER_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CLKE_lke2600_CardReaderPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderPropPage::DoDataExchange - Moves data between page and properties

void CLKE_lke2600_CardReaderPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CLKE_lke2600_CardReaderPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderPropPage message handlers
