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
* 函数名:
* 功能 : 打开设备连接
* 入口参数:
* 出口参数:
* 返回值: 0表示操作成功,否则返回非零值
* 备注: 串口号在配置文件C:\xfs\config\ZTDEV.INI中
		缺省设置为：波特率9600，8位无校验，起始位1位，
		停止位1位
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
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "在配置文件中读串口号失败");
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
* 函数名:
* 功能 : 断开设备连接
* 入口参数:
* 出口参数:
* 返回值: 
* 备注: 
***********************************************************/
void CCardReader::Close()
{
	m_com.Close();
}
/**********************************************************
* 函数名:
* 功能 : IC卡上电
* 入口参数:
* 出口参数:
* 返回值: 0表示操作成功,否返回非0值
* 备注: 上电指令是12H,拆分为31h,32h
***********************************************************/
int CCardReader::ICCardPowerOn()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	//选通IC卡端口.
	/*
	cmd[0]=  0x1b;
	cmd[1]=  0x25;
	cmd[2]=  0x49;
	*/
	szCmd[0]=  0x36; //起始字节
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//长度
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//上电指令
	szCmd[5]=  0x32;


	szCmd[6]=  0x37;//bcc
	szCmd[7]= 0x33;

	szCmd[8]= 0x03;//结束字节

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//写串口数据错误
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
* 函数名:
* 功能 : IC卡下电
* 入口参数:
* 出口参数:
* 返回值: 0表示操作成功,否返回非0值
* 备注: 上电指令是11H,拆分为31h,31h
***********************************************************/
int CCardReader::ICCardPowerOff()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	//选通IC卡端口.
	/*
	cmd[0]=  0x1b;
	cmd[1]=  0x25;
	cmd[2]=  0x49;
	*/
	szCmd[0]=  0x36; //起始字节
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//长度
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//下电指令
	szCmd[5]=  0x31;


	szCmd[6]=  0x37;//bcc
	szCmd[7]= 0x30;

	szCmd[8]= 0x03;//结束字节

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//写串口数据错误
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
* 函数名:
* 功能 : BCC异或操作
* 入口参数:szBuffer-要做BCC的字节数组
			nLen-数组长度
* 出口参数:
* 返回值: BCC结果
* 备注: 
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
* 函数名:
* 功能 : 接收卡机传回的数据
* 入口参数: 
			szRecvData--接收的数据
* 出口参数: nRetState--卡机传回的状态字节
			nRetLen--接收的数据长度
* 返回值: 0表示操作成功，否则返回非0值
* 备注: 由于接收数据的操作比较复杂, 这里再封装一层
		返回数据的格式:头(1)+长度(1)+状态(n, s|data)+bcc(1)+end(1,0x03)
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
	for (i=0; i < (nRet - 1); i++)//去掉了结束字节0x03
	{
		cResult[i] = m_com.m_chRcvbuf[i];//接收到的数据也是拆分状态的,要合并
	}

	gCompressAsc(cResult,strlen(cResult),cAsc);//去除头两个节并成一个字节
	pagLen = (unsigned char )cAsc[1];         //数据域的长度
	nlen  = strlen(cResult)/2;//返回总字节数
	if (nlen > 256)//这种情况下, cASC[1]就无法表示数据域的长度了, 因为数据域已经超过255个字节了,无法用一个字节表示
	{
		pagLen = nlen - 3;//(减去0x60,长度字节,最后的BCC)
	}

	cEnd = cAsc[nlen-1]; //bcc值
	cBCC = BCC(cAsc,nlen-1);//自己计算一个BCC
	
	if (cEnd==cBCC)//比较两个BCC
	{
		memcpy(szRecvData,&cAsc[3],pagLen-1);//取数据域的数据(不包括状态位)

		*nRetLen = pagLen-1;//数据域的数据长度
		*nRetState = (unsigned char)cAsc[2];   //状态位
		return 0;
	}
	else
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "接收的数据BCC错误");
		return _ERR_LKE_BCC;
	}
}
/**********************************************************
* 函数名:
* 功能 : 压缩字节
* 入口参数: 
* 出口参数:
* 返回值: 0表示操作成功，否则返回非0值
* 备注: 比如,"1234", 返回"0x12, 0x34"
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
* 函数名:
* 功能 : 拆字节
* 入口参数: 
* 出口参数:
* 返回值: 0表示操作成功，否则返回非0值
* 备注: 
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
* 函数名:
* 功能 : T=0协议发送APDU
* 入口参数: szCmd--APDU指令, 以字符串的形式发送, 比如:
			应用选择AID, "00A404000D48756157656949442041707001"
* 出口参数:
* 返回值: 0表示操作成功, 否则返回非0值
* 备注: 参数nOption留做扩展用
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
		return _ERR_LKE_SEND;		//写串口数据错误
	}

	Sleep(300);

	nRet =GetRecvData(nRecvLen,DataTemp,&Stauts);
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "接收APDU数据错误", nRet);
		return nRet;
	}

	len = *nRecvLen;
	if (Stauts==0xE7)//sw1, sw2不是90h,00h, 要做进一步判断
	{
		if(DataTemp[len-2]==0x6c)//重发
		{   //需要重发的状态位
			cLe = DataTemp[len-1];	
			gCompressAsc(szCmd,nCmdLen,cAsc); //转成ascii码
			memcpy(SendDataCopy,cAsc,4);     //命令长度;
			SendDataCopy[4]= cLe;            //卡片返回的le位
			memset(SendBCD,0x00,sizeof(SendBCD));  //清空bcd码
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
			SendDataCopy[4]= cLe;            //卡片返回的le位
			memset(SendBCD,0x00,sizeof(SendBCD));  //清空bcd码
			len = 0;
            memset(cBCD,0x00,sizeof(cBCD));
			gSplitBcd(SendDataCopy,5,cBCD);
			BuildAPDUCmd(cBCD,10,SendBCD,&len);
			goto redo;
		}
		else//APDU其它状态，直接返回给上层
		{
			gSplitBcd((char*)DataTemp, len ,(char*)szRecv);
			*nRecvLen = *nRecvLen *2;
		}
		
    }
	else if (Stauts==0x00)//成功
	{
		gSplitBcd((char*)DataTemp, len ,(char*)szRecv);
		*nRecvLen = *nRecvLen *2;
	}
	else//失败
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "接收APDU数据,返回状态错误", Stauts);
		return -1;	
	}
	return 0;
}
/**********************************************************
* 函数名:
* 功能 : 组APDU发送命令包
* 入口参数: 
* 出口参数:
* 返回值: 
* 备注: 由于APDU指令是以字符串的形式由上层传入的, 所以要
		 特殊处理
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
	
	
	Cmd = 0x15;//命令类别, 发送APDU
	cHead = 0x60;//起始字节
	
	gCompressAsc(sendData,sendLen,cAsc);
    sendLen = sendLen/2;
	
	SendDataPag[0]= cHead;
	SendDataPag[1]= sendLen+1; //don't forget 0x15
	SendDataPag[2] = Cmd;

	memcpy(&SendDataPag[3],cAsc,sendLen);	
	cBCC=BCC(SendDataPag,sendLen +3);  //sendlen + 包头1 + 数据长度1 + 命令1
	SendDataPag[3+sendLen] = cBCC ;
	
	gSplitBcd(SendDataPag,sendLen+4,SendBCD);//拆命令
	SendBCD[(sendLen+4)*2]=0x03;
	*OutLen = (sendLen+4)*2+1;
	memcpy(desstr,SendBCD,*OutLen);
}
/**********************************************************
* 函数名:
* 功能 :退卡
* 入口参数: 
* 出口参数:
* 返回值: 0
* 备注: 弹卡指令是16h
***********************************************************/
int CCardReader::EjectCard()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	//选通IC卡端口.
	/*
	cmd[0]=  0x1b;
	cmd[1]=  0x25;
	cmd[2]=  0x49;
	*/
	szCmd[0]=  0x36; //起始字节
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//长度
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//上电指令
	szCmd[5]=  0x36;


	szCmd[6]=  0x37;//bcc
	szCmd[7]= 0x37;

	szCmd[8]= 0x03;//结束字节

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//写串口数据错误
	}
	//弹卡无返回信息
	return 0;
}
/**********************************************************
* 函数名:
* 功能 :退卡
* 入口参数: 
* 出口参数:
* 返回值: 0--无卡, 1表示有卡,负数表示出错
* 备注: 检测卡状态指令,指令是17h, 暂不支持卡上电的情况
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

	szCmd[0]=  0x36;//头
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//长度
	szCmd[3]=  0x31;

	szCmd[4]=  0x31;//指令类别
	szCmd[5]=  0x37;

	szCmd[6]=  0x37;//bcc
	szCmd[7]=  0x36;

	szCmd[8]=  0x03;//end

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return -1;		//写串口数据错误
	}
	Sleep(300);

	if (GetRecvData(&nretLen,retData,&nretStatus)==0)
	{
		if (nretStatus==0x00)
		{
			if ((retData[0]&0x04) == 0x04)//已插卡
			{
				nRet = 1;
			}
			else if ((retData[0]&0x02) == 0x02)//已上电
			{
				nRet = 2; 
			}
			else
			{
				nRet = 0; //无卡
			}
		}
		else
		{
			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "find card 接收数据状态字节不为0", nRet);
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
* 函数名:
* 功能 :选择卡类型()
* 入口参数: nType--卡片类型
* 出口参数:
* 返回值: 0表示操作成功, 否则返回非0值
* 备注: 指令是17h, T, 00h, T=02h表示接触式CPU卡,
		T=22h表示非接CPU卡
***********************************************************/
int CCardReader::SelectCardType(int nType /* = 0 */)
{

	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;

	int nRet = 0;
	
	szCmd[0]=  0x36; //起始字节
	szCmd[1]=  0x30;

	szCmd[2]=  0x30;//长度
	szCmd[3]=  0x33;

	szCmd[4]=  0x31;//上电指令
	szCmd[5]=  0x37;

	if (nType == 0)//接触式CPU
	{
		szCmd[6] = 0x30;
		szCmd[7] = 0x32;

		szCmd[8] = 0x37;
		szCmd[9] = 0x36;
	}
	else //非接
	{
		szCmd[6] = 0x32;
		szCmd[7] = 0x32;

		szCmd[8] = 0x35;
		szCmd[9] = 0x36;
	}


	szCmd[10]= 0x03;//结束字节

	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//写串口数据错误
	}
	return 0;
}
/**********************************************************
* 函数名:
* 功能 : 读卡机复位
* 入口参数:
* 出口参数:
* 返回值: 0表示操作成功, 否则返回非0值
* 备注: 复位命令是:1b, 53
***********************************************************/
int CCardReader::Reset()
{
	char szCmd[MAX_CMD_LEN] = {0};
	char cType[2] = {0}; 
	int nretLen = 0;
    char retData[256] = {0};
	int nretStatus = 0;
	
	int nRet = 0;
	
	szCmd[0]=  0x36; //起始字节
	szCmd[1]=  0x30;
	
	szCmd[2]=  0x30;//长度
	szCmd[3]=  0x32;
	
	szCmd[4]=  0x31;//上电指令
	szCmd[5]=  0x41;
	
	szCmd[6] = 0x35;
	szCmd[7] = 0x33;
		
	szCmd[8] = 0x32;
	szCmd[9] = 0x40;
	
	szCmd[10]= 0x03;//结束字节
	
	nRet = m_com.SendData(szCmd,strlen(szCmd));
	if (nRet != 0)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME, "send data error", nRet);
		return _ERR_LKE_SEND;		//写串口数据错误
	}
	return 0;
}