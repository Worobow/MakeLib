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

		Main include file for image lib for external usage.

--*/

// properties of exbldr structurs



typedef enum 
{
	none	= 0,			// no use, may use for check for errors
	call	= 0x12340001,	// this is call 
	hook	= 0x12340002,	// this is hook
	entry	= 0x12340003	// 
}ExternLinkActions;


// two hardly encoded structures
// for each different platforms
//
struct EXTERN_HOOK32{
	unsigned long   Hook;
	unsigned long   Org;
	unsigned long   OrgName;
	unsigned long   Lenght;
	unsigned long   Action;
};
struct EXTERN_HOOK64{
	unsigned __int64	Hook;
	unsigned __int64	Org;
	unsigned __int64	OrgName;
	unsigned long		Lenght;
	unsigned long		Action;
};

#define EXTERN_LINK_DEF(N)\
struct EXTERN_LINK__##N{\
	void * NoUse;\
	__type_call_##N  Call;\
	const wchar_t * Name;\
	unsigned long    Lenght;\
	ExternLinkActions Action;\
}

#define EXTERN_HOOK_DEF(N)\
struct EXTERN_HOOK__##N{\
	__type_hook_##N  Hook;\
	__type_hook_##N  Org;\
	const wchar_t * OrgName;\
	unsigned long    Lenght;\
	ExternLinkActions Action;\
}

// external binary link definition record

#pragma section( ".exbldr." , read )

#define EXTERN_CALL(r_type,name,parms) \
typedef r_type ( * __type_call_##name )  parms  ;\
__declspec ( allocate(".exbldr.")) const  volatile EXTERN_LINK_DEF(name) s_##name={	0, 0, L#name, 0, call };\
void * ref_i_call_##name=(void*) &s_##name;

#define EXTERN_HOOK(r_type,lname,name,parms) \
typedef r_type ( * __type_hook_##name )  parms  ;\
__declspec ( allocate(".exbldr.")) const  volatile EXTERN_HOOK_DEF(name) s_##lname={ lname, 0, L#name, 0, hook };\
void * ref_i_hook_##lname=(void*) &s_##lname;

#define EXTERN_HOOK_BY_ADDR(r_type,lname,name,addr,len,parms) \
typedef r_type ( * __type_hook_##name )  parms  ;\
__declspec ( allocate(".exbldr.")) const  volatile EXTERN_HOOK_DEF(name) s_##lname={ lname, (__type_hook_##name)addr, 0, len, hook };\
void * ref_i_hook_##lname=(void*) &s_##lname;

#define SUPPER_INIT_DECARE(N) const volatile void (*ptr_##N )() = (void(*)()) N
