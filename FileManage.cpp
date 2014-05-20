#include "StdAfx.h"
#include "FileManage.h"

CFileManage::CFileManage(void)
{
#if FILE_XML_SUPPORT
	m_lngRecordCount = 0;
	///////////////XML��ʼ��/////////////////
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
			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME,"����XML DOMDocument ����ʧ��!", 0);
		}
		_variant_t varXml(POS_TERM_DATA);
		_variant_t varOut((bool)TRUE);
		varOut = m_docPtr->load(varXml);

	}
	catch (...)
	{
		CFileManage::WriteTraceLog(TRACE_LOG_FILENAME,"����POSXML�ļ��쳣", 0);
	}
#endif

}

CFileManage::~CFileManage(void)
{
}

/**********************************************************
* ������:
* ���� : д������־,���ڸ��ٳ�������
* ��ڲ���: buf--��־����,�ַ�����ʽ
			nValue--����,��ѡ
* ���ڲ���: 
* ����ֵ: 
* ��ע: 
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
		if((fp = fopen(filePath, "wt") ) == NULL)//û�о��½�һ���ļ�
		{
			return;
		}
	}			

	fprintf(fp,"%02d:%02d:%02d %s: %d\n",nHour,nMinute,nSecond,buf,nValue);

	fclose(fp);
}
#if FILE_TRANS_SUPPORT
/**********************************************************
* ������:
* ���� : д������־
* ��ڲ���: buf--��־����,�ַ�����ʽ
			tradeType--ָʾ��ǰ�Ƿ��͵Ľ�������
* ���ڲ���: 
* ����ֵ: 
* ��ע: 
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
		if((fp = fopen(szFilePath, "wt") ) == NULL)//û�о��½�һ���ļ�
		{
			return -1;
		}
	}

	sprintf(currTime,"%02d:%02d:%02d",nHour,nMinute,nSecond);
	fprintf(fp,"%s\n",currTime); //��ǰʱ��д���ļ�

	//д16���Ƶı�������
	switch (tradeType)
	{
	case Trade_SignIn:
		fputs("ǩ��:\n",fp);
		break;
	case Trade_SignIn_Resp:
		fputs("ǩ������:\n",fp);
		break;
	case Trade_CheckLoadBalance:
		fputs("��ѯ�������:\n",fp);
		break;
	case Trade_CheckLoadBalance_Resp:
		fputs("��ѯ��������:\n",fp);
		break;
	case Trade_Load:
		fputs("����Ȧ��:\n",fp);
		break;
	case Trade_Load_Resp:
		fputs("����Ȧ�淵��:\n",fp);
		break;
	case Trade_Reverse:
		fputs("����:\n",fp);
		break;
	case Trade_Reverse_Resp:
		fputs("��������:\n",fp);
		break;
	default:
		break;

	}
	
	for (i = 0; i <nLen; i++)
	{
		fprintf(fp, "%02x ", szBuf[i]);//����ת��Ϊ16���ƿɼ��ַ�
	}

	fputs("\n*************************************************************************\n", fp);
	fclose(fp);
	return 0;
}
#endif

#if FILE_XML_SUPPORT
/**********************************************************
* ������:
* ���� :��ȡ�ն˱���POS�������, ��Щ���ݱ�����xml�ļ���.  
* ��ڲ���: index--��������ֵ
* ���ڲ���: nValue--�鵽������ֵ
			szValue--�鵽���ַ���ֵ
* ����ֵ:	0--��ȡ�ն����ݳɹ�. 
			-1--���ҵ����ݲ�����
* ��ע:	 �鵽��������ͨ��ֵnValue����szValue����,������
		�ĸ�ʽ����, ����������������Լ�Ҫ��ȡ�����ݵĸ�ʽ
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
			if (nIndex == index)//�ҵ�����
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

		if (i >= nItemCount)//û�ҵ���Ӧ��tag
		{
			CFileManage::WriteTraceLog(TRACE_LOG_FILENAME,"���ҵ��ն�POS���ݲ�����", index);
			return -1;
		}
		return 0;
	}
	return -1;
}
#endif

#if FILE_XML_SUPPORT
/**********************************************************
* ������:
* ���� : 
* ���ڲ���:
* ��ڲ���:index--д�����������ֵ
		   nValue--Ҫд�������ֵ
		   szValue-Ҫд����ַ���ֵ
* ����ֵ:0��ʾ�����ɹ�, -1��ʾ����ʧ��
* ��ע: ע��ú���ֻ���޸��Ѵ��ڵ�item��ֵ, ���������ֵ
        ���Ҫд���item�����ڣ��򷵻�-1.
		ͬ�����ӿڵĵ��ö�������֪���Լ�д���item�ĸ�ʽ.
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
			if (nIndex == index)//�ҵ�����
			{
				if (index == POS_TRANSNUM_INDEX)//��ˮ��������999999��Ҫ���´�1��ʼ��
				{
					if (nValue>=999999)
					{
						nValue = 1;
					}
				}
				if (strcmp(termItem.format, "n") == 0)
				{
					sprintf(szTmp, "%d", nValue);//ת��Ϊ�ַ���ֵ��д�뱣��
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
* ������:
* ���� : ��λ�����. 
* ��ڲ���: 
* ���ڲ���:
* ����ֵ:
* ��ע:	
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
//��ȡ�����Ŀ
int CFileManage::GetItemCount()
{
	return m_lngRecordCount;
}
/**********************************************************
* ������:
* ���� : �񵽵�index��item������
* ��ڲ���: 
* ���ڲ���:
* ����ֵ:
* ��ע:	
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
		MessageBox(NULL, "get itemʧ��!", NULL, MB_OK);
	}
}
#endif
