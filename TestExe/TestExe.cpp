// TestExe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
int Fun2 ( int i1, char * cpc2 );
int Fun1 ( int i1, char * cpc2 )
{
	printf("\n%s enter", __FUNCTION__);
	printf("\n<<<<<<<<<<Original Fun1>>>>>>>>>>");
	Fun2(2,"char parametr");
	printf("\n%s exit", __FUNCTION__);
	return 1;
}
int Fun2 ( int i1, char * cpc2 ) 
{
	printf("\n%s enter", __FUNCTION__);
	printf("\n<<<<<<<<<<Original Fun2>>>>>>>>>>");
	printf("\n%s exit", __FUNCTION__);
	return 1;
}
int Fun3 ( int i1, char * cpc2 )
{
	printf("\n%s enter", __FUNCTION__);
	printf("\n<<<<<<<<<<Original Fun3>>>>>>>>>>");
	printf("\n%s exit", __FUNCTION__);
	return 1;
}
int Fun4 ( int i1, char * cpc2 )
{
	printf("\n%s enter", __FUNCTION__);
	printf("\n<<<<<<<<<<Original Fun4>>>>>>>>>>");
	printf("\n%s exit", __FUNCTION__);
	return 1;
}
int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _WIN64 
	printf("\n===============================win 64======================================\n");
#else
	printf("\n===============================win 32======================================\n");
#endif
	printf("\n%s enter", __FUNCTION__);
	printf("\n <<<<<<<<<<<<<<Original main>>>>>>>>>>>");
	Fun1(1,"22");
	Fun3(1,"sdsds");
	Fun4(1,"dsdsds");
	printf("\n%s exit", __FUNCTION__);
	return 0;
}

