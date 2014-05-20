#pragma once
/*
* �ļ�������
* ����ű��ļ��������ļ�����־�ļ�,�����ļ��ȵĹ���
*/


class CFileManage
{
public:
	CFileManage(void);
	~CFileManage(void);
	//////////////////////////////////////////////////
#if FILE_XML_SUPPORT
	//POS����, XML��, �ն������������ݽṹ
	typedef struct _Item
	{
		char index[10];	//Ψһ��ʶ���ݵ�����ֵ
		char format[4];	//���ݵĸ�ʽ, Ŀǰ֧�����ָ�ʽ,a��ʾ�ַ���,n��ʾ����ֵ
		char value[50];	//������ֵ
	}POSItem;
#endif
	

public:
	static void WriteTraceLog(const char *szFileName, const char *buf, int nValue = 0);
#if FILE_TRANS_SUPPORT
	static int WriteTransLog(const char *szFileName, unsigned char *szBuf,int nLen, ETradeType tradeType);
	///////////////////////////////////////////////////////////////////////
#endif
	///////////////////////////////////////////////////////////////////////
#if FILE_XML_SUPPORT
	int ReadPOSData(POSParamIndex index, int *nValue, char *szValue);
	int WritePOSData(POSParamIndex index, int nValue, char *szValue);
#endif

private:
	//**************************************************************
#if FILE_XML_SUPPORT
	inline void TESTHR( HRESULT _hr ) 
	{ if FAILED(_hr) throw(_hr); }

	int GetItemCount();
	BOOL FindPath(_bstr_t strPath);
	void GetItemProty(int index, POSItem *item);
	////////////////////////////////////////////////////////////////////
	MSXML2::IXMLDOMDocumentPtr m_docPtr;
	MSXML2::IXMLDOMElementPtr  m_pIXMLDOMElement;
	MSXML2::IXMLDOMElementPtr  m_pItem;
	int m_lngRecordCount;
#endif
};
