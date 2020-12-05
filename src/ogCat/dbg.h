/*
//@author:	zrk
//@time:	2020 / 5 / 30
*/

#pragma once
#include "common.h"

#ifdef OGCAT_DBG_RELEASE
#define CAT_ASSERT(x) x;
#define CAT_CALL(x)  x;
#else
#define CAT_ASSERT(x) if(!(x)) { __debugbreak(); }
#define CAT_CALL(x) cat::ClearError();\
					x;\
					CAT_ASSERT(cat::ErrorLog(#x, __FILE__, __LINE__));
#endif


namespace cat {
	void ClearError();
	bool ErrorLog(const char* info, const char* srcfile, int line);
};