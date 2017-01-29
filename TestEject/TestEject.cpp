// TestEject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define DO_HOOK

#ifdef DO_HOOK

int my_func1 ( int a, char * b );
int my_func2 ( int a, char * b );

#endif

#ifdef _WIN64 
EXTERN_CALL ( int, wmain, (int argc, _TCHAR* argv[]) );
#else
EXTERN_CALL ( int, _wmain, (int argc, _TCHAR* argv[]) );
#endif

EXTERN_CALL ( int, Fun3, (int argc, char * argv) );
EXTERN_CALL ( int, Fun4, (int argc, char * argv) );

#ifdef DO_HOOK
EXTERN_HOOK (int,my_func1,Fun1, (int argc, char * argv) );
EXTERN_HOOK (int,my_func2,Fun2, (int argc, char * argv) );

int my_func1 ( int a, char * b )
{
	printf("\n%s enter", __FUNCTION__);
	printf("\n<<hook %s>>", __FUNCTION__);
	int ret = s_my_func1.Org ( a, b );
	printf("\n%s exit", __FUNCTION__);
	return ret;
}

int my_func2 ( int a, char * b )
{
	printf("\n%s enter", __FUNCTION__);
	printf("\n<<hook %s>>", __FUNCTION__);
	int ret = s_my_func2.Org ( a, b );
	printf("\n%s exit", __FUNCTION__);
	return ret;
}
#endif

int _tmain ( int argc, _TCHAR* argv[] )
{
	printf ( "This is supper start module!!!!"  );

#ifdef _WIN64 
	printf ( "=== 64 ==="  );
#else
	printf ( "=== 32 ==="  );
#endif

#if _DEBUG
	printf ( "=== DEBUG ==="  );
#else
	printf ( "=== RELASE ==="  );
#endif

	printf("\n");

	printf("Now we call 'main' function in base-image.\n");
	printf("Must be seen output about then `main` function start.\n");
	printf("There need to check that all of calls of hooked function, was goto its hooks\n");

	printf("We are hook of func1 and func2, then all calls of thus functions must be covered, ours funcs.\n");

#ifdef _WIN64 
	int r0 = s_wmain.Call ( argc, argv );
#else
	int r0 = s__wmain.Call ( argc, argv );
#endif
	
	int r2 = s_Fun3.Call ( argc, "11111" );

	int r4 = s_Fun4.Call ( argc, "11111" );

#ifdef DO_HOOK	
	int r5 = s_my_func1.Hook ( argc, "11111" );
	int r6 = s_my_func1.Org  ( argc, "11111" );
#endif

	return 0;
}

