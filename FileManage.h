#pragma once
/*
* 文件管理类
* 负责脚本文件，冲正文件，日志文件,配置文件等的管理
*/


class CFileManage
{
public:
	CFileManage(void);
	~CFileManage(void);
	//////////////////////////////////////////////////
#if FILE_XML_SUPPORT
	//POS参数, XML中, 终端数据属性数据结构
	typedef struct _Item
	{
		char index[10];	//唯一标识数据的索引值
		char format[4];	//数据的格式, 目前支持两种格式,a表示字符串,n表示整型值
		char value[50];	//参数的值
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
