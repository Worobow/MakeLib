// TestDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TestDll.h"


// This is an example of an exported variable
TESTDLL_API int nTestDll=0;

// This is an example of an exported function.
TESTDLL_API int fnTestDll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see TestDll.h for the class definition
CTestDll::CTestDll()
{
	return;
}
