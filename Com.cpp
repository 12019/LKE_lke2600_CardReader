// Com.cpp: implementation of the CCom class.
// �첽ģʽ(overlapped)
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
**�������ܣ�	�򿪴���
**��ڲ�����	pszCom���˿ںţ�"COM1",����1
				dwInqueue:���ջ��������ֵ
				dwOutQueue:���ͻ��������ֵ

**���ڲ�����	
**����ֵ��  	0��ʾ�ɹ�, ���򷵻ط�0
*****************************************************************/
int CCom::Open(char *pszCom, DWORD dwInQueue, DWORD dwOutQueue)
{
	m_lLastError = ERROR_SUCCESS;
	if( m_bOpened ) //�����Ѵ�
	{
		return m_lLastError;
	}
		
#ifdef SERIAL_OVERLAPPED //�첽ģʽ
	m_hIDComDev = CreateFile(pszCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL );
//	CTool::WriteLogStr("�����첽ģʽ����");
#else
		//ͬ��ģʽ
	m_hIDComDev = CreateFile(pszCom, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
			OPEN_EXISTING, 0, NULL );
//	CTool::WriteLogStr("����ͬ��ģʽ����");
#endif
		
	if( m_hIDComDev == INVALID_HANDLE_VALUE)
	{
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
		
		/*���ø��ֳ�ʱʱ��*/
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = MAXWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
		
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;//�趨д��ʱʱ��, ��ֹ��������
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );
		/*���ø��ֳ�ʱʱ��*/

#ifdef SERIAL_OVERLAPPED
	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );//��������¼�, �����첽ģʽ
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
		
	if(!::SetupComm( m_hIDComDev, dwInQueue, dwOutQueue ))//���û�����
	{
		::CloseHandle(m_hIDComDev);
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
		
	::PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//��մ���Ӳ��������
		
	m_bOpened = TRUE;
	return m_lLastError;
	
}
/*****************************************************************
**�������ܣ�	�򿪴���
**��ڲ�����	nPort���˿ںţ����磺1 ���� ����1
				dwInqueue:���ջ��������ֵ
				dwOutQueue:���ͻ��������ֵ

**���ڲ�����	
**����ֵ��  	0��ʾ�ɹ�, ���򷵻ط�0
*****************************************************************/
int CCom::Open(int nPort, DWORD dwInQueue, DWORD dwOutQueue)
{
	char szPort[15];
	
	m_lLastError = ERROR_SUCCESS;
	if( m_bOpened ) //�����Ѵ�
	{
		return m_lLastError;
	}
	wsprintf( szPort, "COM%d", nPort );

#ifdef SERIAL_OVERLAPPED //�첽ģʽ
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL );
//	CTool::WriteLogStr("�����첽ģʽ����");
#else
	//ͬ��ģʽ
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
		OPEN_EXISTING, 0, NULL );
//	CTool::WriteLogStr("����ͬ��ģʽ����");
#endif

	if( m_hIDComDev == INVALID_HANDLE_VALUE)
	{
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
	
	/*���ø��ֳ�ʱʱ��*/
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = MAXWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;//�趨д��ʱʱ��, ��ֹ��������
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );
	/*���ø��ֳ�ʱʱ��*/

#ifdef SERIAL_OVERLAPPED
	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );//��������¼�, �����첽ģʽ
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
	
	if(!::SetupComm( m_hIDComDev, dwInQueue, dwOutQueue ))//���û�����
	{
		::CloseHandle(m_hIDComDev);
		m_hIDComDev = NULL;
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}

	::PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//��մ���Ӳ��������
	
	m_bOpened = TRUE;
	return m_lLastError;
}

/*****************************************************************
**�������ܣ�	���ò����ʵ���Ϣ
**��ڲ�����
**���ڲ�����	
**����ֵ�� 0��ʾ���óɹ�, ���򷵻ط�0 	
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

//��ȡ�������ݰ�����
int CCom::GetRecvLen()
{
	return m_nRcvlen;
}
/*****************************************************************
**�������ܣ�	��ȡ�������ص�����
**��ڲ�����	chEndChar���������ַ��ͻ�������ʱ��
				nTimeout:�����ݳ�ʱʱ��,��Ϊ��λ

**���ڲ�����	
**����ֵ��  	�ɹ�����0, ���򷵻ط�0ֵ(������������)
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
		return m_lLastError;//��Ч���
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
			PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//��մ���Ӳ��������
			return _ERR_TIMEOUT; //�����ݳ�ʱ
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
#ifdef SERIAL_OVERLAPPED //�첽ģʽ��

			if(!::ReadFile( m_hIDComDev, ch,1, &dwBytesRead,&m_OverlappedRead))//һ�ζ�һ���ַ�
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
			//ͬ��ģʽ
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
**�������ܣ�	��ȡ�������ص�����(���ݳ���)
**��ڲ�����	chEndChar���������ַ��ͻ�������ʱ��
				nTimeout:�����ݳ�ʱʱ��,��Ϊ��λ

**���ڲ�����	
**����ֵ��  	�ɹ�����0, ���򷵻ط�0ֵ(������������)
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
		return m_lLastError;//��Ч���
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
			PurgeComm(m_hIDComDev, PURGE_TXCLEAR|PURGE_RXCLEAR);//��մ���Ӳ��������
			return _ERR_TIMEOUT; //�����ݳ�ʱ
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
#ifdef SERIAL_OVERLAPPED //�첽ģʽ��

			if(!::ReadFile( m_hIDComDev, ch,1, &dwBytesRead,&m_OverlappedRead))//һ�ζ�һ���ַ�
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
			//ͬ��ģʽ
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

//�رմ���
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

//���ʹ�������
//�ɹ�����0, ���򷵻ط�0
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
		PurgeComm( m_hIDComDev, PURGE_TXABORT | PURGE_TXCLEAR);//�д�����
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
#ifdef SERIAL_OVERLAPPED
	//�첽ģʽ
	bWriteStat = WriteFile(m_hIDComDev, buffer, size, &dwBytesWritten, &m_OverlappedWrite);
	if (!bWriteStat)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			DRes = WaitForSingleObject(m_OverlappedWrite.hEvent, 5000);	// �����첽I/O
			if (DRes == WAIT_OBJECT_0)//����,��һ���ɹ�
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
	//ͬ��ģʽ
	bWriteStat = WriteFile(m_hIDComDev, buffer, size, &dwBytesWritten, NULL);
	if(!bWriteStat)//д����ʧ��
	{
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}

#endif

	return m_lLastError;
}
