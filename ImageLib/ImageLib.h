/***********************************************************************************\
*                                                                                 *
* ImageLib is special for fun edit execution image libruary.                      *
*                                                                                 *
* Copyright (C) 2013  Andrew.W Worobow                                            *
*                            email:worobow@hotmail.com                            *
*                                                                                 *
* This program is free software; you can redistribute it and / or modify          *
* it under the terms of the GNU General Public License as published by            *
* the Free Software Foundation; either version 3 of the License, or               *
* any later version.                                                              *
*                                                                                 *
* This program is distributed in the hope that it will be useful,                 *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the                     *
* GNU General Public License for more details.                                    *
*                                                                                 *
* You should have received a copy of the GNU General Public License               *
* along with this program; if not, write to the Free Software Foundation,         *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301  USA             *
*                                                                                 *
*                                                                                 *
* This library is designed to realize the possibility of disassembly              *
* the binary images into separate components. With the possibility of	           *
* subsequent assembly of these components into an new one executable image back.  *
*                                                                                 *
\***********************************************************************************/

/*++

	Author: 
			
		Andrew.W worobow (worobow@hotmail.com)

	Module Name:
		
		ImageLib.h

	Environment:
		
		User-mode command line

	Comment:

		Main include file for image lib

--*/


#include <windows.h>
#include <conio.h>


#include <dbghelp.h>

#include <map>
#include <list>
#include <vector>

#include <algorithm>

using namespace std;


#ifndef _WIN64

//
// Define unwind information flags for win32.
//

#define UNW_FLAG_NHANDLER       0x0
#define UNW_FLAG_EHANDLER       0x1
#define UNW_FLAG_UHANDLER       0x2
#define UNW_FLAG_CHAININFO      0x4

#endif

/*
extern "C" {
	#include "mscvpdb.h"
	#include "dcvinfo.h"
}
*/


// Class for implementation activity with a relative virtual address.
// RVA it is a such address who have a base point.
// Usualy the base point is a image base virtual address
// But for blocks and sections this base may be set to zero
// And the relative address is offset into this container.
// 'Sizeof' for this class is 4 bytes
// 
class CRELAITIVE_VIRTUAL_ADDRESS
{
	DWORD Value;
	 
public:
	CRELAITIVE_VIRTUAL_ADDRESS()
		:Value(-1){}

	CRELAITIVE_VIRTUAL_ADDRESS(DWORD v)
		:Value(v){}

	CRELAITIVE_VIRTUAL_ADDRESS(int v)
		:Value((DWORD)v){}
	CRELAITIVE_VIRTUAL_ADDRESS(long long v)
		:Value((DWORD)v){}

	static DWORD Get32 ( const CRELAITIVE_VIRTUAL_ADDRESS& v )
	{
		return v.Value;
	}; 
	DWORD Get32 ()
	{
		return Value;
	}

	friend CRELAITIVE_VIRTUAL_ADDRESS			operator -  ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2);
	friend CRELAITIVE_VIRTUAL_ADDRESS			operator +  ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2);
	friend CRELAITIVE_VIRTUAL_ADDRESS			operator &	( const CRELAITIVE_VIRTUAL_ADDRESS &v1,	const DWORD &dw);
	friend bool									operator >= ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 );
	friend bool									operator <= ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 );
	friend bool									operator >  ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 );
	friend bool									operator <  ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 );
	friend bool									operator != ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 );
	friend bool									operator == ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2);
};
__forceinline bool operator == ( const CRELAITIVE_VIRTUAL_ADDRESS &v1,  const CRELAITIVE_VIRTUAL_ADDRESS &v2  )
{
	return v1.Value == v2.Value;
}
__forceinline CRELAITIVE_VIRTUAL_ADDRESS operator -  (const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2)
{
	return CRELAITIVE_VIRTUAL_ADDRESS(v1.Value - v2.Value) ; 
};
__forceinline CRELAITIVE_VIRTUAL_ADDRESS	operator +  (const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2)  
{ 
	return CRELAITIVE_VIRTUAL_ADDRESS(v1.Value + v2.Value) ; 
};
__forceinline CRELAITIVE_VIRTUAL_ADDRESS	operator &	(const CRELAITIVE_VIRTUAL_ADDRESS &v1,						const DWORD &dw)  
{ 
	return CRELAITIVE_VIRTUAL_ADDRESS(v1.Value & dw ) ; 
};
__forceinline bool operator >= ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 )   
{
	return v1.Value >= v2.Value;
};
__forceinline bool operator <= ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 )   
{ 
	return v1.Value <= v2.Value;
};
__forceinline bool operator >  ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 )   
{ 
	return v1.Value >  v2.Value;
};
__forceinline bool operator <  ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 )   
{ 
	return v1.Value <  v2.Value;
};
__forceinline bool operator != ( const CRELAITIVE_VIRTUAL_ADDRESS &v1, const CRELAITIVE_VIRTUAL_ADDRESS &v2 )   
{ 
	return v1.Value != v2.Value;
};


// Class for implementation activity with a full virtual address.
// 'Sizeof' is 8 bytes
class CVIRTUAL_ADDRESS
{
	DWORD64 Value;
	 
public:
	CVIRTUAL_ADDRESS()
		:Value(0){}
	CVIRTUAL_ADDRESS(DWORD64 v)
		:Value(v){}

	friend CRELAITIVE_VIRTUAL_ADDRESS	operator -	(const CVIRTUAL_ADDRESS &, const CVIRTUAL_ADDRESS &);
	friend CVIRTUAL_ADDRESS				operator +	(const  CVIRTUAL_ADDRESS &, const  CRELAITIVE_VIRTUAL_ADDRESS &);

						
	friend bool						operator >= (const CVIRTUAL_ADDRESS &,const CVIRTUAL_ADDRESS & );
	friend bool						operator <= (const CVIRTUAL_ADDRESS &,const CVIRTUAL_ADDRESS & );
	friend bool						operator >  (const CVIRTUAL_ADDRESS &,const CVIRTUAL_ADDRESS & );
	friend bool						operator <  (const CVIRTUAL_ADDRESS &,const CVIRTUAL_ADDRESS & );
	friend bool						operator == (const CVIRTUAL_ADDRESS &,const CVIRTUAL_ADDRESS & );
	friend bool						operator != (const CVIRTUAL_ADDRESS &,const CVIRTUAL_ADDRESS & );

	// get as 32-bit 
	static DWORD32 Get32 ( CVIRTUAL_ADDRESS v)
	{
		return (DWORD32)v.Value;
	};
	static DWORD64 Get64 ( CVIRTUAL_ADDRESS v)
	{
		return v.Value;
	};
	// creazy conversion functions
	// because this class have only one member
	DWORD64 Get64 ()
	{
		return Value;
	}
	DWORD32 Get32 ()
	{
		return (DWORD32)Value;
	}
};


__forceinline CRELAITIVE_VIRTUAL_ADDRESS	operator -	(const CVIRTUAL_ADDRESS &a,const CVIRTUAL_ADDRESS &b) 
{
	DWORD64 d = a.Value - b.Value; 
	return CRELAITIVE_VIRTUAL_ADDRESS ((DWORD)d);
};
__forceinline CVIRTUAL_ADDRESS operator +	(const  CVIRTUAL_ADDRESS &a, const  CRELAITIVE_VIRTUAL_ADDRESS &b)
{ 
	__int32 i32 = CRELAITIVE_VIRTUAL_ADDRESS::Get32(b);
	__int64 disp = i32;
	return  CVIRTUAL_ADDRESS ( a.Value + disp); 
};
__forceinline bool operator >= (const CVIRTUAL_ADDRESS &a,const CVIRTUAL_ADDRESS &b ) 
{ 
	return a.Value >= b.Value ;
};
__forceinline bool operator <= (const CVIRTUAL_ADDRESS &a,const CVIRTUAL_ADDRESS &b ) 	
{ 
	return a.Value <= b.Value ;
};
__forceinline bool operator >  (const CVIRTUAL_ADDRESS &a,const CVIRTUAL_ADDRESS &b )
{ 
	return a.Value >  b.Value ;
};
__forceinline bool operator <  (const CVIRTUAL_ADDRESS &a,const CVIRTUAL_ADDRESS &b )
{ 
	return a.Value <  b.Value ;
};
__forceinline bool operator == (const CVIRTUAL_ADDRESS &a,const CVIRTUAL_ADDRESS &b )
{ 
	return a.Value == b.Value ;
};
__forceinline bool operator != (const CVIRTUAL_ADDRESS &a,const CVIRTUAL_ADDRESS &b )
{ 
	return a.Value != b.Value ;
};

// bit-masks for tags
#define FUNCS_TABLE_REFERENCE		0x00000001
#define NOT_COMPLETE_RELOC_MARKER	0x00000002 // not complete relocs
#define EXBLDR_ORG_REFERENCE		0x00000004

// 
// local includes 

#include "dia2.h"

#include "mspdb.h"

#include "CPdb.h"


#include "CRelocs.h"

#include "CFuncsTable.h"

#include "CSection.h"

#include "CImports.h"
#include "CExports.h"


#include "CImage.h"

#include <capstone.h>
#include "cbitarray.h"
#include "CodeAnalize.h"

#include "exbldr.h"