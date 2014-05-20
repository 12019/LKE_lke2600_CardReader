#include "StdAfx.h"
#include "FileManage.h"

CFileManage::CFileManage(void)
{
#if FILE_XML_SUPPORT
	m_lngRecordCount = 0;
	///////////////XML初始化/////////////////
	IXMLDOMElementPtr  pSetting;
	VARIANT varValue;
	CString strTemp;
	HRESULT hr;
	VariantInit(&varValue);

	try
	{
		TESTHR(CoInitialize(NULL)); 
		hr = m_docPtr.CreateInstance("Msxml2.DOMDocument.4.0");
		if (FAILED(hr))
		{
			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME,"创建XML DOMDocument 对象失败!", 0);
		}
		_variant_t varXml(POS_TERM_DATA);
		_variant_t varOut((bool)TRUE);
		varOut = m_docPtr->load(varXml);

	}
	catch (...)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME,"加载POSXML文件异常", 0);
	}
#endif

}

CFileManage::~CFileManage(void)
{
}

/**********************************************************
* 函数名:
* 功能 : 写跟踪日志,用于跟踪程序流程
* 入口参数: buf--日志内容,字符串格式
			nValue--参数,可选
* 出口参数: 
* 返回值: 
* 备注: 
***********************************************************/
void CFileManage::WriteTraceLog(const char *szFileName, const char *buf, int nValue)
{
	FILE *fp;
	char filePath[200] = {0};
	char szTmp[200] = {0};

	CTime currentTime = CTime::GetCurrentTime();
	int nYear = currentTime.GetYear();
	int nMonth = currentTime.GetMonth();
	int nDay = currentTime.GetDay();
	int nHour = currentTime.GetHour();
	int nMinute = currentTime.GetMinute();
	int nSecond = currentTime.GetSecond();


	strcat(szTmp, szFileName);
	strcat(szTmp, "%04d%02d%02d.log");

	sprintf(filePath,szTmp,nYear,nMonth,nDay);
	fp=fopen(filePath,"at");
	if (fp == NULL)
	{
		if((fp = fopen(filePath, "wt") ) == NULL)//没有就新建一件文件
		{
			return;
		}
	}			

	fprintf(fp,"%02d:%02d:%02d %s: %d\n",nHour,nMinute,nSecond,buf,nValue);

	fclose(fp);
}
#if FILE_TRANS_SUPPORT
/**********************************************************
* 函数名:
* 功能 : 写交易日志
* 入口参数: buf--日志内容,字符串格式
			tradeType--指示当前是发送的交易类型
* 出口参数: 
* 返回值: 
* 备注: 
***********************************************************/
int CFileManage::WriteTransLog(const char *szFileName, unsigned char *szBuf,int nLen, ETradeType tradeType)
{
	char szTmp[200] = {0};
	char szFilePath[200] = {0};
	FILE *fp;
	int i = 0;

	char currTime[20] = {0};

	CTime currentTime = CTime::GetCurrentTime();
	int nYear = currentTime.GetYear();
	int nMonth = currentTime.GetMonth();
	int nDay = currentTime.GetDay();
	int nHour = currentTime.GetHour();
	int nMinute = currentTime.GetMinute();
	int nSecond = currentTime.GetSecond();

	strcat(szTmp, szFileName);
	strcat(szTmp, "%04d%02d%02d.log");


	sprintf(szFilePath,szTmp,nYear,nMonth,nDay);
	fp=fopen(szFilePath,"at");

	if (fp == NULL)
	{
		if((fp = fopen(szFilePath, "wt") ) == NULL)//没有就新建一件文件
		{
			return -1;
		}
	}

	sprintf(currTime,"%02d:%02d:%02d",nHour,nMinute,nSecond);
	fprintf(fp,"%s\n",currTime); //当前时间写进文件

	//写16进制的报文内容
	switch (tradeType)
	{
	case Trade_SignIn:
		fputs("签到:\n",fp);
		break;
	case Trade_SignIn_Resp:
		fputs("签到返回:\n",fp);
		break;
	case Trade_CheckLoadBalance:
		fputs("查询补登余额:\n",fp);
		break;
	case Trade_CheckLoadBalance_Resp:
		fputs("查询补登余额返回:\n",fp);
		break;
	case Trade_Load:
		fputs("补登圈存:\n",fp);
		break;
	case Trade_Load_Resp:
		fputs("补登圈存返回:\n",fp);
		break;
	case Trade_Reverse:
		fputs("冲正:\n",fp);
		break;
	case Trade_Reverse_Resp:
		fputs("冲正返回:\n",fp);
		break;
	default:
		break;

	}
	
	for (i = 0; i <nLen; i++)
	{
		fprintf(fp, "%02x ", szBuf[i]);//这里转化为16进制可见字符
	}

	fputs("\n*************************************************************************\n", fp);
	fclose(fp);
	return 0;
}
#endif

#if FILE_XML_SUPPORT
/**********************************************************
* 函数名:
* 功能 :获取终端保存POS相关数据, 这些数据保存在xml文件中.  
* 入口参数: index--数据索引值
* 出口参数: nValue--查到的整型值
			szValue--查到的字符串值
* 返回值:	0--获取终端数据成功. 
			-1--查找的数据不存在
* 备注:	 查到的数据是通过值nValue还是szValue返回,由它们
		的格式决定, 调用者有义务清楚自己要获取的数据的格式
***********************************************************/
int CFileManage::ReadPOSData(POSParamIndex index, int *nValue, char *szValue)
{
	int i = 0;
	char xmlPath[] = "config/POSParam";
	BOOL bRet = FALSE;
	int nItemCount = 0;
	POSItem termItem;
	int nIndex = 0;

	memset(&termItem, 0x00, sizeof(termItem));

	bRet = FindPath(xmlPath);
	if (bRet)
	{
		nItemCount = GetItemCount();
		for (i = 0; i < nItemCount; i++)
		{
			GetItemProty(i, &termItem);
			sscanf(termItem.index, "%d", &nIndex);
			if (nIndex == index)//找到数据
			{
				if (strcmp(termItem.format, "a") == 0)
				{
					strcpy(szValue, termItem.value);
				}
				else//n
				{
					int nTmp = atoi(termItem.value);
					*nValue = nTmp;
				}
				break;
			}
			
		}

		if (i >= nItemCount)//没找到相应的tag
		{
			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME,"查找的终端POS数据不存在", index);
			return -1;
		}
		return 0;
	}
	return -1;
}
#endif

#if FILE_XML_SUPPORT
/**********************************************************
* 函数名:
* 功能 : 
* 出口参数:
* 入口参数:index--写入的数据索引值
		   nValue--要写入的整型值
		   szValue-要写入的字符串值
* 返回值:0表示操作成功, -1表示操作失败
* 备注: 注意该函数只能修改已存在的item的值, 不能添加新值
        如果要写入的item不存在，则返回-1.
		同样，接口的调用都有义务知道自己写入的item的格式.
***********************************************************/
int CFileManage::WritePOSData(POSParamIndex index, int nValue, char *szValue)
{
	int i = 0;
	char xmlPath[] = "config/POSParam";
	BOOL bRet = FALSE;
	int nItemCount = 0;
	POSItem termItem;
	int nIndex = 0;
	char szTmp[100] = {0};


	memset(&termItem, 0x00, sizeof(termItem));

	bRet = FindPath(xmlPath);
	if (bRet)
	{
		nItemCount = GetItemCount();
		for (i = 0; i < nItemCount; i++)
		{
			GetItemProty(i, &termItem);
			sscanf(termItem.index, "%d", &nIndex);
			if (nIndex == index)//找到数据
			{
				if (index == POS_TRANSNUM_INDEX)//流水号增长到999999就要重新从1开始计
				{
					if (nValue>=999999)
					{
						nValue = 1;
					}
				}
				if (strcmp(termItem.format, "n") == 0)
				{
					sprintf(szTmp, "%d", nValue);//转化为字符串值再写入保存
				}
				else
				{
					strcpy(szTmp, szValue);
				}
				m_pItem->setAttribute(_bstr_t("value"),_variant_t(szTmp));
				m_docPtr->save(_variant_t(POS_TERM_DATA));
				return 0;
			}
		}
	}
	return -1;
}
#endif

#if FILE_XML_SUPPORT
/**********************************************************
* 函数名:
* 功能 : 定位到结点. 
* 入口参数: 
* 出口参数:
* 返回值:
* 备注:	
***********************************************************/
BOOL CFileManage::FindPath(_bstr_t strPath)
{

	VARIANT varValue;
	CString strTemp;
	m_lngRecordCount=0;
	VariantInit(&varValue);

	if(m_docPtr==NULL)
	{
		return FALSE;
	}
	try
	{
		m_pIXMLDOMElement = m_docPtr->selectSingleNode(strPath);
		if (m_pIXMLDOMElement==NULL)
		{
			m_lngRecordCount = 0;
			return FALSE;
		}
		m_lngRecordCount = m_pIXMLDOMElement->GetchildNodes()->Getlength();
	} 
	catch(...)
	{
		m_lngRecordCount=0;
		return FALSE;
	}

	return TRUE;

}
#endif

#if FILE_XML_SUPPORT
//获取结点数目
int CFileManage::GetItemCount()
{
	return m_lngRecordCount;
}
/**********************************************************
* 函数名:
* 功能 : 获到第index个item的属性
* 入口参数: 
* 出口参数:
* 返回值:
* 备注:	
***********************************************************/
void  CFileManage::GetItemProty(int index, POSItem *item)
{
	VARIANT varValue;
	CString strTemp;
	int nTmp = 0;

	nTmp = m_lngRecordCount;
	if (index >= nTmp)
	{
		return;
	}
	if (m_pIXMLDOMElement==NULL)
	{
		return;
	}

	try
	{
		m_pItem = m_pIXMLDOMElement->GetchildNodes()->Getitem(index);
		if (m_pItem==NULL)
		{
			return;
		}

		VariantInit(&varValue);
		varValue = m_pItem->getAttribute(_bstr_t("index"));
		strTemp = varValue.bstrVal;
		strcpy(item->index, strTemp);

	
		VariantInit(&varValue);
		varValue = m_pItem->getAttribute(_bstr_t("format"));
		strTemp = varValue.bstrVal;
		strcpy(item->format,(LPCTSTR)strTemp);

		VariantInit(&varValue);
		varValue = m_pItem->getAttribute(_bstr_t("value"));
		strTemp = varValue.bstrVal;
		strcpy(item->value,(LPCTSTR)strTemp);

	}
	catch (...)
	{
		MessageBox(NULL, "get item失败!", NULL, MB_OK);
	}
}
#endif
