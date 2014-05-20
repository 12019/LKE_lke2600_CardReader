// Pony_SeException.cpp: implementation of the CPony_SeException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LKE_lke2600_CardReader.h"
#include "Pony_SeException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CASE(nSeCode,szDescription) case nSeCode: \
	sprintf(szDescription, "%s", #nSeCode); \
 break;

_se_translator_function CPony_SeException::m_oldSEHTranslator = _set_se_translator(CPony_SeException::sehTranslator);

CPony_SeException::CPony_SeException(CPony_SeException const& exceptionCopee)
:m_exceptionID(exceptionCopee.m_exceptionID), 
m_exceptionPointers(exceptionCopee.m_exceptionPointers)
{
}

CPony_SeException::CPony_SeException(unsigned int exceptionID, EXCEPTION_POINTERS* exceptionPointers)
: m_exceptionID(exceptionID), m_exceptionPointers(exceptionPointers)
{
}

CPony_SeException::~CPony_SeException()
{
}

unsigned int CPony_SeException::GetExceptionID() const
{
	return m_exceptionID;
}

EXCEPTION_POINTERS* CPony_SeException::GetExceptionPointers() const
{
	return m_exceptionPointers;
}
char const * CPony_SeException::what() const
{
	char strError[100] = {0};
	static char strWhat[200] = {0};
	
	strcat(strWhat, "windows SE exception: ");
	
	switch (m_exceptionID)
	{
		CASE(EXCEPTION_ACCESS_VIOLATION,strError);
		CASE(EXCEPTION_DATATYPE_MISALIGNMENT,strError);
		CASE(EXCEPTION_BREAKPOINT,strError);
		CASE(EXCEPTION_SINGLE_STEP,strError);
		CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED,strError);
		CASE(EXCEPTION_FLT_DENORMAL_OPERAND,strError);
		CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO,strError);
		CASE(EXCEPTION_FLT_INEXACT_RESULT,strError);
		CASE(EXCEPTION_FLT_INVALID_OPERATION,strError);
		CASE(EXCEPTION_FLT_OVERFLOW,strError);
		CASE(EXCEPTION_FLT_STACK_CHECK,strError);
		CASE(EXCEPTION_FLT_UNDERFLOW,strError);
		CASE(EXCEPTION_INT_DIVIDE_BY_ZERO,strError);
		CASE(EXCEPTION_INT_OVERFLOW,strError);
		CASE(EXCEPTION_PRIV_INSTRUCTION,strError);
		CASE(EXCEPTION_IN_PAGE_ERROR,strError);
		CASE(EXCEPTION_ILLEGAL_INSTRUCTION,strError);
		CASE(EXCEPTION_NONCONTINUABLE_EXCEPTION,strError);
		CASE(EXCEPTION_STACK_OVERFLOW,strError);
		CASE(EXCEPTION_INVALID_DISPOSITION,strError);
		CASE(EXCEPTION_GUARD_PAGE,strError);
		CASE(EXCEPTION_INVALID_HANDLE,strError);
	default:
		strcpy(strError, "Unknown exception.");
		break;
	}
	strcat(strWhat, strError);
	return strWhat;
}

void CPony_SeException::sehTranslator(unsigned int exceptionID, 
									  EXCEPTION_POINTERS* exceptionPointers)
{
	throw CPony_SeException(exceptionID, exceptionPointers);
}
