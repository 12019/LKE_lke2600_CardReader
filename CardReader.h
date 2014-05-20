// CardReader.h: interface for the CCardReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARDREADER_H__D3796EC6_7DD4_4AFC_89A1_8A2D49CC0F6F__INCLUDED_)
#define AFX_CARDREADER_H__D3796EC6_7DD4_4AFC_89A1_8A2D49CC0F6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Com.h"

class CCardReader  
{
public:
	CCardReader();
	virtual ~CCardReader();

	//最大指令缓冲区大小
	typedef enum
	{
		MAX_CMD_LEN = 600
	}MaxBufferSize;

	typedef enum
	{
		_ERR_LKE_SEND = 0x01,
		_ERR_LKE_RECEIVE = 0x02, 
		_ERR_LKE_STATUS = 0x03,
		_ERR_LKE_BCC = 0x04,
		_ERR_LKE_OTHER = 0x05
	}Error_State;

public:
	int Open();
	void Close();
	int ICCardPowerOn();
	int ICCardPowerOff();
	int ICCardAPDUExchange(char *szCmd, int nCmdLen, char *szRecv, int *nRecvLen, int nOption = 0);
	int EjectCard();
	int FindCard();
	int SelectCardType(int nType = 0);
	int Reset();
	
private:
	char BCC(char *szBuffer, int nLen);
	void gCompressAsc(char *soustr, int len, char *desstr);
	int GetRecvData(int *nRetLen , char *szRecvData, int *nRetState);
	void BuildAPDUCmd(char *sendData,int sendLen,char *desstr,int *OutLen);
	void gSplitBcd(char *soustr, int len, char *desstr);
private:
	CCom m_com;

};

#endif // !defined(AFX_CARDREADER_H__D3796EC6_7DD4_4AFC_89A1_8A2D49CC0F6F__INCLUDED_)
