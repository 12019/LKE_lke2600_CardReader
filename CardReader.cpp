// CardReader.cpp: implementation of the CCardReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CardReader.h"
#include "FileManage.h"
#include "comDev.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//C:\xfs\config\ztdev.int

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCardReader::CCardReader()
{

}

CCardReader::~CCardReader()
{

}
/**********************************************************
* ������:
* ���� : ���豸����
* ��ڲ���:
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�,���򷵻ط���ֵ
* ��ע: ���ں��������ļ�C:\xfs\config\ZTDEV.INI��
		ȱʡ����Ϊ��������9600��8λ��У�飬��ʼλ1λ��
		ֹͣλ1λ
***********************************************************/
int CCardReader::Open()
{
	int nPort = 0;
	int nRet = 0;
	char  FileName[128]={0};
	
	sprintf(FileName, "%s", SYSTEM_INI_FILENAME);
	nPort = GetPrivateProfileInt("LKE2600", "COM", 0, FileName);
	if (nPort <= 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "�������ļ��ж����ں�ʧ��");
	}

	nRet = m_com.Open(nPort, 10000, 10000);
	if(nRet == 0)
	{
		nRet = m_com.Setup(CCom::EBaud9600,CCom::EData8,CCom::EParNone,CCom::EStop1);
		if (nRet == 0)
		{
			return 0;
		}
	}
	return -1;
}
/**********************************************************
* ������:
* ���� : �Ͽ��豸����
* ��ڲ���:
* ���ڲ���:
* ����ֵ: 
* ��ע: 
***********************************************************/
void CCardReader::Close()
{
	m_com.Close();
}
/**********************************************************
* ������:
* ���� : IC���ϵ�
* ��ڲ���:
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�,�񷵻ط�0ֵ
* ��ע: �ϵ�ָ����12H,���Ϊ31h,32h
***********************************************************/
int CCardReader::ICCardPowerOn()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	//ѡͨIC���˿�.
	/*
	cmd[0]=  0x1b;
	cmd[1]=  0x25;
	cmd[2]=  0x49;
	*/
	szCmd[0]=  0x36; //��ʼ�ֽ�
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//����
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//�ϵ�ָ��
	szCmd[5]=  0x32;


	szCmd[6]=  0x37;//bcc
	szCmd[7]= 0x33;

	szCmd[8]= 0x03;//�����ֽ�

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//д�������ݴ���
	}

	if (GetRecvData(&nretLen,retData,&nretStatus)==0)
	{
		if (nretStatus==0x00)
		{
			return 0;
		}
	}
	return -1;
}
/**********************************************************
* ������:
* ���� : IC���µ�
* ��ڲ���:
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�,�񷵻ط�0ֵ
* ��ע: �ϵ�ָ����11H,���Ϊ31h,31h
***********************************************************/
int CCardReader::ICCardPowerOff()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	//ѡͨIC���˿�.
	/*
	cmd[0]=  0x1b;
	cmd[1]=  0x25;
	cmd[2]=  0x49;
	*/
	szCmd[0]=  0x36; //��ʼ�ֽ�
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//����
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//�µ�ָ��
	szCmd[5]=  0x31;


	szCmd[6]=  0x37;//bcc
	szCmd[7]= 0x30;

	szCmd[8]= 0x03;//�����ֽ�

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//д�������ݴ���
	}

	if (GetRecvData(&nretLen,retData,&nretStatus)==0)
	{
		if (nretStatus==0x00)
		{
			return 0;
		}
	}
	return -1;
}
/**********************************************************
* ������:
* ���� : BCC������
* ��ڲ���:szBuffer-Ҫ��BCC���ֽ�����
			nLen-���鳤��
* ���ڲ���:
* ����ֵ: BCC���
* ��ע: 
***********************************************************/
char CCardReader::BCC(char *szBuffer, int nLen)
{
	int i;
	char rech =0x00;
	for(i = 0; i < nLen; i++)
	{
		rech ^= *(szBuffer + i);
	}
	return rech;
}
/**********************************************************
* ������:
* ���� : ���տ������ص�����
* ��ڲ���: 
			szRecvData--���յ�����
* ���ڲ���: nRetState--�������ص�״̬�ֽ�
			nRetLen--���յ����ݳ���
* ����ֵ: 0��ʾ�����ɹ������򷵻ط�0ֵ
* ��ע: ���ڽ������ݵĲ����Ƚϸ���, �����ٷ�װһ��
		�������ݵĸ�ʽ:ͷ(1)+����(1)+״̬(n, s|data)+bcc(1)+end(1,0x03)
***********************************************************/
int CCardReader::GetRecvData(int *nRetLen , char *szRecvData, int *nRetState)
{
	char cResult[MAX_CMD_LEN] = {0};
    char cAsc[MAX_CMD_LEN] = {0};
	char cEnd;  
	char cBCC;     
	int  i;
	int  nlen = 0 ;
	int  pagLen = 0;
	int nRet = 0;

	nRet = m_com.ReadByEndChar(0x03, 10);
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_RECEIVE;
	}

	nRet = m_com.GetRecvLen();
	for (i=0; i < (nRet - 1); i++)//ȥ���˽����ֽ�0x03
	{
		cResult[i] = m_com.m_chRcvbuf[i];//���յ�������Ҳ�ǲ��״̬��,Ҫ�ϲ�
	}

	gCompressAsc(cResult,strlen(cResult),cAsc);//ȥ��ͷ�����ڲ���һ���ֽ�
	pagLen = (unsigned char )cAsc[1];         //������ĳ���
	nlen  = strlen(cResult)/2;//�������ֽ���
	if (nlen > 256)//���������, cASC[1]���޷���ʾ������ĳ�����, ��Ϊ�������Ѿ�����255���ֽ���,�޷���һ���ֽڱ�ʾ
	{
		pagLen = nlen - 3;//(��ȥ0x60,�����ֽ�,����BCC)
	}

	cEnd = cAsc[nlen-1]; //bccֵ
	cBCC = BCC(cAsc,nlen-1);//�Լ�����һ��BCC
	
	if (cEnd==cBCC)//�Ƚ�����BCC
	{
		memcpy(szRecvData,&cAsc[3],pagLen-1);//ȡ�����������(������״̬λ)

		*nRetLen = pagLen-1;//����������ݳ���
		*nRetState = (unsigned char)cAsc[2];   //״̬λ
		return 0;
	}
	else
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "���յ�����BCC����");
		return _ERR_LKE_BCC;
	}
}
/**********************************************************
* ������:
* ���� : ѹ���ֽ�
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ������򷵻ط�0ֵ
* ��ע: ����,"1234", ����"0x12, 0x34"
***********************************************************/
void CCardReader::gCompressAsc(char *soustr, int len, char *desstr)
{

	int i=0;
	int ch;
    char tmpstr[2049] = {0};
	
    sprintf( tmpstr, "%*.*s", len, len, soustr );
    for( i=0; i<(len+1)/2; i++ )
    {
		sscanf( tmpstr+i*2, "%02X", &ch);
		desstr[i] = ch & 0xFF;
    }
    desstr[i] ='\0';
}
/**********************************************************
* ������:
* ���� : ���ֽ�
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ������򷵻ط�0ֵ
* ��ע: 
***********************************************************/
void CCardReader::gSplitBcd(char *soustr, int len, char *desstr)
{
	int i;
    strcpy( desstr, "" );
    for( i=0; i<len; i++ )
    {    
		sprintf( desstr, "%s%02X", desstr, soustr[i]&0XFF );
		desstr+=2;
    }
}
/**********************************************************
* ������:
* ���� : T=0Э�鷢��APDU
* ��ڲ���: szCmd--APDUָ��, ���ַ�������ʽ����, ����:
			Ӧ��ѡ��AID, "00A404000D48756157656949442041707001"
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�, ���򷵻ط�0ֵ
* ��ע: ����nOption������չ��
***********************************************************/
int CCardReader::ICCardAPDUExchange(char *szCmd, int nCmdLen, char *szRecv, int *nRecvLen, int nOption)
{
	char SendBCD[MAX_CMD_LEN] = {0};
	char cBCD[MAX_CMD_LEN] = {0};
    char DataTemp[MAX_CMD_LEN] = {0};
	char cAsc[MAX_CMD_LEN] = {0};
	char SendDataCopy[MAX_CMD_LEN] = {0};
	int  len;
	int Stauts;
	char cLe;
	int nRet = 0;
	
	BuildAPDUCmd(szCmd,nCmdLen,SendBCD,&len);
	
redo:
	nRet = m_com.SendData(SendBCD,len);
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//д�������ݴ���
	}

	Sleep(300);

	nRet =GetRecvData(nRecvLen,DataTemp,&Stauts);
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "����APDU���ݴ���", nRet);
		return nRet;
	}

	len = *nRecvLen;
	if (Stauts==0xE7)//sw1, sw2����90h,00h, Ҫ����һ���ж�
	{
		if(DataTemp[len-2]==0x6c)//�ط�
		{   //��Ҫ�ط���״̬λ
			cLe = DataTemp[len-1];	
			gCompressAsc(szCmd,nCmdLen,cAsc); //ת��ascii��
			memcpy(SendDataCopy,cAsc,4);     //�����;
			SendDataCopy[4]= cLe;            //��Ƭ���ص�leλ
			memset(SendBCD,0x00,sizeof(SendBCD));  //���bcd��
			len = 0;
            memset(cBCD,0x00,sizeof(cBCD));
			gSplitBcd(SendDataCopy,5,cBCD);
			BuildAPDUCmd(cBCD,10,SendBCD,&len);
			goto redo;
		}

		else if(DataTemp[len-2]==0x61)//GET RESPONSE
		{   
			cLe = DataTemp[len-1];	
			SendDataCopy[0]= 0x00;
			SendDataCopy[1]= 0xC0;
			SendDataCopy[2]= 0x00;
			SendDataCopy[3]= 0x00;
			SendDataCopy[4]= cLe;            //��Ƭ���ص�leλ
			memset(SendBCD,0x00,sizeof(SendBCD));  //���bcd��
			len = 0;
            memset(cBCD,0x00,sizeof(cBCD));
			gSplitBcd(SendDataCopy,5,cBCD);
			BuildAPDUCmd(cBCD,10,SendBCD,&len);
			goto redo;
		}
		else//APDU����״̬��ֱ�ӷ��ظ��ϲ�
		{
			gSplitBcd((char*)DataTemp, len ,(char*)szRecv);
			*nRecvLen = *nRecvLen *2;
		}
		
    }
	else if (Stauts==0x00)//�ɹ�
	{
		gSplitBcd((char*)DataTemp, len ,(char*)szRecv);
		*nRecvLen = *nRecvLen *2;
	}
	else//ʧ��
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "����APDU����,����״̬����", Stauts);
		return -1;	
	}
	return 0;
}
/**********************************************************
* ������:
* ���� : ��APDU���������
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 
* ��ע: ����APDUָ�������ַ�������ʽ���ϲ㴫���, ����Ҫ
		 ���⴦��
***********************************************************/
void CCardReader::BuildAPDUCmd(char *sendData,int sendLen,char *desstr,int *OutLen)
{

	char SendDataPag[MAX_CMD_LEN] = {0};
	char SendBCD[MAX_CMD_LEN] = {0};
	char cAsc[MAX_CMD_LEN] = {0};
	char Cmd;          
	char cBCC;  
	char cHead;
	char cEnd;	
	
	
	Cmd = 0x15;//�������, ����APDU
	cHead = 0x60;//��ʼ�ֽ�
	
	gCompressAsc(sendData,sendLen,cAsc);
    sendLen = sendLen/2;
	
	SendDataPag[0]= cHead;
	SendDataPag[1]= sendLen+1; //don't forget 0x15
	SendDataPag[2] = Cmd;

	memcpy(&SendDataPag[3],cAsc,sendLen);	
	cBCC=BCC(SendDataPag,sendLen +3);  //sendlen + ��ͷ1 + ���ݳ���1 + ����1
	SendDataPag[3+sendLen] = cBCC ;
	
	gSplitBcd(SendDataPag,sendLen+4,SendBCD);//������
	SendBCD[(sendLen+4)*2]=0x03;
	*OutLen = (sendLen+4)*2+1;
	memcpy(desstr,SendBCD,*OutLen);
}
/**********************************************************
* ������:
* ���� :�˿�
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0
* ��ע: ����ָ����16h
***********************************************************/
int CCardReader::EjectCard()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	//ѡͨIC���˿�.
	/*
	cmd[0]=  0x1b;
	cmd[1]=  0x25;
	cmd[2]=  0x49;
	*/
	szCmd[0]=  0x36; //��ʼ�ֽ�
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//����
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//�ϵ�ָ��
	szCmd[5]=  0x36;


	szCmd[6]=  0x37;//bcc
	szCmd[7]= 0x37;

	szCmd[8]= 0x03;//�����ֽ�

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//д�������ݴ���
	}
	//�����޷�����Ϣ
	return 0;
}
/**********************************************************
* ������:
* ���� :�˿�
* ��ڲ���: 
* ���ڲ���:
* ����ֵ: 0--�޿�, 1��ʾ�п�,������ʾ����
* ��ע: ��⿨״ָ̬��,ָ����17h, �ݲ�֧�ֿ��ϵ�����
***********************************************************/
int CCardReader::FindCard()
{

	char szCmd[MAX_CMD_LEN] = {0};
	int nCmdLen = 0;
	int nRet = 0;

	int nretLen ;
    char retData[MAX_CMD_LEN] = {0};
	char cType[2]; 
	
	int nretStatus;

    memset(retData,0x00,sizeof(retData));
	memset(cType,0x00,sizeof(cType));

	szCmd[0]=  0x36;//ͷ
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//����
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//ָ�����
	szCmd[5]=  0x37;

	szCmd[6]=  0x37;//bcc
	szCmd[7]=  0x36;

	szCmd[8]=  0x03;//end

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return -1;		//д�������ݴ���
	}
	Sleep(300);

	if (GetRecvData(&nretLen,retData,&nretStatus)==0)
	{
		if (nretStatus==0x00)
		{
			if ((retData[0]&0x04) == 0x04)//�Ѳ忨
			{
				nRet = 1;
			}
			else if ((retData[0]&0x02) == 0x02)//���ϵ�
			{
				nRet = 2; 
			}
			else
			{
				nRet = 0; //�޿�
			}
		}
		else
		{
			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "find card ��������״̬�ֽڲ�Ϊ0", nRet);
			nRet = -1;
		}
	}
	else
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "find card receive data error", nRet);
		nRet = -1;
	}
	return nRet;
}
/**********************************************************
* ������:
* ���� :ѡ������()
* ��ڲ���: nType--��Ƭ����
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�, ���򷵻ط�0ֵ
* ��ע: ָ����17h, T, 00h, T=02h��ʾ�Ӵ�ʽCPU��,
		T=22h��ʾ�ǽ�CPU��
***********************************************************/
int CCardReader::SelectCardType(int nType /* = 0 */)
{

	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	szCmd[0]=  0x36; //��ʼ�ֽ�
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//����
	szCmd[3]=  0x33;

	szCmd[4]=  0x31;//�ϵ�ָ��
	szCmd[5]=  0x37;

	if (nType == 0)//�Ӵ�ʽCPU
	{
		szCmd[6] = 0x30;
		szCmd[7] = 0x32;

		szCmd[8] = 0x37;
		szCmd[9] = 0x36;
	}
	else //�ǽ�
	{
		szCmd[6] = 0x32;
		szCmd[7] = 0x32;

		szCmd[8] = 0x35;
		szCmd[9] = 0x36;
	}


	szCmd[10]= 0x03;//�����ֽ�

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//д�������ݴ���
	}
	return 0;
}
/**********************************************************
* ������:
* ���� : ��������λ
* ��ڲ���:
* ���ڲ���:
* ����ֵ: 0��ʾ�����ɹ�, ���򷵻ط�0ֵ
* ��ע: ��λ������:1b, 53
***********************************************************/
int CCardReader::Reset()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;
	
	int nRet = 0;
	
	szCmd[0]=  0x36; //��ʼ�ֽ�
	szCmd[1]=  0x30;
	
	szCmd[2]=  0x30;//����
	szCmd[3]=  0x32;
	
	szCmd[4]=  0x31;//�ϵ�ָ��
	szCmd[5]=  0x41;
	
	szCmd[6] = 0x35;
	szCmd[7] = 0x33;
		
	szCmd[8] = 0x32;
	szCmd[9] = 0x40;
	
	szCmd[10]= 0x03;//�����ֽ�
	
	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//д�������ݴ���
	}
	return 0;
}