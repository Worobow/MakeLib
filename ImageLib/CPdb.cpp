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
		
			CPdb.h, CPdb.cpp

		Environment:
			
			User-mode command line

		Comment:

			class for work with PDB 

--*/

#include "stdafx.h"

void CPDB::AddPublicSimbol ( IDiaSymbol *Symbol )
{
	DWORD dwSymTag;
	DWORD dwRVA;
	DWORD dwSeg;
	DWORD dwOff;
	ULONGLONG qwLenght;
	ULONGLONG qwVA;

	BSTR bstrName;
	BSTR bstrUndname;

	wstring Name;
	wstring UndName;
  
	BOOL Code;
	BOOL Function;

	if ( Symbol->get_symTag ( &dwSymTag ) != S_OK) 
		throw "get_symTag";

	Symbol->get_relativeVirtualAddress ( &dwRVA );

	Symbol->get_addressSection ( &dwSeg );
		
	Symbol->get_addressOffset ( &dwOff );
	//Symbol->get_hasLongJump ();
	Symbol->get_length ( &qwLenght );
	Symbol->get_virtualAddress( &qwVA );
	Symbol->get_code(&Code);
	Symbol->get_function(&Function);

	if ( dwSymTag == SymTagThunk ){
		if ( Symbol->get_name ( &bstrName ) == S_OK) {
			Name = bstrName;
			SysFreeString ( bstrName );
		}
		else {
			if ( Symbol->get_targetRelativeVirtualAddress ( &dwRVA ) != S_OK) {
				dwRVA = 0xFFFFFFFF;
			}

			Symbol->get_targetSection ( &dwSeg );
			Symbol->get_targetOffset ( &dwOff );

		//	wprintf(L"target -> [%08X][%04X:%08X]\n", dwRVA, dwSeg, dwOff);
		}
		PubSymbols.insert ( pair<CRELAITIVE_VIRTUAL_ADDRESS, CSYMBOL*> ( CRELAITIVE_VIRTUAL_ADDRESS(dwRVA), new CSYMBOL ( Name,UndName,dwSymTag,dwRVA,dwSeg,dwOff,qwLenght,qwVA,Code,Function ) ) );
	}
	else {
		if ( Symbol->get_name ( &bstrName ) == S_OK ) {
			Name = bstrName;
			SysFreeString(bstrName);

			if ( Symbol->get_undecoratedName ( &bstrUndname ) == S_OK) {
				UndName = bstrUndname;
				SysFreeString(bstrUndname);
			}
		}
		PubSymbols.insert( pair<CRELAITIVE_VIRTUAL_ADDRESS, CSYMBOL*> (  CRELAITIVE_VIRTUAL_ADDRESS(dwRVA), new CSYMBOL(Name,UndName,dwSymTag,dwRVA,dwSeg,dwOff,qwLenght,qwVA,Code,Function) ));
	}
}
void CPDB::LoadSection ( IDiaSectionContrib *SecContrib )
{
	DWORD dwRVA;
	DWORD dwSect;
	DWORD dwOffset;
	DWORD dwLen;
	ULONGLONG qwVA;

	IDiaSymbol *Compiland;

	BSTR bstrName;

 	if ( SecContrib->get_virtualAddress(&qwVA) != S_OK)
		return;
	if ( SecContrib->get_relativeVirtualAddress(&dwRVA) != S_OK)
		return;
	if ( SecContrib->get_addressSection(&dwSect) != S_OK ) 
		return;
	if ( SecContrib->get_addressOffset(&dwOffset)!= S_OK ) 
		return;
	if ( SecContrib->get_length(&dwLen)!= S_OK ) 
		return;
	if ( SecContrib->get_compiland(&Compiland) != S_OK ) 
		return;

	if ( Compiland->get_name(&bstrName) != S_OK ) 
		return;

	Sections.push_back ( new CSECTION (dwSect,dwOffset,dwLen,dwRVA,qwVA, wstring(bstrName)) );
 
	Compiland->Release();

	SysFreeString(bstrName);
		
}

////////////////////////////////////////////////////////////
// Retreive the table that matches the given iid
//
//  A PDB table could store the section contributions, the frame data,
//  the injected sources
//
HRESULT CPDB::GetTable ( IDiaSession *pSession, REFIID iid, void **ppUnk )
{
	IDiaEnumTables *pEnumTables;

	if (FAILED(pSession->getEnumTables(&pEnumTables))) {
		//wprintf(L"ERROR - GetTable() getEnumTables\n");
		return E_FAIL;
	}

	IDiaTable *pTable;
	ULONG celt = 0;

	while (SUCCEEDED(pEnumTables->Next(1, &pTable, &celt)) && (celt == 1)) {
		// There's only one table that matches the given IID

		if ( SUCCEEDED ( pTable->QueryInterface ( iid, (void **) ppUnk))) {
			pTable->Release();
			pEnumTables->Release();
			return S_OK;
		}
		pTable->Release();
	}

	pEnumTables->Release();

	return E_FAIL;
}
void CPDB::LoadAllSections ( void )
{
	IDiaEnumSectionContribs *EnumSecContribs;

	if ( FAILED ( GetTable ( Session, __uuidof(IDiaEnumSectionContribs), (void **) &EnumSecContribs))) 
		throw "GetTable IDiaEnumSectionContribs failed";
		

	//wprintf(L"    RVA        Address       Size    Module\n");

	IDiaSectionContrib *SecContrib;
	ULONG celt = 0;

	while ( SUCCEEDED ( EnumSecContribs->Next ( 1, &SecContrib, &celt ) ) && ( celt == 1 ) ){
		LoadSection ( SecContrib );

		SecContrib->Release();
	}

	EnumSecContribs->Release();

}
void CPDB::LoadAllModules ( void )
{
	// Retrieve all the compiland symbols

	IDiaEnumSymbols * EnumSymbols;

	if ( FAILED ( Global->findChildren ( SymTagCompiland, NULL, nsNone, & EnumSymbols ))) 
		throw "findChildren failed";

	IDiaSymbol * Compiland;

	ULONG celt = 0;
	//ULONG iMod = 1;

	while ( SUCCEEDED ( EnumSymbols->Next ( 1, &Compiland, &celt ) ) && (celt == 1)) {
		BSTR bstrName;

		if ( Compiland->get_name ( &bstrName ) != S_OK )
			throw "ERROR - Failed to get the compiland's name";

		//Compiland->Release();
		//EnumSymbols->Release();

		// Deallocate the string allocated previously by the call to get_name

		Modules.push_back ( new wstring(bstrName));

		SysFreeString ( bstrName );

		Compiland->Release();
	}
	EnumSymbols->Release();
}
void CPDB::LoadAllPublicSymbols ( void )
{
	IDiaEnumSymbols *EnumSymbols;

	if ( FAILED ( Global->findChildren ( SymTagPublicSymbol, NULL, nsNone, &EnumSymbols ) ) ) 
		throw "enum SymTagPublicSymbol failed";
	  
	IDiaSymbol *Symbol;
	ULONG celt = 0;

	while ( SUCCEEDED ( EnumSymbols->Next ( 1, &Symbol, &celt) ) && ( celt == 1 )) {
		AddPublicSimbol ( Symbol );
			
		Symbol->Release();
	}

	EnumSymbols->Release();
}
void CPDB::Load ( const wstring & file )
{
	HRESULT hr = Source->loadDataFromPdb ( file.data() );

	if (FAILED(hr)) 
		throw "loadDataFromPdb failed";

      
    // Open a session for querying symbols

	hr = Source->openSession ( &Session );

	if ( FAILED(hr) ) 
		throw "openSession failed";

// Retrieve a reference to the global scope

	hr = Session->get_globalScope ( &Global );

	if ( FAILED(hr) ) 
		throw "get_globalScope failed";


// Set Machine type for getting correct register names
		
	DWORD dwMachType;

	hr = Global->get_machineType ( &dwMachType) ;

	if ( FAILED(hr) ) 
		throw "get_machineType failed";

	switch (dwMachType) {
		case IMAGE_FILE_MACHINE_I386:		MachineType = CV_CFL_80386; break;
		case IMAGE_FILE_MACHINE_IA64:		MachineType = CV_CFL_IA64;	break;
		case IMAGE_FILE_MACHINE_AMD64:		MachineType = CV_CFL_AMD64; break;
	}
}