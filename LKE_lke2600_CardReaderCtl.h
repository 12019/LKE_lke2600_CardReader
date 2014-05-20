#if !defined(AFX_LKE_LKE2600_CARDREADERCTL_H__D3A4E9B0_E920_448A_915A_C905DAA61DE8__INCLUDED_)
#define AFX_LKE_LKE2600_CARDREADERCTL_H__D3A4E9B0_E920_448A_915A_C905DAA61DE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LKE_lke2600_CardReaderCtl.h : Declaration of the CLKE_lke2600_CardReaderCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl : See LKE_lke2600_CardReaderCtl.cpp for implementation.

#define MSG_MAGNETIS_STATUS                   (WM_USER + 3)
#define MSG_ICCARD_STATUS                     (WM_USER+4)

#define MAGNETIS_STATUS_TIMEOUT               -3 //超时
#define MAGNETIS_STATUS_CANCEL                -2//取消进卡
#define MAGNETIS_STATUS_ERROR                 -1//刷卡失败
#define MAGNETIS_STATUS_SUCCESS               0 //刷卡成功

#define ICCARD_STATUS_TIMEOUT                 -3 //超时
#define ICCARD_STATUS_CANCEL                  -2 //取消
#define ICCARD_STATUS_INSTER_ERROR            -4 //卡插反
#define ICCARD_STATUS_NO_CARD                 -1 //未插卡
#define ICCARD_STATUS_SUCCESS                 0  //成功

class CLKE_lke2600_CardReaderCtrl : public COleControl
{
	DECLARE_DYNCREATE(CLKE_lke2600_CardReaderCtrl)

// Constructor
public:
	CLKE_lke2600_CardReaderCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLKE_lke2600_CardReaderCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CLKE_lke2600_CardReaderCtrl();

	DECLARE_OLECREATE_EX(CLKE_lke2600_CardReaderCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CLKE_lke2600_CardReaderCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CLKE_lke2600_CardReaderCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CLKE_lke2600_CardReaderCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CLKE_lke2600_CardReaderCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnExecuteEventIcCardInsert(WPARAM wParam,LPARAM lParam);

// Dispatch maps
	//{{AFX_DISPATCH(CLKE_lke2600_CardReaderCtrl)
	afx_msg void Open();
	afx_msg short OpenDevice();
	afx_msg void CloseDevice();
	afx_msg short EjectCard();
	afx_msg long CardPowerOn();
	afx_msg short CardPowerOff();
	afx_msg short FindCard();
	afx_msg void WaitForInsertCard(long nTimeOut);
	afx_msg short APDUExchange(LPCTSTR cAPDU, short cAPDULen, VARIANT FAR* rAPDU, short FAR* rAPDULen);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CLKE_lke2600_CardReaderCtrl)
	void FireCardInsert(short nState)
		{FireEvent(eventidCardInsert,EVENT_PARAM(VTS_I2), nState);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CLKE_lke2600_CardReaderCtrl)
	dispidOpen = 1L,
	dispidOpenDevice = 2L,
	dispidCloseDevice = 3L,
	dispidEjectCard = 4L,
	dispidCardPowerOn = 5L,
	dispidCardPowerOff = 6L,
	dispidFindCard = 7L,
	dispidWaitForInsertCard = 8L,
	dispidAPDUExchange = 9L,
	eventidCardInsert = 1L,
	//}}AFX_DISP_ID
	};
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LKE_LKE2600_CARDREADERCTL_H__D3A4E9B0_E920_448A_915A_C905DAA61DE8__INCLUDED)
