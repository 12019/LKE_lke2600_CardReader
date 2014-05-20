// Com.h: interface for the CCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COM_H__E256A2B4_59DD_47DB_BBD3_2AB7B242CD23__INCLUDED_)
#define AFX_COM_H__E256A2B4_59DD_47DB_BBD3_2AB7B242CD23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SERIAL_OVERLAPPED
#define SERIAL_OVERLAPPED
#endif

class CCom  
{
public:
	//错误状态码
	typedef enum 
	{
		_ERR_INVALID_HANDLE = -1,
			_ERR_TIMEOUT		= -2,
			_ERR_UNKNOWN		= -3
			
	}EErrorState;
	//最大缓冲区大小
	typedef enum
	{
		MAXBLOCK = 1024
	}MaxBufferSize;
	//波特率
	typedef enum
	{
		EBaudUnknown = -1,			// Unknown
			EBaud110     = CBR_110,		// 110 bits/sec
			EBaud300     = CBR_300,		// 300 bits/sec
			EBaud600     = CBR_600,		// 600 bits/sec
			EBaud1200    = CBR_1200,	// 1200 bits/sec
			EBaud2400    = CBR_2400,	// 2400 bits/sec
			EBaud4800    = CBR_4800,	// 4800 bits/sec
			EBaud9600    = CBR_9600,	// 9600 bits/sec
			EBaud14400   = CBR_14400,	// 14400 bits/sec
			EBaud19200   = CBR_19200,	// 19200 bits/sec (default)
			EBaud38400   = CBR_38400,	// 38400 bits/sec
			EBaud56000   = CBR_56000,	// 56000 bits/sec
			EBaud57600   = CBR_57600,	// 57600 bits/sec
			EBaud115200  = CBR_115200,	// 115200 bits/sec
			EBaud128000  = CBR_128000,	// 128000 bits/sec
			EBaud256000  = CBR_256000,	// 256000 bits/sec
	}EBaudrate;
	//奇偶校验位
	typedef enum
	{
		EParUnknown = -1,			// Unknown
			EParNone    = NOPARITY,		// No parity (default)
			EParOdd     = ODDPARITY,	// Odd parity
			EParEven    = EVENPARITY,	// Even parity
			EParMark    = MARKPARITY,	// Mark parity
			EParSpace   = SPACEPARITY	// Space parity
	}EParity;
	//字节大小
	typedef enum
	{
		EDataUnknown = -1,			// Unknown
			EData5       =  5,			// 5 bits per byte
			EData6       =  6,			// 6 bits per byte
			EData7       =  7,			// 7 bits per byte
			EData8       =  8			// 8 bits per byte (default)
	}EDataBits;
	//停止位
	typedef enum
	{
		EStopUnknown = -1,			// Unknown
		EStop1       = ONESTOPBIT,	// 1 stopbit (default)
		EStop1_5     = ONE5STOPBITS,// 1.5 stopbit
		EStop2       = TWOSTOPBITS	// 2 stopbits
	}EStopBits;

public:
	CCom();
	virtual ~CCom();
public:
	int ReadByEndChar(char chEndChar, int nTimeout);
	int ReadByLen(int nLen, int nTimeout);
	int SendData(const char *, int);
	int GetRecvLen();

	int Open(char *pszCom, DWORD dwInQueue, DWORD dwOutQueue);
	int Open(int nPort, DWORD dwInQueue, DWORD dwOutQueue);
	int Setup(EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits);
	int Close();
	char m_chRcvbuf[MAXBLOCK];
	
private:
	//////////////////////////////////
	int m_nRcvlen;
	HANDLE m_hIDComDev;
	BOOL m_bReadOK;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	BOOL m_bOpened;
	LONG	m_lLastError;		// Last serial error

};

#endif // !defined(AFX_COM_H__E256A2B4_59DD_47DB_BBD3_2AB7B242CD23__INCLUDED_)
