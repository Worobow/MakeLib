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
		
		CImports.h, CImports.cpp

	Environment:
		
		User-mode command line

	Comment:

		class for the implementation of functional work imports

--*/

#include "stdafx.h"

bool CIMPORTS::PrintThunk64 ( CIMAGE * img, PIMAGE_THUNK_DATA64 ch )
{
	if ( IMAGE_SNAP_BY_ORDINAL64(ch->u1.Ordinal) ){
		wprintf (L"Ord:0x%I64X\n", ch->u1.Ordinal & ~IMAGE_ORDINAL_FLAG64 );
	}
	else{
		PIMAGE_IMPORT_BY_NAME p = (PIMAGE_IMPORT_BY_NAME) img->GetPtrFromRVA ( (DWORD)ch->u1.AddressOfData) ;
			
		wprintf (L"0x%X->%S\n",p->Hint, p->Name );
	}
	return true;
}
bool CIMPORTS::PrintThunk32 ( CIMAGE * img, PIMAGE_THUNK_DATA32 ch )
{
	if ( IMAGE_SNAP_BY_ORDINAL32(ch->u1.Ordinal) ){
		wprintf (L"Ord:0x%X\n", ch->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32 );
	}
	else{
		PIMAGE_IMPORT_BY_NAME p = (PIMAGE_IMPORT_BY_NAME)img->GetPtrFromRVA ( ch->u1.AddressOfData);
			
		wprintf (L"0x%X->%S\n",p->Hint, p->Name );
	}
	return true;
}
bool  CIMPORTS::PrintAllThunk64 ( CIMAGE * img, DWORD ch ) 
{
	PIMAGE_THUNK_DATA64 Thunk = (PIMAGE_THUNK_DATA64)img->GetPtrFromRVA ( ch );

	while ( Thunk->u1.AddressOfData ){
		PrintThunk64 (img, Thunk);
		Thunk++;
	}
	return true;
}
bool  CIMPORTS::PrintAllThunk32 ( CIMAGE * img, DWORD ch ) 
{
	PIMAGE_THUNK_DATA32 Thunk = (PIMAGE_THUNK_DATA32)img->GetPtrFromRVA ( ch );

	while ( Thunk->u1.AddressOfData ){
		PrintThunk32 (img, Thunk);
		Thunk++;
	}
	return true;
}
// debug print all imports from bin image. 
bool CIMPORTS::PrintSource ( CIMAGE * img )
{
	PIMAGE_DATA_DIRECTORY dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_IMPORT );

	if ( dd->VirtualAddress != 0 ){
		
 		PIMAGE_IMPORT_DESCRIPTOR ImportDescripor = (PIMAGE_IMPORT_DESCRIPTOR)img->GetPtrFromRVA ( dd->VirtualAddress);

		while ( ImportDescripor->Characteristics ){
		
			char * Name = (char*)img->GetPtrFromRVA (ImportDescripor->Name);

			wprintf(L"Name:%S\n", Name );

			wprintf(L"Time stamp:0x%X\n",ImportDescripor->TimeDateStamp );
			wprintf(L"Index of first forwarder reference:0x%X\n",ImportDescripor->ForwarderChain );

			if ( img->IsImage64() )	{

				wprintf(L"INT:0x%X\n" , ImportDescripor->OriginalFirstThunk );

				if ( ! PrintAllThunk64(img, ImportDescripor->OriginalFirstThunk) )
					return false;

				wprintf(L"IAT:0x%X\n" , ImportDescripor->FirstThunk );

				if ( ! PrintAllThunk64(img, ImportDescripor->FirstThunk) )
					return false;
			}
			else{
				
				wprintf(L"INT:0x%X\n" , ImportDescripor->OriginalFirstThunk );

				if ( ! PrintAllThunk32(img, ImportDescripor->OriginalFirstThunk) )
					return false;

				wprintf(L"IAT:0x%X\n" , ImportDescripor->FirstThunk );

				if ( ! PrintAllThunk32(img, ImportDescripor->FirstThunk) )
					return false;
			}

			ImportDescripor++;
		}
	}
	else{ // IMAGE_DIRECTORY_ENTRY_IMPORT
		wprintf (L"No IMAGE_DIRECTORY_ENTRY_IMPORT \n" );
	}

	//

	dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT );

	if ( dd->VirtualAddress != 0 ){
		throw "not implemented yet"; // 
	}
	else{
		wprintf (L"No IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT  \n" );
	}

	dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT );

	if ( dd->VirtualAddress != 0 ){
		throw " not implemented yet"; //
	}
	else{
		wprintf (L"No IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT  \n" );
	}
	
	return false;
}
void CIMPORTS::IMPORT_DLL::AddThunk64 ( CIMAGE * img, IMPORT_DLL * Dll, PIMAGE_THUNK_DATA64 ch, DWORD Num, bool PrintDetailInfo  )
{
	IMPORT * n = new IMPORT;

	n->Num = Num;
	n->Dll = Dll;

	if ( IMAGE_SNAP_BY_ORDINAL64(ch->u1.Ordinal) ){
		n->Ordinal = ( ch->u1.Ordinal & ~IMAGE_ORDINAL_FLAG64) ; // 
		n->SetIsOrdinal (true);
		n->SetFuncName( NULL );
	}
	else{
		PIMAGE_IMPORT_BY_NAME p = (PIMAGE_IMPORT_BY_NAME)img->GetPtrFromRVA ( (DWORD)ch->u1.AddressOfData ); 
			
		n->SetFuncName ( new string ( p->Name ) );
		n->Ordinal = p->Hint;
		n->SetIsOrdinal ( false );
	}

	if ( PrintDetailInfo )
		printf ( "\n%d %s %s" , n->Num, n->GetFuncName(), n->Dll->DllName->data() );
	

	Funcs.push_back(n);
}
void CIMPORTS::IMPORT_DLL::AddThunk32 ( CIMAGE * img, IMPORT_DLL * Dll, PIMAGE_THUNK_DATA32 ch, DWORD Num, bool PrintDetailInfo  )
{
	IMPORT * n= new IMPORT;
	
	n->Dll = Dll;
	n->Num = Num;

	if ( IMAGE_SNAP_BY_ORDINAL32(ch->u1.Ordinal) ){
		n->Ordinal = ch->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32 ;// 
		n->SetFuncName( NULL );
		n->SetIsOrdinal(true);
		
	}
	else{
		PIMAGE_IMPORT_BY_NAME p = (PIMAGE_IMPORT_BY_NAME)img->GetPtrFromRVA ( ch->u1.AddressOfData );
			
		n->SetFuncName ( new string ( p->Name ) );
		n->Ordinal = p->Hint;
		n->SetIsOrdinal (false);
	}
	
	if ( PrintDetailInfo )
		printf ( "\n%d %s %s" , n->Num, n->GetFuncName(), n->Dll->DllName->data() );

	Funcs.push_back(n);

}
void CIMPORTS::IMPORT_DLL::AddAllThunk64 ( CIMAGE * img, IMPORT_DLL * Dll,  DWORD IntCh, DWORD IatCh, PIMAGE_DATA_DIRECTORY  iat, bool PrintDetailInfo  ) 
{
	PIMAGE_THUNK_DATA64 Thunk=(PIMAGE_THUNK_DATA64)img->GetPtrFromRVA ( IntCh );

	DWORD n = (IatCh - iat->VirtualAddress ) / sizeof(IMAGE_THUNK_DATA64);

	size_t TableSize = iat->Size / sizeof(IMAGE_THUNK_DATA64);

	while ( Thunk->u1.AddressOfData ){
		AddThunk64 ( img, Dll, Thunk, n++, PrintDetailInfo );
		Thunk++;
		if ( n >= TableSize )
			throw "invalid import table";
	}
}

void CIMPORTS::IMPORT_DLL::AddAllThunk32 ( CIMAGE * img, IMPORT_DLL * Dll, DWORD IntCh, DWORD IatCh, PIMAGE_DATA_DIRECTORY  iat, bool PrintDetailInfo  ) 
{
	PIMAGE_THUNK_DATA32 Thunk = (PIMAGE_THUNK_DATA32)img->GetPtrFromRVA ( IntCh );
	DWORD n = (IatCh - iat->VirtualAddress ) / sizeof(IMAGE_THUNK_DATA32);
	
	size_t TableSize = iat->Size / sizeof(IMAGE_THUNK_DATA32);

	while ( Thunk->u1.AddressOfData ){
		AddThunk32 (img, Dll, Thunk, n++, PrintDetailInfo ) ;
		Thunk++;
		if ( n >= TableSize )
			throw "invalid import table";
	}
}

void CIMPORTS::IMPORT_DLL::AddFunc ( string * n, ULONGLONG o )
{
	IMPORT * ni = new IMPORT;
	ni->SetFuncName ( n );
	if ( n == NULL )
		ni->SetIsOrdinal (true);
	else
		ni->SetIsOrdinal (false);
	ni->Ordinal = o;

	Funcs.push_back (ni);
}

void CIMPORTS::IMPORT_DLL::AddFunc ( IMPORT * ni )
{
	Funcs.push_back (ni);
}

CIMPORTS::IMPORT * CIMPORTS::IMPORT_DLL::Find ( IMPORT * f )
{
	for ( auto & i : Funcs ){
		if ( ! f->IsOrdinal() && ! i->IsOrdinal() ){
			if ( ! strcmp( i->GetFuncName(), f->GetFuncName() ) )
				return i;
		}
	}
	return 0;
}

// unpack imports 
// 
bool CIMPORTS::Create ( CIMAGE * img, bool PrintDetailInfo )
{
	printf ( "-------------------Load imports ----------------------\n" );
	PIMAGE_DATA_DIRECTORY iat = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_IAT );

	PIMAGE_DATA_DIRECTORY dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_IMPORT );

	if ( dd->VirtualAddress != 0 ){
		
		printf("Imports:\n");

 		PIMAGE_IMPORT_DESCRIPTOR ImportDescripor = (PIMAGE_IMPORT_DESCRIPTOR)img->GetPtrFromRVA ( dd->VirtualAddress );

		while ( ImportDescripor->Characteristics ){
		
			PCHAR DllName = (PCHAR)img->GetPtrFromRVA (ImportDescripor->Name );

			IMPORT_DLL * ni = new  IMPORT_DLL( DllName, ImportDescripor->TimeDateStamp, ImportDescripor->ForwarderChain );

			if ( img->IsImage64() )	{

				ni->AddAllThunk64 ( img, ni, ImportDescripor->OriginalFirstThunk, ImportDescripor->FirstThunk, iat, PrintDetailInfo ) ;

				// this two tables are equal 
				// then use only first
				// if ( ! AddAllThunk64(img, ImportDescripor->FirstThunk) )
				//	return false;
			}
			else{
				
				ni->AddAllThunk32 ( img, ni, ImportDescripor->OriginalFirstThunk, ImportDescripor->FirstThunk, iat, PrintDetailInfo ) ;

				// this two tables are equal 
				// then use only first
				//if ( ! AddAllThunk32(img, ImportDescripor->FirstThunk) )
				//	return false;
			}

			printf ( "\nDll:%s, funcs:%d\n", ni->DllName->data(), (int)ni->Funcs.size() );

			Imports.push_back(ni);

			ImportDescripor++;
		}

		printf ( "Total:%d\n",(int)Imports.size() );
	}
	else{ // IMAGE_DIRECTORY_ENTRY_IMPORT
		printf ( "No IMAGE_DIRECTORY_ENTRY_IMPORT\n" );
	}

	//

	dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT );

	if ( dd->VirtualAddress != 0 )
		throw "Image uncompatible"; //the my image do not have a bound import. Please implement its youself, in case, if you need this.
	else
		printf ( "No IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT\n" );

	dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT );

	if ( dd->VirtualAddress != 0 )
		throw "Image uncompatible"; //the my image do not have a delay import. Please implement its youself, in case if you need this.
	else
		wprintf (L"No IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT  \n" );
	
	printf("-------------------End Load imports ----------------------\n");
	

	return true;
}


