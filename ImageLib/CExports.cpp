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
		
			CExports.h, CExports.cpp

		Environment:

			User-mode command line

		Comment:

			class for the implementation of functional work exports

--*/

#include "stdafx.h"


bool CEXPORTS::Pack ( CSECTION * sec, CIMAGE * img, bool PrintDebugDetails )
{
	if ( Exports.size() == 0 )
		return true;

	// new datas - exports
	BLOCK<BYTE> * ExportDir = new BLOCK<BYTE>("Exports");


	// cacl size
	// sum of all name lens
	// size of exports 
	// size of names
	// size of ords
	size_t NeedSize = sizeof(IMAGE_EXPORT_DIRECTORY); // header

	NeedSize +=strlen(Name) + 1;
	
	NeedSize += Exports.size() * sizeof(DWORD); // EAT 

	size_t EntSize=0;

	for ( const auto &e : Exports ){
		if ( e.Name != NULL ){
			NeedSize += strlen(e.Name)+1;
			EntSize++;
		}
	}

	NeedSize += sizeof(WORD) * EntSize;// ORD

	NeedSize += sizeof(DWORD)*EntSize;// ENT

	ExportDir->Alloc (NeedSize);

	PIMAGE_EXPORT_DIRECTORY Ret = (PIMAGE_EXPORT_DIRECTORY)ExportDir->GetBuffer();  

	// only for debug

	char * MaxPtr = (char*)Ret + NeedSize;

	Ret->Base=1;
	Ret->Characteristics=0;
	Ret->MajorVersion=0;
    Ret->MinorVersion=0;
    Ret->TimeDateStamp=0;
    Ret->NumberOfFunctions = (DWORD)Exports.size();
    Ret->NumberOfNames = (DWORD)EntSize;

	CRELAITIVE_VIRTUAL_ADDRESS * FuncsRVA = (CRELAITIVE_VIRTUAL_ADDRESS*) ((PBYTE)Ret + sizeof(IMAGE_EXPORT_DIRECTORY));
	CRELAITIVE_VIRTUAL_ADDRESS * NamesRVA = FuncsRVA + Ret->NumberOfFunctions;
	CRELAITIVE_VIRTUAL_ADDRESS * OrdsRVA  = (CRELAITIVE_VIRTUAL_ADDRESS*)(NamesRVA + Ret->NumberOfNames);
	char  *  NameRVA = (char*)(OrdsRVA +  Ret->NumberOfNames);

	size_t l = strlen ( Name ) + 1;
	memcpy ( NameRVA, Name, l );

	ExportDir->WriteRVA ( ExportDir->CreateNewInternalRelocFromPtr ( &Ret->Name,"PIMAGE_EXPORT_DIRECTORY.Name",										img->GetImageBase() ), CRELAITIVE_VIRTUAL_ADDRESS ( NameRVA - (char*)Ret ) );// set

	//Ret->Name = VA + ( NameRVA - (char*)Ret );
	
	char * Names = NameRVA + l;

	ExportDir->WriteRVA ( ExportDir->CreateNewInternalRelocFromPtr ( &Ret->AddressOfNames, "PIMAGE_EXPORT_DIRECTORY.AddressOfNames",				img->GetImageBase() ), CRELAITIVE_VIRTUAL_ADDRESS ( (char*)NamesRVA - (char*)Ret ) );//set 
	ExportDir->WriteRVA ( ExportDir->CreateNewInternalRelocFromPtr ( &Ret->AddressOfFunctions, "PIMAGE_EXPORT_DIRECTORY.AddressOfFunctions",		img->GetImageBase() ), CRELAITIVE_VIRTUAL_ADDRESS ( (char*)FuncsRVA - (char*)Ret ) );//set
	ExportDir->WriteRVA ( ExportDir->CreateNewInternalRelocFromPtr ( &Ret->AddressOfNameOrdinals, "PIMAGE_EXPORT_DIRECTORY.AddressOfNameOrdinals",	img->GetImageBase() ), CRELAITIVE_VIRTUAL_ADDRESS ( (char*)OrdsRVA  - (char*)Ret ) );//set

	char * p = Names;

	size_t j=0;

	for ( size_t i=0; i < Exports.size(); i++ ){
		FuncsRVA[i] = Exports[i].VirtualAddress;
		if ( Exports[i].Name != NULL ){
			l = strlen ( Exports[i].Name) + 1;
			memcpy ( p, Exports[i].Name, l );

			ExportDir->WriteRVA ( ExportDir->CreateNewInternalRelocFromPtr ( &NamesRVA[j], "Names[*]", img->GetImageBase() ), CRELAITIVE_VIRTUAL_ADDRESS (p - (char*)Ret) );//set

			OrdsRVA[j] = (WORD)i;
			j++;
			if ( p > MaxPtr )
				throw;
			p +=l;
		}
	}

	
	ExportDir->WriteRVA ( new  CINTERNAL_RVA_RELOC ( 0, img->GetImageBase(), &img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT)->VirtualAddress,  "IMAGE_DIRECTORY_ENTRY_EXPORT" ),  ExportDir->GetRelativeVirtialAddress() );
	img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT)->Size = ExportDir->GetByteSize(); //SecRels.Record.SizeOfRawData;

	// put to rdata
	sec->AddBlock ( ExportDir, PrintDebugDetails );

	return true;
	
}

// 
bool CEXPORTS::Create ( CIMAGE * img, bool PrintDetailInfo )
{

	printf("-------------------Load exports ----------------------\n");

	PIMAGE_DATA_DIRECTORY dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_EXPORT );

	if ( dd->VirtualAddress == 0 ){
		printf ("Exports none.\n");
	}
	else {

		PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY)img->GetPtrFromRVA ( dd->VirtualAddress ); 

		PCHAR DllName = (PCHAR) img->GetPtrFromRVA (ExportDir->Name );
	
		//wprintf (L"Name:%S\n",DllName);

		Name = DllName;
	
		PDWORD Eat = (PDWORD)img->GetPtrFromRVA (ExportDir->AddressOfFunctions );

		for ( size_t i =0; i<ExportDir->NumberOfFunctions; i++ ){
			EXPORT n;
			n.VirtualAddress = Eat[i];
			n.Name=0;
			Exports.push_back(n);
		}

		PDWORD  Ent = (PDWORD)img->GetPtrFromRVA (ExportDir->AddressOfNames );

		PWORD  Ords  = (PWORD)img->GetPtrFromRVA (ExportDir->AddressOfNameOrdinals );


		for ( size_t i =0; i<ExportDir->NumberOfNames; i++ ){
		
			size_t idx = Ords[i];

			PCHAR n = (PCHAR)img->GetPtrFromRVA (Ent[i]); 

			Exports[idx].Name = n;

		}
	}
	printf("-------------------End load exports ----------------------\n");

	return true;
}