// LKE_lke2600_CardReaderCtl.cpp : Implementation of the CLKE_lke2600_CardReaderCtrl ActiveX Control class.

#include "stdafx.h"
#include "LKE_lke2600_CardReader.h"
#include "LKE_lke2600_CardReaderCtl.h"
#include "LKE_lke2600_CardReaderPpg.h"
#include "CardReader.h"
#include "FileManage.h"
#include "comDev.h"
#include "Pony_SeException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//���ﶨ��ȫ�ֱ���
CCardReader g_cardReader;
long g_nTimeOut;
HANDLE g_hEventKill;
BOOL g_bThreadRun;
HANDLE g_hThread;
unsigned long ThreadProcInsertCard(LPVOID hwnd);

IMPLEMENT_DYNCREATE(CLKE_lke2600_CardReaderCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLKE_lke2600_CardReaderCtrl, COleControl)
	//{{AFX_MSG_MAP(CLKE_lke2600_CardReaderCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(MSG_ICCARD_STATUS,OnExecuteEventIcCardInsert)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CLKE_lke2600_CardReaderCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CLKE_lke2600_CardReaderCtrl)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "Open", Open, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "OpenDevice", OpenDevice, VT_I2, VTS_NONE)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "CloseDevice", CloseDevice, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "EjectCard", EjectCard, VT_I2, VTS_NONE)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "CardPowerOn", CardPowerOn, VT_I4, VTS_NONE)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "CardPowerOff", CardPowerOff, VT_I2, VTS_NONE)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "FindCard", FindCard, VT_I2, VTS_NONE)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "WaitForInsertCard", WaitForInsertCard, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CLKE_lke2600_CardReaderCtrl, "APDUExchange", APDUExchange, VT_I2, VTS_BSTR VTS_I2 VTS_PVARIANT VTS_PI2)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CLKE_lke2600_CardReaderCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CLKE_lke2600_CardReaderCtrl, COleControl)
	//{{AFX_EVENT_MAP(CLKE_lke2600_CardReaderCtrl)
	EVENT_CUSTOM("CardInsert", FireCardInsert, VTS_I2)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CLKE_lke2600_CardReaderCtrl, 1)
	PROPPAGEID(CLKE_lke2600_CardReaderPropPage::guid)
END_PROPPAGEIDS(CLKE_lke2600_CardReaderCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CLKE_lke2600_CardReaderCtrl, "LKELKE2600CARDREADER.LKElke2600CardReaderCtrl.1",
	0xff95882b, 0x5841, 0x4ab2, 0x9a, 0x45, 0x47, 0x4, 0x7c, 0x28, 0x81, 0x98)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CLKE_lke2600_CardReaderCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DLKE_lke2600_CardReader =
		{ 0xe4204929, 0x6e28, 0x41a9, { 0xa2, 0xb1, 0x2e, 0x4e, 0xce, 0x93, 0x15, 0x46 } };
const IID BASED_CODE IID_DLKE_lke2600_CardReaderEvents =
		{ 0x9aed877f, 0xab67, 0x4cdf, { 0xa5, 0x26, 0x58, 0xc5, 0x7a, 0xf, 0xa6, 0x1a } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwLKE_lke2600_CardReaderOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CLKE_lke2600_CardReaderCtrl, IDS_LKE_LKE2600_CARDREADER, _dwLKE_lke2600_CardReaderOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl::CLKE_lke2600_CardReaderCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CLKE_lke2600_CardReaderCtrl

BOOL CLKE_lke2600_CardReaderCtrl::CLKE_lke2600_CardReaderCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_LKE_LKE2600_CARDREADER,
			IDB_LKE_LKE2600_CARDREADER,
			afxRegApartmentThreading,
			_dwLKE_lke2600_CardReaderOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl::CLKE_lke2600_CardReaderCtrl - Constructor

CLKE_lke2600_CardReaderCtrl::CLKE_lke2600_CardReaderCtrl()
{
	InitializeIIDs(&IID_DLKE_lke2600_CardReader, &IID_DLKE_lke2600_CardReaderEvents);

	// TODO: Initialize your control's instance data here.
	g_hEventKill = ::CreateEvent(NULL,TRUE,FALSE,"KillCard");//manual reset, nonsignaled
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl::~CLKE_lke2600_CardReaderCtrl - Destructor

CLKE_lke2600_CardReaderCtrl::~CLKE_lke2600_CardReaderCtrl()
{
	// TODO: Cleanup your control's instance data here.
	::CloseHandle(g_hEventKill);
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl::OnDraw - Drawing function

void CLKE_lke2600_CardReaderCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl::DoPropExchange - Persistence support

void CLKE_lke2600_CardReaderCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl::OnResetState - Reset control to default state

void CLKE_lke2600_CardReaderCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl::AboutBox - Display an "About" box to the user

void CLKE_lke2600_CardReaderCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_LKE_LKE2600_CARDREADER);
	dlgAbout.DoModal();
}
/**********************************************************
* ������:
* ���� : �����ϲ�Ӧ�õ��¼�
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 
* ��ע: �¼��ӿڵĲ�����������:
		0��ʾ�п����,����Ϊ����ֵ
***********************************************************/
void CLKE_lke2600_CardReaderCtrl::OnExecuteEventIcCardInsert(WPARAM wParam,LPARAM lParam)
{
	short Status;
	Status = (short)lParam;
	FireCardInsert(Status);
}


/////////////////////////////////////////////////////////////////////////////
// CLKE_lke2600_CardReaderCtrl message handlers

void CLKE_lke2600_CardReaderCtrl::Open() 
{
	// TODO: Add your dispatch handler code here

}
/**********************************************************
* ������:
* ���� : ���豸����
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�, ���򷵻ط�0ֵ
* ��ע: 
***********************************************************/
short CLKE_lke2600_CardReaderCtrl::OpenDevice() 
{
	// TODO: Add your dispatch handler code here
	int nRet = 0;

	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "���豸");
	try
	{
		nRet = g_cardReader.Open();
	}
	catch (CPony_SeException &e)
	{
		char strErrMsg[300] = {0};
		sprintf(strErrMsg, "address:0x%.8x ", e.GetExceptionPointers()->ExceptionRecord->ExceptionAddress);
		strcat(strErrMsg, e.what());
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, strErrMsg);
		return -2;
	}

	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "���豸ʧ��", nRet);
		return -1;
	}
	return 0;
}
/**********************************************************
* ������:
* ���� : �ر��豸����
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 
* ��ע: 
***********************************************************/
void CLKE_lke2600_CardReaderCtrl::CloseDevice() 
{
	// TODO: Add your dispatch handler code here
	int nRet = 0;
	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�ر��豸");
	g_cardReader.Close();
}
/**********************************************************
* ������:
* ���� : ����
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 
* ��ע: 
***********************************************************/
short CLKE_lke2600_CardReaderCtrl::EjectCard() 
{
	int nRet = 0;
	
	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "����");
	try
	{
		nRet = g_cardReader.EjectCard();
	}
	catch (CPony_SeException &e)
	{
		char strErrMsg[300] = {0};
		sprintf(strErrMsg, "address:0x%.8x ", e.GetExceptionPointers()->ExceptionRecord->ExceptionAddress);
		strcat(strErrMsg, e.what());
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, strErrMsg);
		return -2;
	}

	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "����ʧ��", nRet);
		return -1;
	}
	return 0;
}
/**********************************************************
* ������:
* ���� : ��Ƭ�ϵ�
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0��ʾ�ϵ�ɹ�,���򷵻ط�0ֵ
* ��ע: �ϵ�ʧ�ܿ���ԭ���ǿ��巴��δ�忨
***********************************************************/
long CLKE_lke2600_CardReaderCtrl::CardPowerOn() 
{
	int nRet = 0;
	
	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "��Ƭ�ϵ�");

	try
	{
		nRet = g_cardReader.ICCardPowerOn();
	}
	catch (CPony_SeException &e)
	{
		char strErrMsg[300] = {0};
		sprintf(strErrMsg, "address:0x%.8x ", e.GetExceptionPointers()->ExceptionRecord->ExceptionAddress);
		strcat(strErrMsg, e.what());
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, strErrMsg);
		return -2;
	}
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�ϵ�ʧ��", nRet);
		return -1;
	}
	return 0;
}
/**********************************************************
* ������:
* ���� : ��Ƭ�µ�
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0��ʾ�µ�ɹ�,���򷵻ط�0ֵ
* ��ע: 
***********************************************************/
short CLKE_lke2600_CardReaderCtrl::CardPowerOff() 
{
	// TODO: Add your dispatch handler code here
	int nRet = 0;

	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "��Ƭ�µ�");

	try
	{
		nRet = g_cardReader.ICCardPowerOff();
	}
	catch (CPony_SeException &e)
	{
		char strErrMsg[300] = {0};
		sprintf(strErrMsg, "address:0x%.8x ", e.GetExceptionPointers()->ExceptionRecord->ExceptionAddress);
		strcat(strErrMsg, e.what());
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, strErrMsg);
		return -2;
	}

	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�µ�ʧ��", nRet);
		return -1;
	}
	return 0;
}
/**********************************************************
* ������:
* ���� : ��⿨Ƭ״̬
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0��ʾ�޿�, 1��ʾ�п�
* ��ע: 
***********************************************************/
short CLKE_lke2600_CardReaderCtrl::FindCard() 
{
	// TODO: Add your dispatch handler code here

	int nRet = 0;
	
	try
	{
		nRet = g_cardReader.FindCard();
	}
	catch (CPony_SeException &e)
	{
		char strErrMsg[300] = {0};
		sprintf(strErrMsg, "address:0x%.8x ", e.GetExceptionPointers()->ExceptionRecord->ExceptionAddress);
		strcat(strErrMsg, e.what());
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, strErrMsg);
		return -3;
	}
	
	return nRet;
}
/**********************************************************
* ������:
* ���� : �ȴ��忨���ú������ö��̴߳���
* ��ڲ���: nTimeOut--��ʱʱ��, ��λ��ms
			�����ֵΪ0, ���޵ȴ�
* ���ڲ���:
* ����ֵ: 
* ��ע: 
***********************************************************/
void CLKE_lke2600_CardReaderCtrl::WaitForInsertCard(long nTimeOut) 
{
	// TODO: Add your dispatch handler code here
	DWORD dwID;

	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�ȴ��忨......");

	g_nTimeOut = nTimeOut;

	::ResetEvent(g_hEventKill);//set to nonsignaled
	if (g_bThreadRun)//�߳���������, ǿ�ƹر�
	{
		g_bThreadRun = !g_bThreadRun;
		::CloseHandle(g_hThread);
		::TerminateThread(g_hThread,0);
	}
	
	if (g_hThread != NULL)
	{
		::CloseHandle(g_hThread);
		::TerminateThread(g_hThread,0);
		g_hThread = NULL;
	}
	
	g_bThreadRun = FALSE;
	g_hThread = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadProcInsertCard,(LPVOID)this->m_hWnd,0,&dwID);
	
	if (!g_hThread)
	{
		g_bThreadRun = FALSE;
		g_hThread = NULL;
	}
	else
	{
		g_bThreadRun = TRUE;
	}
}
/**********************************************************
* ������:
* ���� : �忨�̴߳�����
* ��ڲ���:
* ���ڲ���:
* ����ֵ: 
* ��ע: 
***********************************************************/
unsigned long ThreadProcInsertCard(LPVOID hwnd)
{
	int nRet = 0;
	short nType=-1;
	DWORD curTime,delayTime;
	HWND  hwnd1;
	hwnd1 = (HWND)hwnd;
	delayTime = g_nTimeOut;

	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�ȴ��忨�߳�����");
	curTime = GetCurrentTime();
	
	while(1)
	{
		if (delayTime > 0)
		{
			if (GetCurrentTime() - curTime > delayTime)//��ʱ
			{
				PostMessage(hwnd1,MSG_ICCARD_STATUS,0,(LPARAM)ICCARD_STATUS_TIMEOUT);

				CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�ȴ��忨�̳߳�ʱ");
				g_bThreadRun  = FALSE;
				return -1;
			}
		}
		
		if (WaitForSingleObject(g_hEventKill,10) == WAIT_OBJECT_0)
		{
			g_bThreadRun = FALSE;

			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�ȴ��忨�̱߳�ȡ��");
			PostMessage(hwnd1,MSG_ICCARD_STATUS,0,(LPARAM)ICCARD_STATUS_CANCEL);
			ResetEvent(g_hEventKill);
			Sleep(200);
			return -2;
		}
		
		nRet = g_cardReader.FindCard();
		if (nRet == 1)//�п�����
		{  
			PostMessage(hwnd1,MSG_ICCARD_STATUS,0,(LPARAM)ICCARD_STATUS_SUCCESS);
			g_bThreadRun  = FALSE;
			break; 
		}
		else if (nRet == 2)//�����ϵ�,��״̬�ݲ�֧��
		{ 
			PostMessage(hwnd1,MSG_ICCARD_STATUS,0,(LPARAM)ICCARD_STATUS_SUCCESS);
			g_bThreadRun  = FALSE;
			break;
		}
		else if (nRet < 0)
		{
			//��ֹ���ϵ�Ѱ��, ִ��ʧ��ֱ���˳��߳�
			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "Ѱ��ָ��ִ��ʧ��", nRet);
			g_bThreadRun  = FALSE;
			break;
		}
		else
		{
			continue;
		}
		Sleep(200);
	}
	return 0;
}

/**********************************************************
* ������:
* ���� : APDU����
* ��ڲ���:
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�,���򷵻ط�0ֵ
* ��ע: 
***********************************************************/
short CLKE_lke2600_CardReaderCtrl::APDUExchange(LPCTSTR cAPDU, short cAPDULen, VARIANT FAR* rAPDU, short FAR* rAPDULen) 
{
	// TODO: Add your dispatch handler code here
	char OutDataTemp[1024] = {0};
	CString sRet;
	int nRecvLen = 0;
	int nRet = 0;

	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "APDU��ʼ����");

	try
	{
		nRet = g_cardReader.ICCardAPDUExchange((char *)cAPDU, cAPDULen, OutDataTemp, &nRecvLen);
	}
	catch (CPony_SeException &e)
	{
		char strErrMsg[300] = {0};
		sprintf(strErrMsg, "address:0x%.8x ", e.GetExceptionPointers()->ExceptionRecord->ExceptionAddress);
		strcat(strErrMsg, e.what());
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, strErrMsg);
		return -2;
	}

	if (nRet == 0)
	{
		*rAPDULen = nRecvLen;
		sRet= OutDataTemp;
		VariantInit(rAPDU);
		V_VT(rAPDU)=VT_BSTR;
		V_BSTR(rAPDU) = sRet.AllocSysString();
		return 0;

	}
	CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "APDU����ʧ��");
	return -1;
}
