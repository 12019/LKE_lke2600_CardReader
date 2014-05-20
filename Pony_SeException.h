// Pony_SeException.h: interface for the CPony_SeException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PONY_SEEXCEPTION_H__1DA0D16B_4DBD_47A3_995D_E7FC56A2DF78__INCLUDED_)
#define AFX_PONY_SEEXCEPTION_H__1DA0D16B_4DBD_47A3_995D_E7FC56A2DF78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <exception>
#include <eh.h>

using std::exception;

class CPony_SeException :public exception
{
public:
	CPony_SeException(CPony_SeException const& copee);
	CPony_SeException(unsigned int exceptionID, EXCEPTION_POINTERS* exceptionPointers);
	virtual ~CPony_SeException();
	
	unsigned int GetExceptionID() const;
	
	EXCEPTION_POINTERS* GetExceptionPointers() const;
	
	virtual char const * what() const;
	
private:
	
	static void sehTranslator(unsigned int exceptionID, EXCEPTION_POINTERS* exceptionPointers);
	
	unsigned int m_exceptionID;
	EXCEPTION_POINTERS* m_exceptionPointers;
	
	static _se_translator_function m_oldSEHTranslator;
};

#endif // !defined(AFX_PONY_SEEXCEPTION_H__1DA0D16B_4DBD_47A3_995D_E7FC56A2DF78__INCLUDED_)
