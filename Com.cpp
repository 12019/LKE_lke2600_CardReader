// Com.cpp: implementation of the CCom class.
// 异步模式(overlapped)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include "Com.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCom::CCom()
{
	memset(m_chRcvbuf, 0x00, sizeof(m_chRcvbuf));
	memset(&m_OverlappedRead, 0x00, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0x00, sizeof(OVERLAPPED));

	m_nRcvlen = 0;
	m_bReadOK = FALSE;
	m_bOpened = FALSE;
}

CCom::~CCom()
{
	Close();
}

/*****************************************************************
**函数功能：	打开串口
**入口参数：	pszCom：端口号，"COM1",串口1
				dwInqueue:接收缓冲区最大值
				dwOutQueue:发送缓冲区最大值

**出口参数：	
**返回值：  	0表示成功, 否则返回非0
*****************************************************************/
int CCom::Open(char *pszCom, DWORD dwInQueue, DWORD dwOutQueue)
{
	m_lLastError = ERROR_SUCCESS;
	if( m_bOpened ) //串口已打开
	{
		return m_lLastError;
	}
		
#ifdef SERIAL_OVERLAPPED //异步模式
	m_hIDComDev = CreateFile(pszCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL );
//	CTool::WriteLogStr("串口异步模式操作");
#else
		//同步模式
	m_hIDComDev = CreateFile(pszCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
			OPEN_EXISTING, 0, NULL );
//	CTool::WriteLogStr("串口同步模式操作");
#endif
		
	if( m_hIDComDev == INVALID_HANDLE_VALUE)
	{
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
		
		/*设置各种超时时间*/
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = MAXWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
		
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;//设定写超时时间, 防止出现阻塞
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );
		/*设置各种超时时间*/

#ifdef SERIAL_OVERLAPPED
	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );//创建检测事件, 用于异步模式
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if ((m_OverlappedRead.hEvent == NULL) || (m_OverlappedWrite.hEvent == NULL))
	{
		::CloseHandle(m_hIDComDev);
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
#endif
		
	assert(dwInQueue >= 16);
	assert(dwOutQueue >= 16);
		
	if(!::SetupComm( m_hIDComDev, dwInQueue, dwOutQueue ))//设置缓冲区
	{
		::CloseHandle(m_hIDComDev);
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
		
	::PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//清空串口硬件缓冲区
		
	m_bOpened = TRUE;
	return m_lLastError;
	
}
/*****************************************************************
**函数功能：	打开串口
**入口参数：	nPort：端口号，例如：1 代表 串口1
				dwInqueue:接收缓冲区最大值
				dwOutQueue:发送缓冲区最大值

**出口参数：	
**返回值：  	0表示成功, 否则返回非0
*****************************************************************/
int CCom::Open(int nPort, DWORD dwInQueue, DWORD dwOutQueue)
{
	char szPort[15];
	
	m_lLastError = ERROR_SUCCESS;
	if( m_bOpened ) //串口已打开
	{
		return m_lLastError;
	}
	wsprintf( szPort, "COM%d", nPort );

#ifdef SERIAL_OVERLAPPED //异步模式
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL );
//	CTool::WriteLogStr("串口异步模式操作");
#else
	//同步模式
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
		OPEN_EXISTING, 0, NULL );
//	CTool::WriteLogStr("串口同步模式操作");
#endif

	if( m_hIDComDev == INVALID_HANDLE_VALUE)
	{
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
	
	/*设置各种超时时间*/
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = MAXWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;//设定写超时时间, 防止出现阻塞
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );
	/*设置各种超时时间*/

#ifdef SERIAL_OVERLAPPED
	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );//创建检测事件, 用于异步模式
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if ((m_OverlappedRead.hEvent == NULL) || (m_OverlappedWrite.hEvent == NULL))
	{
		::CloseHandle(m_hIDComDev);
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
#endif

	assert(dwInQueue >= 16);
	assert(dwOutQueue >= 16);
	
	if(!::SetupComm( m_hIDComDev, dwInQueue, dwOutQueue ))//设置缓冲区
	{
		::CloseHandle(m_hIDComDev);
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}

	::PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//清空串口硬件缓冲区
	
	m_bOpened = TRUE;
	return m_lLastError;
}

/*****************************************************************
**函数功能：	设置波特率等信息
**入口参数：
**出口参数：	
**返回值： 0表示设置成功, 否则返回非0 	
*****************************************************************/
int CCom::Setup(EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits)
{
	m_lLastError = ERROR_SUCCESS;
	
	// Check if the device is open
	if (m_hIDComDev == NULL)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;
		return m_lLastError;
	}
	
	// Obtain the DCB structure for the device
	DCB dcb;
	if (!::GetCommState(m_hIDComDev,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
	
	// Set the new data
	dcb.BaudRate = DWORD(eBaudrate);
	dcb.ByteSize = BYTE(eDataBits);
	dcb.Parity   = BYTE(eParity);
	dcb.StopBits = BYTE(eStopBits);
	
	// Determine if parity is used
	dcb.fParity  = (eParity != EParNone);
	
	// Set the new DCB structure
	if (!::SetCommState(m_hIDComDev,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
	
	// Return successful
	return m_lLastError;
}

//获取接收数据包长度
int CCom::GetRecvLen()
{
	return m_nRcvlen;
}
/*****************************************************************
**函数功能：	读取卡机返回的数据
**入口参数：	chEndChar：读到该字符就会跳出超时间
				nTimeout:读数据超时时间,秒为单位

**出口参数：	
**返回值：  	成功返回0, 否则返回非0值(正整数或负整数)
*****************************************************************/
int CCom::ReadByEndChar(char chEndChar, int nTimeout)
{
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	short RcvCurrent=0;
	char *chInBuf ;
	unsigned long TimeCurr, TimeDelay;
	int i = 0;
	unsigned char ch[2];
	MSG Message;
	
	memset(m_chRcvbuf,'\0', MAXBLOCK);
	m_nRcvlen = 0;
	chInBuf = m_chRcvbuf;
	m_bReadOK = FALSE;
	
	TimeCurr = GetTickCount();
    TimeDelay = nTimeout * 1000;

	m_lLastError = ERROR_SUCCESS;
	if(!m_bOpened || m_hIDComDev == NULL ) 
	{
		m_lLastError = _ERR_INVALID_HANDLE;
		return m_lLastError;//无效句柄
	}

	while(1)
	{
		if( ::PeekMessage(&Message,NULL,0,0,PM_REMOVE) ) 
		{
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}

		if( GetTickCount()-TimeCurr > TimeDelay )
		{
			PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//清空串口硬件缓冲区
			return _ERR_TIMEOUT; //读数据超时
		}
		SleepEx(10,TRUE);
        ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat);
		RcvCurrent = (short) ComStat.cbInQue;
		if( RcvCurrent == 0 ) 
		{
			continue;
		}
		
		for(i=0;i<RcvCurrent;i++ ) 
		{
#ifdef SERIAL_OVERLAPPED //异步模式下

			if(!::ReadFile( m_hIDComDev, ch,1, &dwBytesRead,&m_OverlappedRead))//一次读一个字符
			{
				m_lLastError = ::GetLastError();
				
				if (m_lLastError != ERROR_IO_PENDING)
				{
					return m_lLastError;
				}
				
				switch (::WaitForSingleObject(m_OverlappedRead.hEvent,INFINITE))
				{
				case WAIT_OBJECT_0:
					// The overlapped operation has completed
					if (!::GetOverlappedResult( m_hIDComDev,&m_OverlappedWrite,&dwBytesRead,FALSE))
					{
						m_lLastError = ::GetLastError();
						return m_lLastError;
					}
					break;
				case WAIT_TIMEOUT:
					m_lLastError = ERROR_TIMEOUT;
					return m_lLastError;
				default:
					m_lLastError = ::GetLastError();
					return m_lLastError;
				}
			}
#else
			//同步模式
			if(!::ReadFile( m_hIDComDev, ch,1, 
				&dwBytesRead,NULL))
			{
				SleepEx(10,TRUE);
				continue;
			}

#endif
			
			if (ch[0]==chEndChar)
			{
				m_bReadOK = TRUE;
			}
			
			*chInBuf = ch[0];
			m_nRcvlen++;
			chInBuf++;
		}
		if(m_bReadOK) 
		{
			break;
		}
	
	}	
	*chInBuf = '\0';
	return 0;
}
/*****************************************************************
**函数功能：	读取卡机返回的数据(根据长度)
**入口参数：	chEndChar：读到该字符就会跳出超时间
				nTimeout:读数据超时时间,秒为单位

**出口参数：	
**返回值：  	成功返回0, 否则返回非0值(正整数或负整数)
*****************************************************************/
int CCom::ReadByLen(int nLen, int nTimeout)
{
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	short RcvCurrent=0;
	char *chInBuf ;
	unsigned long TimeCurr, TimeDelay;
	int i = 0;
	unsigned char ch[2];
	MSG Message;
	
	memset(m_chRcvbuf,'\0', MAXBLOCK);
	m_nRcvlen = 0;
	chInBuf = m_chRcvbuf;
	m_bReadOK = FALSE;
	
	TimeCurr = GetTickCount();
    TimeDelay = nTimeout * 1000;

	m_lLastError = ERROR_SUCCESS;
	if(!m_bOpened || m_hIDComDev == NULL ) 
	{
		m_lLastError = _ERR_INVALID_HANDLE;
		return m_lLastError;//无效句柄
	}

	while(1)
	{
		if( ::PeekMessage(&Message,NULL,0,0,PM_REMOVE) ) 
		{
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}

		if( GetTickCount()-TimeCurr > TimeDelay )
		{
			PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//清空串口硬件缓冲区
			return _ERR_TIMEOUT; //读数据超时
		}
		SleepEx(10,TRUE);
        ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat);
		RcvCurrent = (short) ComStat.cbInQue;
		if( RcvCurrent == 0 ) 
		{
			continue;
		}
		
		for(i=0;i<RcvCurrent;i++ ) 
		{
#ifdef SERIAL_OVERLAPPED //异步模式下

			if(!::ReadFile( m_hIDComDev, ch,1, &dwBytesRead,&m_OverlappedRead))//一次读一个字符
			{
				m_lLastError = ::GetLastError();
				
				if (m_lLastError != ERROR_IO_PENDING)
				{
					return m_lLastError;
				}
				
				switch (::WaitForSingleObject(m_OverlappedRead.hEvent,INFINITE))
				{
				case WAIT_OBJECT_0:
					// The overlapped operation has completed
					if (!::GetOverlappedResult( m_hIDComDev,&m_OverlappedWrite,&dwBytesRead,FALSE))
					{
						m_lLastError = ::GetLastError();
						return m_lLastError;
					}
					break;
				case WAIT_TIMEOUT:
					m_lLastError = ERROR_TIMEOUT;
					return m_lLastError;
				default:
					m_lLastError = ::GetLastError();
					return m_lLastError;
				}
			}
#else
			//同步模式
			if(!::ReadFile( m_hIDComDev, ch,1, 
				&dwBytesRead,NULL))
			{
				SleepEx(10,TRUE);
				continue;
			}

#endif
			*chInBuf = ch[0];
			m_nRcvlen++;
			chInBuf++;
		}
		if(m_nRcvlen >= nLen) 
		{
			break;
		}
	
	}	
	*chInBuf = '\0';
	return 0;
}

//关闭串口
int CCom::Close( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return 0;

	if( m_OverlappedRead.hEvent != NULL )
	{
		::CloseHandle( m_OverlappedRead.hEvent );
	}
	
	if( m_OverlappedWrite.hEvent != NULL )
	{
		::CloseHandle( m_OverlappedWrite.hEvent );
	}
	
	::CloseHandle( m_hIDComDev );
	m_bOpened = FALSE;
	m_hIDComDev = NULL;
	return 0;
}

//发送串口数据
//成功返回0, 否则返回非0
int CCom::SendData(const char *buffer, int size)
{
	DWORD dwBytesWritten = 0;
	BOOL bWriteStat;
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	DWORD DRes = 0;

	m_lLastError = ERROR_SUCCESS;

	if(!m_bOpened || m_hIDComDev == NULL ) 
	{
		return _ERR_INVALID_HANDLE;
	}

	if ( ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat ) && (dwErrorFlags > 0))
	{
		PurgeComm( m_hIDComDev, PURGE_TXABORT | PURGE_TXCLEAR);//有错误发生
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
#ifdef SERIAL_OVERLAPPED
	//异步模式
	bWriteStat = WriteFile(m_hIDComDev, buffer, size, &dwBytesWritten, &m_OverlappedWrite);
	if (!bWriteStat)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			DRes = WaitForSingleObject(m_OverlappedWrite.hEvent, 5000);	// 结束异步I/O
			if (DRes == WAIT_OBJECT_0)//读完,不一定成功
			{
				GetOverlappedResult(m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE);
				return m_lLastError;
			}
			else
			{
				m_lLastError = ::GetLastError();
				return m_lLastError;
			}
		}
		else
		{
			m_lLastError = ::GetLastError();
			return m_lLastError;
		}
	}
#else
	//同步模式
	bWriteStat = WriteFile(m_hIDComDev, buffer, size, &dwBytesWritten, NULL);
	if(!bWriteStat)//写串口失败
	{
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}

#endif

	return m_lLastError;
}
