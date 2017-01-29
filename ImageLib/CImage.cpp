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
		
		CImage.cpp, h

	Environment:
	
		User-mode, c++
	
	Comment:

--*/

#include "stdafx.h"
 
void CIMAGE::PrintHeader ( void )
{
	wprintf( L"Header.\n------\n");
	wprintf( L"Offset:0x%08X\n",0);
	wprintf ( L"Signature:0x%X\n", _Header32->Signature );
	
	wprintf ( L"Machine:0x%X\n", _Header32->FileHeader.Machine );

	wprintf ( L"NumberOfSections:0x%X\n", _Header32->FileHeader.NumberOfSections );
	wprintf ( L"TimeDateStamp:0x%X\n", _Header32->FileHeader.TimeDateStamp );
	wprintf ( L"PointerToSymbolTable:0x%X\n", _Header32->FileHeader.PointerToSymbolTable );
	wprintf ( L"NumberOfSymbols:0x%X\n", _Header32->FileHeader.NumberOfSymbols );
	wprintf ( L"SizeOfOptionalHeader:0x%X\n", _Header32->FileHeader.SizeOfOptionalHeader );
	wprintf ( L"Characteristics:0x%X\n", _Header32->FileHeader.Characteristics );
}
void PrintOptionalHeader32 ( PIMAGE_NT_HEADERS32 hdr32 )
{
	wprintf ( L"Magic:0x%X\n", hdr32->OptionalHeader.Magic );
	wprintf ( L"LinkerVersion:0x%X,0x%X\n", hdr32->OptionalHeader.MajorLinkerVersion,hdr32->OptionalHeader.MinorLinkerVersion );

	wprintf ( L"SizeOfCode:0x%X\n", hdr32->OptionalHeader.SizeOfCode );
	wprintf ( L"SizeOfInitializedData:0x%X\n", hdr32->OptionalHeader.SizeOfInitializedData );
	wprintf ( L"SizeOfUninitializedData:0x%X\n", hdr32->OptionalHeader.SizeOfUninitializedData );
	wprintf ( L"AddressOfEntryPoint:0x%X\n", hdr32->OptionalHeader.AddressOfEntryPoint );
	wprintf ( L"BaseOfCode:0x%X\n", hdr32->OptionalHeader.BaseOfCode );
	wprintf ( L"BaseOfData:0x%X\n", hdr32->OptionalHeader.BaseOfData );
	wprintf ( L"ImageBase:0x%X\n", hdr32->OptionalHeader.ImageBase );
	wprintf ( L"SectionAlignment:0x%X\n", hdr32->OptionalHeader.SectionAlignment );
	wprintf ( L"FileAlignment:0x%X\n", hdr32->OptionalHeader.FileAlignment );
	wprintf ( L"OperatingSystemVersion:0x%X,0x%X\n", hdr32->OptionalHeader.MajorOperatingSystemVersion, hdr32->OptionalHeader.MinorOperatingSystemVersion);
	wprintf ( L"ImageVersion:0x%X,0x%X\n", hdr32->OptionalHeader.MajorImageVersion,hdr32->OptionalHeader. MinorImageVersion);
	wprintf ( L"SubsystemVersion:0x%X,0x%X\n", hdr32->OptionalHeader.MajorSubsystemVersion, hdr32->OptionalHeader.MinorSubsystemVersion);
	wprintf ( L"Win32VersionValue:0x%X\n", hdr32->OptionalHeader.Win32VersionValue );
	wprintf ( L"SizeOfImage:0x%X\n", hdr32->OptionalHeader.SizeOfImage );
	wprintf ( L"SizeOfHeaders:0x%X\n", hdr32->OptionalHeader.SizeOfHeaders );
	wprintf ( L"CheckSum:0x%X\n", hdr32->OptionalHeader.CheckSum );
	wprintf ( L"Subsystem:0x%X\n", hdr32->OptionalHeader.Subsystem );
	wprintf ( L"DllCharacteristics:0x%X\n", hdr32->OptionalHeader.DllCharacteristics );
	wprintf ( L"SizeOfStackReserve:0x%X\n", hdr32->OptionalHeader.SizeOfStackReserve );
	wprintf ( L"SizeOfStackCommit:0x%X\n", hdr32->OptionalHeader.SizeOfStackCommit );
	wprintf ( L"SizeOfHeapReserve:0x%X\n", hdr32->OptionalHeader.SizeOfHeapReserve );
	wprintf ( L"SizeOfHeapCommit:0x%X\n", hdr32->OptionalHeader.SizeOfHeapCommit );
	wprintf ( L"LoaderFlags:0x%X\n", hdr32->OptionalHeader.LoaderFlags );
	wprintf ( L"NumberOfRvaAndSizes:0x%X\n", hdr32->OptionalHeader.NumberOfRvaAndSizes );

}
void PrintOptionalHeader64 ( PIMAGE_NT_HEADERS64 hdr64 )
{
	wprintf ( L"Magic:0x%X\n", hdr64->OptionalHeader.Magic );
	wprintf ( L"LinkerVersion:0x%X,0x%X\n", hdr64->OptionalHeader.MajorLinkerVersion,hdr64->OptionalHeader.MinorLinkerVersion );

	wprintf ( L"SizeOfCode:0x%X\n", hdr64->OptionalHeader.SizeOfCode );
	wprintf ( L"SizeOfInitializedData:0x%X\n", hdr64->OptionalHeader.SizeOfInitializedData );
	wprintf ( L"SizeOfUninitializedData:0x%X\n", hdr64->OptionalHeader.SizeOfUninitializedData );
	wprintf ( L"AddressOfEntryPoint:0x%X\n", hdr64->OptionalHeader.AddressOfEntryPoint );
	wprintf ( L"BaseOfCode:0x%X\n", hdr64->OptionalHeader.BaseOfCode );
	wprintf ( L"ImageBase:0x%I64X\n", hdr64->OptionalHeader.ImageBase );
	wprintf ( L"SectionAlignment:0x%X\n", hdr64->OptionalHeader.SectionAlignment );
	wprintf ( L"FileAlignment:0x%X\n", hdr64->OptionalHeader.FileAlignment );
	wprintf ( L"OperatingSystemVersion:0x%X,0x%X\n", hdr64->OptionalHeader.MajorOperatingSystemVersion, hdr64->OptionalHeader.MinorOperatingSystemVersion);
	wprintf ( L"ImageVersion:0x%X,0x%X\n", hdr64->OptionalHeader.MajorImageVersion,hdr64->OptionalHeader. MinorImageVersion);
	wprintf ( L"SubsystemVersion:0x%X,0x%X\n", hdr64->OptionalHeader.MajorSubsystemVersion, hdr64->OptionalHeader.MinorSubsystemVersion);
	wprintf ( L"Win32VersionValue:0x%X\n", hdr64->OptionalHeader.Win32VersionValue );
	wprintf ( L"SizeOfImage:0x%X\n", hdr64->OptionalHeader.SizeOfImage );
	wprintf ( L"SizeOfHeaders:0x%X\n", hdr64->OptionalHeader.SizeOfHeaders );
	wprintf ( L"CheckSum:0x%X\n", hdr64->OptionalHeader.CheckSum );
	wprintf ( L"Subsystem:0x%X\n", hdr64->OptionalHeader.Subsystem );
	wprintf ( L"DllCharacteristics:0x%X\n", hdr64->OptionalHeader.DllCharacteristics );
	wprintf ( L"SizeOfStackReserve:0x%I64X\n", hdr64->OptionalHeader.SizeOfStackReserve );
	wprintf ( L"SizeOfStackCommit:0x%I64X\n", hdr64->OptionalHeader.SizeOfStackCommit );
	wprintf ( L"SizeOfHeapReserve:0x%I64X\n", hdr64->OptionalHeader.SizeOfHeapReserve );
	wprintf ( L"SizeOfHeapCommit:0x%I64X\n", hdr64->OptionalHeader.SizeOfHeapCommit );
	wprintf ( L"LoaderFlags:0x%X\n", hdr64->OptionalHeader.LoaderFlags );
	wprintf ( L"NumberOfRvaAndSizes:0x%X\n", hdr64->OptionalHeader.NumberOfRvaAndSizes );
 
}
void CIMAGE:: PrintOptionalHeader()
{
	WCHAR * DDName[]=
		{
			L"Export table address",
			L"Import table address",
			L"Resource table address",
			L"Exception table address",
			L"Certificate table address",
			L"Base relocation table address",
			L"Debugging information starting address",
			L"Architecture-specific data address",
			L"Global pointer register relative virtual address ",
			L"Thread local storage (TLS) table address",
			L"Load configuration table address",
			L"Bound import table address",
			L"Import address table address",
			L"Delay import descriptor address",
			L"The CLR header address",
			L"Reserved"
		};

	wprintf( L"OptionalHeader Header.\n-----------------------\n");
	wprintf( L"Offset:0x%08X\n", GetFileOffsetFromPtr ( (PBYTE) &_Header32->OptionalHeader) );

	PIMAGE_DATA_DIRECTORY dd;
	DWORD ddSize;

	if ( ! _IsHeader64 ){
		PrintOptionalHeader32 ( _Header32 );
		dd = &_Header32->OptionalHeader.DataDirectory[0];
		ddSize = _Header32->OptionalHeader.NumberOfRvaAndSizes;
	}
	else {
		PrintOptionalHeader64 ( _Header64 );
		dd = &_Header64->OptionalHeader.DataDirectory[0];
		ddSize = _Header64->OptionalHeader.NumberOfRvaAndSizes;
	}
	wprintf ( L"Data directorys.\n");
	wprintf ( L"-------------------\n");
	wprintf ( L"#   Size       VA          Desc                \n");
	wprintf ( L"--- ---------- --------    --------------------\n");

	for ( size_t i=0;i<ddSize;i++ ){
		wprintf(L"%03d %010d 0x%08X -- %s\n", (int)i, dd[i].Size, dd[i].VirtualAddress , DDName[i]);
	}
}
void CIMAGE::PrintSections()
{
	
	PIMAGE_SECTION_HEADER pSectionHdr = IMAGE_FIRST_SECTION ( _Header32 );
 
	wprintf(L"Sections\n");
	
	wprintf(L"#   Offset     Name     VirtAddr   PhAddr     SizeRDta   PtrRDta    PtrRelo    ReloCnt    PtrLNums   LNumsCnt\n");
	wprintf(L"--- ---------- -------- ---------- ---------- ---------- ---------- ---------- ---------- ---------- ---------- \n");
 	for ( size_t i=0; i<_Header32->FileHeader.NumberOfSections; i++ ){
		DWORD RecOffset = GetFileOffsetFromPtr ( (PBYTE) pSectionHdr);  
		wprintf ( L"%03d 0x%08X %8S 0x%08x 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X\n", 
					(int)i,
					RecOffset,
					pSectionHdr->Name, 
					pSectionHdr->VirtualAddress, 
					pSectionHdr->Misc.PhysicalAddress, 
					pSectionHdr->SizeOfRawData,
					pSectionHdr->PointerToRawData,
					pSectionHdr->PointerToRelocations,
					pSectionHdr->NumberOfRelocations,
					pSectionHdr->PointerToLinenumbers,
					pSectionHdr->NumberOfLinenumbers,
					pSectionHdr->Characteristics
					);
		
	
 		pSectionHdr++;
 	}

	DWORD off = GetFileOffsetFromPtr((PBYTE)pSectionHdr);

	wprintf(L"Section table left records:%d\n", (int) ((GetMinPointerToRawData() - off)/sizeof(IMAGE_SECTION_HEADER)) );
	
}
void CIMAGE::PrintExportDirectory ()
{
	DWORD ExportTableFileOffset;
	PIMAGE_DATA_DIRECTORY dd=  _HeaderGetDataDirectory (IMAGE_DIRECTORY_ENTRY_EXPORT );
	
	wprintf ( L"Exports:\n" );
	wprintf ( L"----------\n" );

	if ( dd->VirtualAddress == 0 ){
			wprintf ( L"No exports.\n" );
			return;
	}
	if ( ! GetFileOffsetFromRVA ( dd->VirtualAddress, &ExportTableFileOffset ) )
		return;

	PIMAGE_EXPORT_DIRECTORY ExportTable = (PIMAGE_EXPORT_DIRECTORY) GetPtrFromFileOffset (ExportTableFileOffset); 
	size_t ExportTableSize = dd->Size/sizeof(IMAGE_EXPORT_DIRECTORY);

	

	wprintf ( L"#   Chars    Time     Ver       Name     Base     FuncsCnt NamesCnt EAT      ENT      EOT\n" );
	for (size_t i=0; i<ExportTableSize;i++)
		wprintf(L"%03d %08X %08X %04X %04X %08X %08X %08X %08X %08X %08X %08X\n",
			(int)i,
			ExportTable[i].Characteristics,
			ExportTable[i].TimeDateStamp,
			ExportTable[i].MajorVersion,
			ExportTable[i].MinorVersion,
			ExportTable[i].Name,
			ExportTable[i].Base,
			ExportTable[i].NumberOfFunctions,
			ExportTable[i].NumberOfNames,
			ExportTable[i].AddressOfFunctions,
			ExportTable[i].AddressOfNames,
			ExportTable[i].AddressOfNameOrdinals
			);
}
void CIMAGE::PrintImportDirectory()
{
	Imports.PrintSource(this);
}
void CIMAGE::PrintBaseRelocDirectory ()
{
	PIMAGE_DATA_DIRECTORY dd = _HeaderGetDataDirectory ( IMAGE_DIRECTORY_ENTRY_BASERELOC );

	if ( dd->VirtualAddress == 0 ){
			wprintf ( L"No relocs.\n" );
			return;
	}

	DWORD RelocTableFileOffset;

	if ( ! GetFileOffsetFromRVA ( dd->VirtualAddress, &RelocTableFileOffset ) )
		return;

	if ( ! GetFileOffsetFromRVA ( dd->VirtualAddress, &RelocTableFileOffset ) )
		return;


	PIMAGE_BASE_RELOCATION  RelocTable= (PIMAGE_BASE_RELOCATION) GetPtrFromFileOffset (RelocTableFileOffset);
	size_t RelocTableSize = dd->Size;

	wprintf (L"TABLE OFFSET:%08X SIZE:%08X\n",RelocTableFileOffset, (int)RelocTableSize);
	
	size_t n=0;

	/*
#define IMAGE_REL_BASED_ABSOLUTE              0
#define IMAGE_REL_BASED_HIGH                  1
#define IMAGE_REL_BASED_LOW                   2
#define IMAGE_REL_BASED_HIGHLOW               3
#define IMAGE_REL_BASED_HIGHADJ               4
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_5    5
	#define IMAGE_REL_BASED_MIPS_JMPADDR          5 -
#define IMAGE_REL_BASED_ARM_MOV32             5
#define IMAGE_REL_BASED_RESERVED              6
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_7    7
	#define IMAGE_REL_BASED_THUMB_MOV32           7 -
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_8    8
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_9    9
#define IMAGE_REL_BASED_IA64_IMM64            9
	#define IMAGE_REL_BASED_MIPS_JMPADDR16        9 -
#define IMAGE_REL_BASED_DIR64                 10
	*/
	
	while ( RelocTable->VirtualAddress != 0 ) {
		size_t RelocsCount = ( RelocTable->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION) )/ sizeof(WORD);
		wprintf(L"%03d VA:%08X Size:%08X Cnt:%x\n", (int)n, RelocTable->VirtualAddress,RelocTable->SizeOfBlock,(int)RelocsCount);
		CRELOCS::PACKEDRELOC * Reloc = (CRELOCS::PACKEDRELOC*)((char*)RelocTable + sizeof(IMAGE_BASE_RELOCATION) );
		for (size_t i=0;i<RelocsCount;i++){
			DWORD va = Reloc[i].Offset + RelocTable->VirtualAddress;
			PDWORD val = (PDWORD)GetPtrFromRVA(va);
			wprintf(L"%1X %08X %08x\n", Reloc[i].RelocType, va, *val );
		}
		RelocTable = (PIMAGE_BASE_RELOCATION)((char*)RelocTable + RelocTable->SizeOfBlock);
		n++;
	};
}
bool IsHeader64 ( PIMAGE_NT_HEADERS h )
{
	if ( h->FileHeader.Machine  == IMAGE_FILE_MACHINE_AMD64	)
		return true;// 

	return false;// successfull
}
// return true if image is OK
bool CIMAGE::CheckImageType () 
{
	PIMAGE_NT_HEADERS h = ImageNtHeader ( FileImage );

	_Header32 =  (PIMAGE_NT_HEADERS32)h;
	_Header64 =  (PIMAGE_NT_HEADERS64)h;

	if ( h->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {
		_IsHeader64=false;
		return true;// successfull
	}
	else if ( h->FileHeader.Machine  == IMAGE_FILE_MACHINE_AMD64	){
		_IsHeader64=true;
		return true;// successfull
	}
	return false;
}
DWORD CIMAGE::Read ( WCHAR * ImageName )
{
	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	DWORD Ret = ERROR_SUCCESS;

	do {
		FileHandle =  CreateFileW ( ImageName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( FileHandle == INVALID_HANDLE_VALUE ) 
			break;

		LARGE_INTEGER fs;

		if ( ! GetFileSizeEx ( FileHandle, &fs) )
			break;
	
		if ( FileImage != NULL )
			LocalFree( FileImage );

		AllocateFileImageSize =  fs.LowPart;

		FileImage = (PBYTE)LocalAlloc ( LMEM_FIXED, AllocateFileImageSize );
		if ( FileImage == NULL )
			break;
		DWORD br=0;
		if ( ! ReadFile ( FileHandle, FileImage, (DWORD)AllocateFileImageSize, &br, 0 ) )
			break;
		
	//	_PtrMax=(char*)FileImage+AllocateFileImageSize;
	//	_PtrTail=_PtrMax;

	}while (0);

	Ret = GetLastError();

	if ( FileHandle != INVALID_HANDLE_VALUE )
		CloseHandle ( FileHandle ); 

	if ( Ret == ERROR_SUCCESS ){

		if ( CheckImageType () )
			return Ret;

		Ret = ERROR_IMAGE_MACHINE_TYPE_MISMATCH ;// error
	}

	if ( FileImage != NULL )
		LocalFree( FileImage );
		
	FileImage = NULL;
	
	return Ret;
}
PIMAGE_DATA_DIRECTORY CIMAGE::_HeaderGetDataDirectory ( size_t Dir )
{
	PIMAGE_DATA_DIRECTORY Ret;

	if ( _IsHeader64 )
		Ret = _Header64->OptionalHeader.DataDirectory;
	else
		Ret = _Header32->OptionalHeader.DataDirectory;

	return &Ret[Dir];

}
PIMAGE_DATA_DIRECTORY CIMAGE::GetDataDirectory ( size_t Dir )
{
	return &_Directorys[Dir];
}
DWORD CIMAGE::GetMinPointerToRawData ( void )
{

	if ( ! ReadOnly )
		throw "Function only for loaded bin images.";

	DWORD MinPointerToRawData = UINT_MAX;

	PIMAGE_SECTION_HEADER pSectionHdr = IMAGE_FIRST_SECTION ( _Header32 );

 	for ( size_t i=0; i<_Header32->FileHeader.NumberOfSections; i++ ){
		if ( MinPointerToRawData > pSectionHdr->PointerToRawData )
			MinPointerToRawData = pSectionHdr->PointerToRawData;
 		pSectionHdr++;
 	}
	return MinPointerToRawData;
}
PBYTE CIMAGE::GetPtrFromFileOffset ( DWORD of )
{
	if ( ! ReadOnly )
		throw "Function only for loaded bin images.";

	return FileImage + of;
}
DWORD CIMAGE::GetFileOffsetFromPtr ( PBYTE p )
{
	if ( ! ReadOnly )
		throw "Function only for loaded bin images.";
	return (DWORD)(p - FileImage);
}
CVIRTUAL_ADDRESS CIMAGE::GetMinVA ( )
{
	return GetVaFromRVA ( GetMinRVA() );
}
CVIRTUAL_ADDRESS CIMAGE::GetMaxVA ( )
{
	return GetVaFromRVA ( GetMaxRVA() );
}
CRELAITIVE_VIRTUAL_ADDRESS CIMAGE::GetMinRVA ( void )
{
	CRELAITIVE_VIRTUAL_ADDRESS MinRVA = CRELAITIVE_VIRTUAL_ADDRESS((DWORD)UINT_MAX);

	for ( auto s: Sections){
		if ( MinRVA > CRELAITIVE_VIRTUAL_ADDRESS(s->Record.VirtualAddress) )
			MinRVA = CRELAITIVE_VIRTUAL_ADDRESS(s->Record.VirtualAddress);
	}
	return MinRVA;
}
CRELAITIVE_VIRTUAL_ADDRESS CIMAGE::GetMaxRVA ( void )
{
	CRELAITIVE_VIRTUAL_ADDRESS MaxRVA = CRELAITIVE_VIRTUAL_ADDRESS(0);

	if ( ReadOnly ) {// use header if image was not changed BUT we can use the 'Sections' as editable image, but why
		// 
		PIMAGE_SECTION_HEADER pSectionHdr = IMAGE_FIRST_SECTION ( _Header32 );

 		for ( size_t i=0; i<_Header32->FileHeader.NumberOfSections; i++ ){
			CRELAITIVE_VIRTUAL_ADDRESS fo = pSectionHdr[i].VirtualAddress + pSectionHdr[i].SizeOfRawData;
			if ( MaxRVA < fo )
				MaxRVA = fo;
 		}
	}
	else{

		for ( auto s: Sections){
			CRELAITIVE_VIRTUAL_ADDRESS fo = s->Record.VirtualAddress + s->Record.Misc.VirtualSize;  
			if ( MaxRVA < fo )
				MaxRVA = fo;
		}
	}

	return  MaxRVA ;
}
void CIMAGE::AddSection ( CSECTION * n, bool PrintDebugDetails )
{
	if  ( ReadOnly )
		throw "Try edit readonly image";

	printf("------------Add section <<%8s>> to image <<%s>>(0x%p)------------\n", n->Record.Name, Description.data(), this );

	n->MyImage =  this;
	
	Sections.push_back ( n );

	n->SetBase ( GetImageBase() );

	if ( n->Record.VirtualAddress == 0 ){
	
		CRELAITIVE_VIRTUAL_ADDRESS rva = SectionAlignment ( CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( GetMaxRVA() ) ) ;

		n->ChangeVirtualAddress ( rva, PrintDebugDetails );
	}

	if ( n->Record.SizeOfRawData == 0 )
		n->Record.SizeOfRawData = CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( FileAlignment ( n->Record.Misc.VirtualSize ) );

}

DWORD CIMAGE::Write ( WCHAR * PathName )
{

	if ( ReadOnly )
		return ERROR_ACCESS_DENIED;

	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	DWORD Ret = ERROR_SUCCESS;

	do {
		FileHandle =  CreateFileW ( PathName, GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( FileHandle == INVALID_HANDLE_VALUE ) 
			break;

		DWORD SizeOfImage;

		if ( _IsHeader64 )
			SizeOfImage=_Header64->OptionalHeader.SizeOfImage;
		else
			SizeOfImage=_Header32->OptionalHeader.SizeOfImage;

		DWORD bw=0;
		if ( ! WriteFile ( FileHandle, FileImage, SizeOfImage, &bw, 0 ) )
			break;
	} while (0);
	
	Ret = GetLastError();
	
	if ( FileHandle != INVALID_HANDLE_VALUE )
		CloseHandle ( FileHandle ); 

	return Ret;
}
bool CIMAGE::GetFileOffsetFromRVA ( CRELAITIVE_VIRTUAL_ADDRESS rva, DWORD * RetPtr )
{
	if ( ! ReadOnly ){ // we work ONLY with (!) loaded images
		throw "Try use not ready image";
	}

	PIMAGE_SECTION_HEADER pSectionHdr = IMAGE_FIRST_SECTION ( _Header32 );

 	for ( size_t i=0; i<_Header32->FileHeader.NumberOfSections; i++ ){

		CRELAITIVE_VIRTUAL_ADDRESS Start = pSectionHdr->VirtualAddress;
		CRELAITIVE_VIRTUAL_ADDRESS End   = pSectionHdr->VirtualAddress + pSectionHdr->SizeOfRawData;

		if ( rva <= End && rva >= Start ){
			*RetPtr = CRELAITIVE_VIRTUAL_ADDRESS::Get32 (  ( rva - CRELAITIVE_VIRTUAL_ADDRESS(pSectionHdr->VirtualAddress) ) + CRELAITIVE_VIRTUAL_ADDRESS(pSectionHdr->PointerToRawData ) );
			return true;
		}
 		pSectionHdr++;
 	}
	return false;
}
// Open exist file 
// create all sections array
// create exports array
// create imports array
//

bool CIMAGE::OpenExist ( WCHAR * ImageName, PDWORD Err, string & descr, bool PrintDetailInfo )
{
	*Err = Read ( ImageName );
	
	if ( *Err != ERROR_SUCCESS )
		return false;

	ReadOnly = true;

	if ( _IsHeader64 ){
		_Directorys =  _Header64->OptionalHeader.DataDirectory;
		_SectionAlignment = _Header64->OptionalHeader.SectionAlignment;
		_FileAlignment = _Header64->OptionalHeader.FileAlignment;
	}
	else{
		_Directorys = _Header32->OptionalHeader.DataDirectory;
		_SectionAlignment = _Header32->OptionalHeader.SectionAlignment;
		_FileAlignment = _Header32->OptionalHeader.FileAlignment;
	}

	// for all section create CSECTION and put it to the 'Sections' array 
	// section place don't depends from 64 / 32 bits
	//
	PIMAGE_SECTION_HEADER pSectionHdr = IMAGE_FIRST_SECTION ( _Header32 );

	printf("Image %s, Sections.\n",descr.data());

 	for ( size_t i=0; i < _Header32->FileHeader.NumberOfSections; i++ ){

		BLOCK<BYTE> * blk = new BLOCK<BYTE>("Data block from exist image");

		if (pSectionHdr[i].SizeOfRawData > 0) {
			// 
			DWORD DataSize;

			if (pSectionHdr[i].SizeOfRawData > pSectionHdr[i].Misc.VirtualSize)
				DataSize = pSectionHdr[i].Misc.VirtualSize;
			else
				DataSize = pSectionHdr[i].SizeOfRawData;

			blk->SetBuffer(GetPtrFromFileOffset(pSectionHdr[i].PointerToRawData), DataSize, pSectionHdr[i].VirtualAddress);
		}
		else { // raw data size = eq 0
			blk->AllocBuffer(pSectionHdr[i].Misc.VirtualSize, pSectionHdr[i].VirtualAddress);
		}

		CSECTION * sec = new CSECTION( &pSectionHdr[i], blk, this );
		printf ( "[%d]%10.8s %08x->%08x\n", (int)i, sec->Record.Name, sec->Record.VirtualAddress, sec->Record.VirtualAddress + sec->Record.Misc.VirtualSize );
		Sections.push_back(sec);
 	}

	printf("Total:%d\n" , _Header32->FileHeader.NumberOfSections);
	
	Description = descr;

	Exports.Create ( this, PrintDetailInfo );
	Imports.Create ( this, PrintDetailInfo );
	Relocs .Create ( this, PrintDetailInfo );
	FuncsTable.SetPrintDetailInfo ( PrintDetailInfo );
	FuncsTable.Create ( this );
	return true;
	
}

#if 0
bool CIMAGE::WritePdb ( const wstring & file )
{
	if ( ReadOnly)
		throw "load only new bin image";

	Pdb.Save ( file );

	return true;
}
#endif

bool CIMAGE::LoadPdb ( const wstring & file, bool PrintDetailInfo )
{
	if ( ! ReadOnly)
		throw "load only loaded bin image";

	printf("-------------------Load Program Data Base ----------------------\n");

	Pdb.Load ( file );
	Pdb.LoadAllSections();
	Pdb.LoadAllModules ();
	Pdb.LoadAllPublicSymbols();

	for ( auto & s : Pdb.Sections ){
		BLOCK<BYTE> * b = GetBlockByRVA ( s->dwRVA );
		CSECTION * sec = GetSectionByRVA (s->dwRVA);

		if ( b != NULL ){
			PBYTE st = (PBYTE) b->GetPtrFromRva (s->dwRVA);
			PBYTE en = (PBYTE) b->GetPtrFromRva (s->dwRVA + s->dwLen - 1 ); // use GetPtr for checking intervals
			if (PrintDetailInfo )
				printf( "Section=<<%s>> Block=<<%s>> %p %p sec:%d - %S\n", sec->Record.Name, b->Desc.data(), st, en,  s->dwSect, s->Modulename.data() );
			b->AddPdbSection (s);
		}
		else{
			if ( sec != NULL ){
				if (PrintDetailInfo )
					printf( "Section=<<%s>> Block <<none>>: rva:%08x, len:%08x, off:%08x, sec:%08x,  %S\n", sec->Record.Name, s->dwRVA.Get32(), s->dwLen, s->dwOffset, s->dwSect, s->Modulename.data() );
				sec->AddPdbSection (s);
			}
			else {
				printf( "error - not section. 0x%08X 0x%X - %S\n",s->dwRVA.Get32(), s->dwLen, s->Modulename.data());
				throw "Pdb invalid";
			}
		}
	}

	for ( auto & ss: Pdb.PubSymbols ){
		auto s = ss.second;
		if ( s->RVA == 0 )
			continue;

		BLOCK<BYTE> * b = GetBlockByRVA ( s->RVA );
		CSECTION * sec = GetSectionByRVA (s->RVA);

		if ( b != NULL ){
			PBYTE st = (PBYTE)b->GetPtrFromRva (s->RVA);
			PBYTE en = (PBYTE)b->GetPtrFromRva (s->RVA + (DWORD)s->Lenght - 1 );
			if (PrintDetailInfo )
				printf( "Section=<<%s>> Block=<<%s>> %p %p, seg:%d - %S, %S\n",sec->Record.Name, b->Desc.data(), st, en, s->Seg, s->Name.data(),s->Undname.data());
			b->AddPdbSymbol (s);
		}
		else{
			if ( sec != NULL ){
				if (PrintDetailInfo )
					printf( "Section=<<%s>> block=<<none>>: rva:%08X len:%08x off:%08x seg:%08x, %S \n" , sec->Record.Name, s->RVA.Get32(), (DWORD)s->Lenght, s->Off, s->Seg, s->Name.data() );
				sec->AddPdbSymbol (s);
			}
			else {
				printf("Error: not ptr for symbol 0x%08X %S\n", s->RVA.Get32(), s->Name.data() );
				throw "Pdb invalid";
			}
		}
	}
	printf("-------------------End Load Program Data Base ----------------------\n");
	return true;

}
bool CIMAGE::CreateNew (  PIMAGE_DOS_HEADER _DosHeader, PIMAGE_NT_HEADERS _Header /*, DWORD InitSize, DWORD ExpandSize*/ , string & descr  )
{
	if ( FileImage )
		return false;

	if ( _Header == NULL )
		_Header =  ImageNtHeader ( _DosHeader );

/*
	The combined size of the following items, rounded to a multiple of the value specified in the FileAlignment member.

			e_lfanew member of IMAGE_DOS_HEADER 
			4 byte signature 
			size of IMAGE_FILE_HEADER 
			size of optional header 
			size of all section headers
*/

	DWORD SizeOfHeaders = 0;

	if ( ::IsHeader64 ( _Header ) )
		SizeOfHeaders = sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_NT_HEADERS64) + _DosHeader->e_lfanew + 4;
	else
		SizeOfHeaders = sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_NT_HEADERS32) + _DosHeader->e_lfanew + 4;

	FileImage =(PBYTE) LocalAlloc ( LMEM_FIXED | LMEM_ZEROINIT, SizeOfHeaders );
	if ( FileImage == NULL )
		return false;

	AllocateFileImageSize = SizeOfHeaders;

	memcpy ( FileImage, _DosHeader, _DosHeader->e_lfanew );

	Description = descr;

	// make ptr to header for copy target
	_Header32 = (PIMAGE_NT_HEADERS32) ((char*)FileImage + _DosHeader->e_lfanew);
	_Header64 = (PIMAGE_NT_HEADERS64)_Header32;

	if ( IsHeader64 ( _Header ) )
		memcpy ( _Header64, _Header, sizeof(IMAGE_NT_HEADERS64)  );
	else
		memcpy ( _Header32, _Header, sizeof(IMAGE_NT_HEADERS32)  );

	//
	// Now the header is good state, we make use it.
	//

	if ( CheckImageType () ){// check image type and set 64/32 bits option flag

		 
		// set option that this image may will assembled from part. 
		ReadOnly = false;

		// standart header the same in 32 and 64 bits
		//
		_Header32->FileHeader.NumberOfSections = 0;  
	
		// optional header is different
		//

		if ( _IsHeader64 ){
			_Header64->OptionalHeader.NumberOfRvaAndSizes=IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
			_Header64->OptionalHeader.SizeOfCode = 0;
			_Header64->OptionalHeader.SizeOfHeaders = SizeOfHeaders;
			_Header64->OptionalHeader.SizeOfImage=0;
			_Header64->OptionalHeader.SizeOfInitializedData=0;
			_Header64->OptionalHeader.SizeOfUninitializedData=0;
			_Header64->OptionalHeader.AddressOfEntryPoint=0;
			_Directorys =  _Header64->OptionalHeader.DataDirectory;
			_SectionAlignment = _Header64->OptionalHeader.SectionAlignment;
			_FileAlignment = _Header64->OptionalHeader.FileAlignment;
		}
		else{
			 _Header32->OptionalHeader.NumberOfRvaAndSizes=IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
			_Header32->OptionalHeader.SizeOfCode = 0;
			_Header32->OptionalHeader.SizeOfHeaders = SizeOfHeaders;
			_Header32->OptionalHeader.SizeOfImage=0;
			_Header32->OptionalHeader.SizeOfInitializedData=0;
			_Header32->OptionalHeader.SizeOfUninitializedData=0;
			_Header32->OptionalHeader.AddressOfEntryPoint=0;
			_Directorys = _Header32->OptionalHeader.DataDirectory;
			_SectionAlignment = _Header32->OptionalHeader.SectionAlignment;
			_FileAlignment = _Header32->OptionalHeader.FileAlignment;
		}

		return true;
	}

	if ( FileImage != NULL ) 
		LocalFree ( FileImage );

	FileImage = NULL;
	
	return false;
}

//
//
//Build binary image ready for write to file. Use Save for write.
//
//

bool CIMAGE::Build ( bool PrintDebugDetails )
{
	if ( ReadOnly )
		throw "try write readonly image";

	// build data directory

	PIMAGE_DATA_DIRECTORY dd = GetDataDirectory (0);

	for ( size_t i=0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++ ){
		dd[i].Size =0;
		dd[i].VirtualAddress =0;
	}

	// let create all required section 

	CSECTION * SecRels = new CSECTION ( ".reloc", IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_CNT_INITIALIZED_DATA );

	// collect all refs from section and image and make relocs

	Relocs.Build ( this, PrintDebugDetails );
	
	// pack relocs to .reloc
	Relocs.Pack ( SecRels, this, PrintDebugDetails  ) ;
		
	AddSection ( SecRels, PrintDebugDetails );

	// new section .rdata
	CSECTION * SecRData = new CSECTION ( ".rdata", IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA );
	
	// pack imports to rdata

	if ( _IsHeader64 )
		Imports.Pack<IMAGE_THUNK_DATA64> ( SecRData, this, PrintDebugDetails );
	else
		Imports.Pack<IMAGE_THUNK_DATA32> ( SecRData, this, PrintDebugDetails );

	// pack exports to .rdata

	Exports.Pack ( SecRData, this, PrintDebugDetails );

	// add rdata to image

	AddSection ( SecRData, PrintDebugDetails );

	// new section .rdata
	//
	
	CSECTION * SecPData = new CSECTION ( ".pdata", IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA );

	// pack
	if ( FuncsTable.Pack ( SecPData, this, PrintDebugDetails ) ){
		// add pdata to image

		AddSection ( SecPData, PrintDebugDetails );
		
	}
	else
		delete SecPData;

	// change header

	DWORD SizeOfHeaders =  CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( FileAlignment ( (DWORD)( GetSizeOfHeaders() + GetSectionCount() * sizeof(IMAGE_SECTION_HEADER) ) ) );

	SetSizeOfHeaders ( SizeOfHeaders );

	return true;
}
bool CIMAGE::Save ( WCHAR * PathName )
{

	DWORD offset = GetSizeOfHeaders(); // 

	DWORD SizeOfInitializedData=0;
	DWORD SizeOfUninitializedData=0;
	DWORD SizeOfCode=0;
	DWORD NoZeroSectionCount=0;

	for ( auto &s : Sections ){
		
		DWORD VirtualSize = 0;
		
		for ( auto &d: s->_Data ){
			VirtualSize += d->GetByteSize();
		}

		if ( VirtualSize == 0 )
			continue;	
		
		NoZeroSectionCount++;

		DWORD SizeOfRawData;

		if ( s->Record.Misc.VirtualSize == 0 ){
			s->Record.Misc.VirtualSize = VirtualSize;
			SizeOfRawData = FileAlignment ( s->Record.Misc.VirtualSize );
		}
		else {
			if ( s->Record.Misc.VirtualSize > s->Record.SizeOfRawData ){
				SizeOfRawData = FileAlignment ( s->Record.SizeOfRawData );
				if ( s->Record.SizeOfRawData != VirtualSize ) 
					throw "section invalid";
			}
			else {
				SizeOfRawData = FileAlignment ( s->Record.Misc.VirtualSize );
				if ( s->Record.Misc.VirtualSize != VirtualSize ) 
					throw "section invalid";
			}
		}

		if ( s->Record.SizeOfRawData == 0 )
			s->Record.SizeOfRawData = SizeOfRawData;
		else
			if (s->Record.SizeOfRawData != SizeOfRawData)
				throw "section invalid";

		if ( s->Record.PointerToRawData == 0 )
			s->Record.PointerToRawData = offset;
		else
			if ( s->Record.PointerToRawData != offset )
				throw "section invalid";
		
		if ( s->Record.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA )
			SizeOfInitializedData += s->Record.SizeOfRawData;
		if ( s->Record.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA )
			SizeOfUninitializedData += s->Record.SizeOfRawData;
		if (  s->Record.Characteristics &  IMAGE_SCN_MEM_EXECUTE )
			SizeOfCode += s->Record.SizeOfRawData;

		offset += SizeOfRawData;
	}

	
	SetNumberOfSections ( NoZeroSectionCount );

	if ( GetSizeOfCode() == 0 )
		SetSizeOfCode(SizeOfCode); 
	else
		if ( GetSizeOfCode() != SizeOfCode )
			throw "header not clean";

	if ( GetSizeOfUninitializedData() == 0 )
		SetSizeOfUninitializedData(SizeOfUninitializedData);
	else
		if ( GetSizeOfUninitializedData() != SizeOfUninitializedData)
			throw "header not clean";

	if ( GetSizeOfInitializedData() == 0 )
		SetSizeOfInitializedData(SizeOfInitializedData);
	else
		if ( GetSizeOfInitializedData() != SizeOfInitializedData){
			printf("no fatal (normal ) Get SizeOfInitializedData != Calc SizeOfInitializedData \n");
		}

	//////////////////////

	SetSizeOfImage (  SectionAlignment ( CRELAITIVE_VIRTUAL_ADDRESS::Get32 (GetMaxRVA()) ) ) ;

// create file 
	
	HANDLE FileHandle = INVALID_HANDLE_VALUE;

	FileHandle =  CreateFileW ( PathName, GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0 );

	if ( FileHandle == INVALID_HANDLE_VALUE ) 
		return false;

	DWORD bw=0;

	LARGE_INTEGER FileOffset;

	LARGE_INTEGER np;


	DWORD SizeOfHeaders = (DWORD)( (PBYTE)IMAGE_FIRST_SECTION ( _Header32 ) - FileImage );

	if ( ! WriteFile ( FileHandle, FileImage, SizeOfHeaders, &bw, 0 ) )
		throw "write error";


	//////////////////

	for ( auto &s :Sections ){
		if ( s->Record.Misc.VirtualSize ){
			if ( ! WriteFile ( FileHandle, &s->Record, sizeof(IMAGE_SECTION_HEADER) , &bw, 0 ) )
				throw "write error";
		}
	}

	BYTE * ZeroBuf = new BYTE[_FileAlignment];

	// new fill zero memory 
	// but fill too
	//
	memset (ZeroBuf, 0, _FileAlignment );

	for ( auto &s :Sections ){

		if ( s->Record.Misc.VirtualSize ){

			FileOffset.QuadPart = s->Record.PointerToRawData;

			if ( ! SetFilePointerEx ( FileHandle, FileOffset, &np, FILE_BEGIN ) )
				throw "SetFilePointerEx";

			DWORD ByteSize=0;

			for ( auto &b: s->_Data ){
				
				if ( b->GetByteSize() )
					if ( ! WriteFile ( FileHandle, b->GetBuffer(), b->GetByteSize(), &bw, 0 ) ){
						throw "write error";
					}
				ByteSize += b->GetByteSize();
			}

			if ( ByteSize != s->Record.Misc.VirtualSize && ByteSize != s->Record.SizeOfRawData )
				throw "oops.";

			if ( s->Record.SizeOfRawData > s->Record.Misc.VirtualSize ){
				if ( ! WriteFile ( FileHandle, ZeroBuf, s->Record.SizeOfRawData - s->Record.Misc.VirtualSize, &bw, 0 ) ){
					throw "write error";
				}
			}
		}
	}

	delete ZeroBuf;

	CloseHandle (FileHandle);

	 

	return true;
}

bool CIMAGE::CheckReferences ( CODE_ANALIZE &env )
{
	size_t WrondVirtualRelocs=0;

	printf("\n\nEraze wrong references");
	printf("\n----------------------");
	
	
	for ( auto &i= env.References.begin(); i!=env.References.end(); i++  ){

		CINTERNAL_RELOC * ir = FindInternalRelocInMyVARelocs ( (*i).Place.VA );
	
		if ( ( (*i).Place.Size != 4 && (*i).Place.Size != 8 && ir != 0 ) || (ir != 0 && (*i).Place.Rip) ){
			printf("\n At 0x%08I64X -> 0x%08I64X type %s, oper:%s, its seems a wrong reference! (will delete)",
				(*i).Place.VA.Get64(),
				(*i).TargetVa.Get64(),  
				CODE_ANALIZE::CREFERENCE::TypeToString((*i).Type), 
				CODE_ANALIZE::CREFERENCE::OperationToString((*i).Operation)
			);

			i = env.References.erase ( i );
			WrondVirtualRelocs++;
			continue;
		}
	 

		if ( (*i).Place.Size == 4 || (*i).Place.Size == 8 ){

			// If the relation carried out as reference by absolute address.
			if ( *(PDWORD)(*i).Place.Ptr == (*i).TargetVa ){ 
				// this MUST_BE  relocs!!!
				// else ref is wrong
				if ( ! ir ){
					printf("\n At 0x%08I64X -> 0x%08I64X type %s, oper:%s, its seems a wrong reference! (will delete)",
						(*i).Place.VA.Get64(),
						(*i).TargetVa.Get64(),  
						CODE_ANALIZE::CREFERENCE::TypeToString((*i).Type), 
						CODE_ANALIZE::CREFERENCE::OperationToString((*i).Operation)
					);
					// below commened code is code fo delete labels which have zero refs to it.
					// but now yet references to labels no add
					// this action do below - in next stage of analize
#if 0					
					CODE_ANALIZE::CREFERENCE * r = &(*i);
					CODE_ANALIZE::LABEL * l = r->ReferenceTo;
#endif
					i = env.References.erase ( i );
					WrondVirtualRelocs++;
#if 0
					if ( l ){
						l->DelReference (r);
						if ( l->References.size() == 0 ){
							auto il = env.Labels.find(l->Va);
							env.Labels.erase(il);
						}
					}
#endif
				}
			}
		}
	}

	printf("\nTotal :%d\n", (int)WrondVirtualRelocs );
	return true;
}
bool CIMAGE::DoRefsAnalize ( CODE_ANALIZE &env, bool PrintDetailInfo )
{
	if ( PrintDetailInfo ){
		printf("\nList of suspicious Refs.");
		printf("\n------------------------");
		printf("\n*** Such that not in PDB (skiping of the local short jumps)");
		printf("\n");
		printf("\nFrom                To                (Rva       ) :Sz  Type Operation");
		printf("\n------------------  ------------------(----------) :--- --- --");
	}

	size_t SuspiciousRefsCount =0;

	for ( auto &r: env.References ){
		_ASSERTE( r.ReferenceTo != NULL );
		CRELAITIVE_VIRTUAL_ADDRESS rva = GetRvaFromVA ( r.ReferenceTo->Va );

		auto s = Pdb.PubSymbols.lower_bound(rva);

		CPDB::CSYMBOL* sym = 0;

		if ( s != Pdb.PubSymbols.end() ){

			if ( rva < (*s).second->RVA )
				if ( s !=  Pdb.PubSymbols.begin() )
					s--;
			if ( rva >= CRELAITIVE_VIRTUAL_ADDRESS ( (*s).second->RVA ) && rva <= CRELAITIVE_VIRTUAL_ADDRESS ( (*s).second->RVA + (DWORD)(*s).second->Lenght) )
				sym = (*s).second;
		}

		if ( !sym ){
			if ( r.Type != CODE_ANALIZE::CREFERENCE::Jump ){
				if ( PrintDetailInfo ){
					printf("\n0x%016I64X->0x%016I64X(0x%08X) :%03d %s %s",
								r.Place.VA.Get64(), 
								r.ReferenceTo->Va.Get64(),
								rva.Get32(), 
								(int)r.Place.Size,
								CODE_ANALIZE::CREFERENCE::TypeToString(r.Type), 
								CODE_ANALIZE::CREFERENCE::OperationToString(r.Operation) 
							);
				}
				SuspiciousRefsCount++;
			}
		}

		r.ReferenceTo->PubSymbol = sym;
		

		// we don't add a new relocs to images

#if 0
		// we create only DWORD-size relocs
		if ( r.Place.Size == 4 ){
			// If the relation carried out as reference by absolute address.
			if ( *(PDWORD)r.Place.Ptr == r.TargetVa ){ 

				if ( ! FindInternalRelocInMyVARelocs ( r.Place.VA ) ){
					printf("\n Seems wrong reference!");

				}

				bool IsOk;
				if ( r.ReferenceTo->PubSymbol )
					IsOk = CreateRelation ( r.Place.VA, r.TargetVa, *ToString ( (const wstring &)r.ReferenceTo->PubSymbol->Name) ) ;
				else
					IsOk = CreateRelation ( r.Place.VA, r.TargetVa, "Create in code analize" ) ;
				if ( IsOk )
					printf("\n22"); // 
			}
			else
					printf("\n33");
		}
#endif
	}

	if ( ! PrintDetailInfo )
		printf("\nRefs analize - Suspicious Refs was detected (this is not error):%d\n.", (int)SuspiciousRefsCount );
	else 
		printf("\nTotal :%d\n", (int)SuspiciousRefsCount );

	return true;
}

bool CIMAGE::CreateVirtualRelocs ( CODE_ANALIZE &env, bool InterSectionOnly, bool PrintDetailInfo )
{
	printf("\n\n\nCreated virtual relocs");
	printf(   "\n------------------------");
	printf("\n");

	PIMAGE_DATA_DIRECTORY iat = GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_IAT );

	CVIRTUAL_ADDRESS IatVa = GetVaFromRVA ( iat->VirtualAddress );

	size_t NeedAdditionalVirtualRelocs=0;

	for ( auto & i:env.References ) {
		
		CSECTION * SourceSec = GetSectionByVA ( i.Place.VA );
		CSECTION * TargetSec = GetSectionByVA ( i.TargetVa );

		if ( SourceSec != TargetSec || ! InterSectionOnly ){

			CINTERNAL_RELOC * ir = FindInternalRelocInMyVARelocs ( i.Place.VA );

			// First of all -- if this reference is a full virtual address but relocs not exist in image -- this is error. This ref seems error detected
			// and it will be delete in next stage of analize (see below).
			
			// Second if this reference use RIP but reloc exist - seems this wrond and reference need to erase ( next stage )

			// Third, if this reference use RIP for get target address, then need create _virtual_ relocs

			if ( ! ir && i.Place.Rip ){

				NeedAdditionalVirtualRelocs++;
#if 0			 
				printf ( "\n%05d At (%8.8s)0x%016I64X -> (%8.8s)0x%016I64X type %4.4s, oper:%8.8s, Size:%d, RIP:%d, RIP Base: 0x%016I64X need make virtual reloc\n", 
					NeedAdditionalVirtualRelocs,
					SourceSec->Record.Name,
					i.Place.VA,
					TargetSec->Record.Name,
					i.TargetVa,  
					CODE_ANALIZE::CREFERENCE::TypeToString( i.Type ), 
					CODE_ANALIZE::CREFERENCE::OperationToString( i.Operation), 
					i.Place.Size,
					i.Place.Rip,
					i.Place.RipBase 
				);
#endif				 
				// create link object
				CINTERNAL_RIP_RELOC * ir = new CINTERNAL_RIP_RELOC ( i.Place.VA, GetImageBase(), i.Place.RipBase, (void*)i.Place.Ptr, (DWORD)i.Place.Size, "Virtual" );

				// Add point with virtual address in source block 
				SourceSec->AddMyVAReloc ( ir );

				// check for import
				if ( i.TargetVa >= IatVa && i.TargetVa < IatVa + iat->Size ){
					
					CIMPORTS::IMPORT * f =  GetImportFuncByIatOffset ( CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( i.TargetVa - IatVa) );
					f->AddRef ( ir );
					string d("Reference to fununction via IAT, from :");
					ir->SetDescription ( d + Description + "@" + (const char * )SourceSec->Record.Name  );

					if ( PrintDetailInfo )
						printf ( "%s:0x%016I64X(0x%016I64X) -> %s:0x%08x import func %s::%s (descr:%s)\n", SourceSec->Record.Name, ir->GetSourcePoint().Get64(), ir->GetTargetPoint().Get64(),TargetSec->Record.Name, ir->GetTargetPointRva().Get32(), f->Dll->DllName->data(), f->GetFuncName(), ir->GetDescription().data() );
				}
				else{
					// Write to source point value of displ to the target point.
					// and add to contain target point block the 'refenece'
				
					if ( TargetSec ){
						
						TargetSec->WriteVA ( ir, ir->GetTargetPoint() );
						string d ( "Reference to ");
						ir->SetDescription ( d + (const char * )TargetSec->Record.Name + ", from " + Description + "@" + (const char * )SourceSec->Record.Name  );

						if ( PrintDetailInfo )
							printf("%s:0x%016I64X(0x%016I64X) -> %s:0x%08X, Descr:%s\n", SourceSec->Record.Name, ir->GetSourcePoint().Get64(), ir->GetTargetPoint().Get64(), TargetSec->Record.Name, ir->GetTargetPointRva().Get32(), ir->GetDescription().data() );
					}
					else{
						WriteVA ( ir, ir->GetTargetPoint() );
						ir->SetDescription ( "Reference outsize any sectons, from:" + Description + "@" + (const char * )SourceSec->Record.Name  );
						if ( PrintDetailInfo )
							printf("%s:0x%016I64X(0x%016I64X) -> (img):0x%08X Descr:%s\n", SourceSec->Record.Name, ir->GetSourcePoint().Get64(), ir->GetTargetPoint().Get64(), ir->GetTargetPointRva().Get32(), ir->GetDescription().data() );
					}
				}
			}
		}
	}

	printf("\nTotal :%d\n", (int)NeedAdditionalVirtualRelocs );
	return true;
}

// The code analysis process includes reassemble for detail analysis of operands.
// All jump/call instructions are executed. 
// For point "target" adds structure LABEL. 
// For point "source" adds structure "REFERENCE". 
// For all rest of instructions, if address of its operand looks correct,  the LABEL and the REFERENCE are created.

bool  CIMAGE::MakeCodeAnalize ( bool PrintDetailInfo )
{

	csh cs_handle;
	
	cs_mode mode;

	if ( IsImage64() )
		mode = CS_MODE_64;
	else
		mode = CS_MODE_32;

	if ( GetArch() != IMAGE_FILE_MACHINE_I386 && GetArch() != IMAGE_FILE_MACHINE_AMD64 ){
		throw "Arch not supported.";
		return false;
	}

	if ( cs_open ( CS_ARCH_X86, mode, &cs_handle) ) 
		return false;

	cs_option ( cs_handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL );
	cs_option ( cs_handle, CS_OPT_DETAIL, CS_OPT_ON );

	CVIRTUAL_ADDRESS Start = GetVaFromRVA ( GetEntryPoint () );

	// set true for produce assembler output listing
	//
	bool ProduseDisAssemblerText = false;

	if ( PrintDetailInfo )
		ProduseDisAssemblerText = true;

	CODE_ANALIZE env ( this, cs_handle, ProduseDisAssemblerText );
	
	if ( PrintDetailInfo )
		env.WarningsSet = (CODE_ANALIZE::WarringsEnabe) ( env.UseRegisterInCallJump | env.BadBaseInCallJump | env.BadDisplInCallJump | env.CallJumpToJunk );
	else
		env.WarningsSet = (CODE_ANALIZE::WarringsEnabe) ( env.BadDisplInCallJump | env.CallJumpToJunk );


	printf("\n*** The code analize start***");

	printf("\n*** The code analize from entry point : Start");

	env.Analize ( Start, true );

	printf("\n*** The code analize from entry point : Done");

	if ( ! Pdb.PubSymbols.empty() ){
		printf("\n*** The code analize from Programm Data Base (PDB) : Start");
		
		for ( auto & ff : Pdb.PubSymbols ){
			auto f = ff.second;
			if ( f->Code ){
				if ( f->Function ){
					if ( PrintDetailInfo )
						wprintf( L"\n%s",f->Name.data());
					env.Analize ( GetVaFromRVA ( f->RVA  ) , true );
				}
			}
		}

		printf("\n*** The code analize from Programm Data Base (PDB) : Done");
	}

	for ( auto & s : Sections )	{
		for ( auto & b: s->_Data ){
			const map <CRELAITIVE_VIRTUAL_ADDRESS, CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY*> & Funcs = b->GetFuncsTable();
			
			if ( !Funcs.empty() ){
				printf("\n*** The code analize from 'RUNTIME_FUNCTIONS' TABLE in %s: Start",  b->Desc.data() );
		
				for ( auto & ff : Funcs ){
					auto & f = ff.second;

					if ( PrintDetailInfo )
						printf("\nSub %016I64X", GetVaFromRVA ( f->BeginAddress ).Get64() );
					
					env.Analize ( GetVaFromRVA ( f->BeginAddress ) , true );
					if ( f->ExceptionHandler != 0 ){
						if ( PrintDetailInfo )
							printf("\nException sub %016I64X", GetVaFromRVA ( f->ExceptionHandler ).Get64() );
						env.Analize ( GetVaFromRVA ( f->ExceptionHandler ) , true );
					}
				}
				printf("\n*** The code analize from 'RUNTIME_FUNCTIONS' TABLE in %s : Done" , b->Desc.data() );
			}			
		}
	}

	cs_close( & cs_handle);

	printf("\n*** The code analize End ***");

	printf("\n\nStart analysis of the results of the analysis code. :-)");
	printf("\n---------------------------------------------------");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CheckReferences ( env );
	CreateVirtualRelocs ( env, false, PrintDetailInfo );
	
	// go thruw all references, and saerch in labels array, target of reference
	// if not create new label
	// and finally to the label add a reference to it.
	// and to the reference add label on which it refers
 
	for ( auto &r: env.References ){
		CODE_ANALIZE::LABEL * l = env.FindLabel ( r.TargetVa );
		if ( !l ){
			l = new CODE_ANALIZE::DATALABEL ( r.TargetVa, r.TargetSize );
			env.AddLable ( l );
		}

		l->AddReference ( &r );
		r.ReferenceTo = l;
	}

	//init fill label array
	for ( auto & l : env.Labels ){
		l.second->PubSymbol = nullptr;
	}

	// Thru all public symbols find the corresponding label and then add this pub symbol to this label.

	for ( auto & ss : Pdb.PubSymbols ){
		auto s = ss.second;
		CVIRTUAL_ADDRESS SymbolVa  = GetVaFromRVA ( s->RVA );
		auto l = env.Labels.find ( SymbolVa ) ;
		if ( l != env.Labels.end() ){
			(*l).second->PubSymbol = s; 
		}
	 
#if 0
		// Normally these are the symbols that have links type data<-->data.
		// Now, we are no need to move and edit data, and we don't do data analysis.
		// If in the future you will need to edit some datas, then you have to do this analysis yourself.

		else {
			printf("\nSymbol %S (0x%08h) was not detected in code analize process.", s->Name.data(), SymbolVa );
		}
#endif
	}

	//Release this big array. Simply it is no need any more
	delete env.AnalizeSpace;

	// create relation between source and target 
	// if have name use it 

	
	DoRefsAnalize(env, PrintDetailInfo);

	printf("\nThe End. (of analysis)\n");

#if 0	
	// printf map of code analize results
	DWORD va = env.MinVa;
	bool a = false;
	size_t e =  env.AnalizeSpace2->Size();

	for ( size_t i = 0; i < e; i++ ){
		bool b = env.AnalizeSpace2->Test(i);
		if ( b != a )
			if ( b )
				printf("\n%08X +",va );
			else
				printf("\n%08X -",va );
		a=b;
		va++;
	}
#endif

	return true;
}

// foolish convert func - need for convert name from wstring to string 
// 
string * ToString ( const wstring & s )
{
	string * rs = new string();
	size_t l = s.length() ;
	rs->resize (l);

	size_t i;
	for ( i=0; i<l; i++ )
		(*rs)[i] = (char)s[i];

	return rs;
}
