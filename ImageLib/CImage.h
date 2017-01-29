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

class CSECTION;
class CEXPORTS;
class CIMPORTS;
class CRELOCS;
struct CODE_ANALIZE;
class CIMAGE
{

	CPDB Pdb;

	// load bin image 
	// no for edit. To edit create new.
	bool ReadOnly;

	bool _IsHeader64;
	
	// ptr to image start
	PBYTE FileImage; 

	DWORD AllocateFileImageSize;
	
	PIMAGE_NT_HEADERS32 _Header32;
	PIMAGE_NT_HEADERS64 _Header64;

	map<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*> MyVARelocs;// // This relocs such localed into this place 
	vector<CINTERNAL_RELOC*> VARelocs;//

	vector<CSECTION*> Sections;
	PIMAGE_DATA_DIRECTORY _Directorys;

	CIMPORTS Imports;
	CRELOCS  Relocs;
	CEXPORTS Exports;
	CFUNCSTABLE FuncsTable;

	bool _Load ( WCHAR * ImageName );
	DWORD Read ( WCHAR * ImageName );
	DWORD Write ( WCHAR * PathName );

	bool CheckImageType (); // check image for compatibility with this software and set 64 or 32 bits option. See _IsHeader64 above.

	PIMAGE_DATA_DIRECTORY _HeaderGetDataDirectory ( size_t Dir );

	DWORD _SectionAlignment;
	DWORD _FileAlignment;

	void ReAllocImageBuffer ( DWORD NewSize );


public:

	WORD GetArch ( void )
	{
		return _Header32->FileHeader.Machine;
	}

	bool LoadPdb ( const wstring & file, bool PrintDetailInfo  );
//	bool WritePdb ( const wstring & file );

	bool CreateVirtualRelocs ( CODE_ANALIZE & env, bool InterSectionOnly, bool PrintDetailInfo );
	bool CheckReferences ( CODE_ANALIZE & env );
	bool DoRefsAnalize ( CODE_ANALIZE &env, bool PrintDetailInfo );

	bool MakeCodeAnalize ( bool PrintDetailInfo );
	
	string Description;
	
	DWORD GetAllocateFileImageSize (void )
	{
		return AllocateFileImageSize;
	}

	CVIRTUAL_ADDRESS GetImageBase (void )
	{
		if ( _IsHeader64 )
			return CVIRTUAL_ADDRESS ( _Header64->OptionalHeader.ImageBase );
		else
			return CVIRTUAL_ADDRESS (  _Header32->OptionalHeader.ImageBase );
	}
	CRELAITIVE_VIRTUAL_ADDRESS GetRvaFromVA ( CVIRTUAL_ADDRESS va )
	{

		return CRELAITIVE_VIRTUAL_ADDRESS ( va - GetImageBase() );
	}
	CVIRTUAL_ADDRESS GetVaFromRVA ( CRELAITIVE_VIRTUAL_ADDRESS rva )
	{
		return GetImageBase() + rva;
	}
	bool AddExport ( char * name, CRELAITIVE_VIRTUAL_ADDRESS rva )
	{
		if ( ReadOnly )
			throw "image is readonly";

		//TODO: need check 'va' conditions there

		Exports.Add ( name, rva );
		return true;
	}
	bool AddExport ( const CEXPORTS::EXPORT &n )
	{
		if ( ReadOnly )
			throw "image is readonly";

		Exports.Add (n);
		return true;
	}
	void AddImportDll ( CIMPORTS::IMPORT_DLL *n)
	{
		if ( ReadOnly )
			throw "image is readonly";

		Imports.AddDll(n);
	}
	void AddImport ( CIMPORTS::IMPORT_DLL * Dll )
	{
		if ( ReadOnly )
			throw "image is readonly";

		Imports.AddImport ( Dll );
	}
	size_t GetImportsCount ()
	{
		return Imports.Size();
	}
	CIMPORTS::IMPORT_DLL * GetImport ( size_t i )
	{
		if ( i < Imports.Size() )
			return Imports.Get(i);

		throw "out of imports";
	}
	const vector<CIMPORTS::IMPORT_DLL *> GetAllImports ( void )
	{
		return Imports.GetAllImports();
	}

	CIMPORTS::IMPORT * GetImportFuncByIatOffset ( DWORD num )
	{
		if ( _IsHeader64 )
			num = num/sizeof(IMAGE_THUNK_DATA64);
		else
			num = num/sizeof(IMAGE_THUNK_DATA32);

		return Imports.GetImportFuncByNum(num);
	}
	// set export name for all the image
	bool SetExportName ( char * n )
	{
		if ( ReadOnly )
			throw "try edit readonly image.";

		Exports.Name = n;
		return true;
	}
	char * GetExportName ( void )
	{
		return Exports.Name;
	}
	size_t GetExportsCount ()
	{
		return Exports.Size();
	}
	CEXPORTS::EXPORT GetExport ( size_t i )
	{
		if ( i < Exports.Size() )
			return Exports.Get(i);
		throw;
	}
	const vector <CEXPORTS::EXPORT> GetAllExports ( void )
	{
		return Exports.GetAllExports();
	}
	CIMAGE()
		:FileImage(NULL),Description("")
	{}
	~CIMAGE()
	{
		if ( FileImage != NULL )
			LocalFree( FileImage );
	}
	bool OpenExist ( WCHAR * ImageName, PDWORD error, string & descr, bool PrintDetailInfo );
	bool CreateNew ( PIMAGE_DOS_HEADER DosHeader, PIMAGE_NT_HEADERS h, string & descr  );

	bool Save ( WCHAR * ImageName );
	
	bool Build ( bool PrintDetailInfo  );
	void AddSection ( char Name[8], CRELAITIVE_VIRTUAL_ADDRESS VA, DWORD Size, char Fill, DWORD Flg, bool PrintDebugDetails );
	void AddSection ( CSECTION * n, bool PrintDebugDetails );
	bool DelSection ( size_t n )
	{
		throw "no yet implement";
	}
	void SetEntryPoint ( CRELAITIVE_VIRTUAL_ADDRESS target_rva )
	{
		if ( ReadOnly )
			throw "Try edit read only image";

		CRELAITIVE_VIRTUAL_ADDRESS * place; // size is DWORD32 for all image types

		// but place is difference
		if ( _IsHeader64 )
			place = (CRELAITIVE_VIRTUAL_ADDRESS*)&_Header64->OptionalHeader.AddressOfEntryPoint;
		else
			place = (CRELAITIVE_VIRTUAL_ADDRESS*)&_Header32->OptionalHeader.AddressOfEntryPoint;

		// if wants setup zero value ?
		if ( target_rva != CRELAITIVE_VIRTUAL_ADDRESS(0) ){
			// search section where the address is located
			CSECTION * target_sec = GetSectionByRVA ( target_rva );
			CINTERNAL_RVA_RELOC * r = new CINTERNAL_RVA_RELOC ( target_rva, GetImageBase(), place, "EntryPoint" );
			if ( target_sec )
				target_sec->WriteRVA ( r, target_rva );// this place where EntryPoint located (this header) don't have VA, in image virtual speace
			else 
				throw "Virtual address not found";
		}
		else
			*place = target_rva; //set 0 and don't create any relocs
	}
	CRELAITIVE_VIRTUAL_ADDRESS GetEntryPoint ( void )
	{
		if ( _IsHeader64 )
			return _Header64->OptionalHeader.AddressOfEntryPoint;

		return _Header32->OptionalHeader.AddressOfEntryPoint;
	}
	bool IsImage64 ()
	{
		return _IsHeader64 ;
	}

	void PrintHeader ( void );
	void PrintOptionalHeader ();
	void PrintSections ();
	void PrintExportDirectory ();
	void PrintImportDirectory ();
	void PrintBaseRelocDirectory ();
	DWORD GetMinPointerToRawData ( void );
	PBYTE GetPtrFromFileOffset ( DWORD of );
	DWORD GetFileOffsetFromPtr ( PBYTE p );
//	DWORD GetMaxPointerToRawData ( void );
	void * GetPtrFromRVA ( CRELAITIVE_VIRTUAL_ADDRESS rva )
	{
		CSECTION * s = GetSectionByRVA ( rva );
		if ( s != NULL ){
			BLOCK<BYTE> * b = s->GetBlockByRVA ( rva );
			if ( b != NULL )
				return  b->GetPtrFromRva (rva) ; 
		}
		throw "Virtual address not found";
	}

	// hard coded size 
	// for me now it's good.
	// check and edit there if you want work with all relocs!!!
	DWORD GetRelocSize()
	{
		if ( IsImage64() )
			return 8;
		return 4;
	}

	CINTERNAL_VA_RELOC * CreateNewInternalVaRelocFromPtr ( void * ptr, const string desc )
	{
		CSECTION * s = GetSectionByPtr ( (PBYTE)ptr );
		if ( s ){
			CRELAITIVE_VIRTUAL_ADDRESS rva = s->GetRVAFromPtr ( (PBYTE)ptr );
			return new CINTERNAL_VA_RELOC ( GetVaFromRVA ( rva ), GetImageBase() ,ptr, GetRelocSize(), desc );
		}
		throw "pointer outside of datas";
	}

	CINTERNAL_RIP_RELOC * CreateNewInternalRipRelocFromPtr ( void * ptr, DWORD BaseOffset, const string desc )
	{
		CSECTION * s = GetSectionByPtr ( (PBYTE)ptr );
		if ( s ){
			CRELAITIVE_VIRTUAL_ADDRESS rva = s->GetRVAFromPtr ( (PBYTE)ptr );
			CVIRTUAL_ADDRESS va = GetVaFromRVA ( rva );
			return new CINTERNAL_RIP_RELOC ( va, GetImageBase(), va + BaseOffset , ptr, GetRelocSize(), desc );
		}
		throw "pointer outside of datas";
	}

	void * GetPtrFromVA ( CVIRTUAL_ADDRESS va )
	{
		CSECTION * s = GetSectionByVA ( va );
		if ( s != NULL ){
			CRELAITIVE_VIRTUAL_ADDRESS rva = GetRvaFromVA ( va );
			BLOCK<BYTE> * b = s->GetBlockByRVA ( rva );
			if ( b != NULL )
				return  b->GetPtrFromRva ( rva ) ; 
		}
		return NULL;	//throw "Virtual address not found";
	}

	CSECTION * GetSectionByRVA ( CRELAITIVE_VIRTUAL_ADDRESS rva )
	{
		for ( auto &s : Sections ){
			if ( s->IsRVAInsize ( rva ) )
				return s;

		}
		return NULL; 		// throw "Virtual address not found";
	}

	BLOCK<BYTE>* GetBlockByRVA ( CRELAITIVE_VIRTUAL_ADDRESS rva )
	{
		for ( auto &s : Sections ){
			if ( s->IsRVAInsize ( rva ) )
				return s->GetBlockByRVA ( rva );
		}
		return NULL; 		// throw "Virtual address not found";
	}

	CSECTION * GetSectionByVA ( CVIRTUAL_ADDRESS va )
	{
		CRELAITIVE_VIRTUAL_ADDRESS rva = va - GetImageBase();
		for ( auto &s : Sections ){
			if ( s->IsRVAInsize ( rva ) )
				return s;

		}
		return NULL; 	// throw "Virtual address not found";
	}
	CSECTION * GetSectionByPtr ( PBYTE ptr )
	{
		for ( auto &s : Sections){
			BLOCK<BYTE>* b = s->GetBlockByPtr (ptr);
			if ( b )
				return s;
		}
		return NULL; 	// throw "Virtual address not found";
	}

	BLOCK<BYTE> * GetBlockByPtr ( void * ptr )
	{
		for ( auto &s : Sections){
			BLOCK<BYTE>* b = s->GetBlockByPtr (ptr);
			if ( b )
				return b;
		}
		return NULL; 
	}

	CPDB::CSYMBOL* GetFunctionByName ( wstring name )
	{
		for ( auto & s : Sections ){
			CPDB::CSYMBOL* r = s->GetSymbolByName ( name );
			if ( r )
				return r;
		}
		return 0;
	}

	CRELAITIVE_VIRTUAL_ADDRESS GetRvaFromPtr ( void* ptr )
	{
		// scan all sections and its data-blocks for address interval such to have this 'ptr'
		// return virtual address of of this ptr in the block

		BLOCK<BYTE>*  b = GetBlockByPtr (ptr);
		if ( b != NULL )
			return b->GetRvaFromPtr ( ptr );
		
		// ptr not found in section
		// check maybe header
		
		if ( ptr >= (PBYTE)_Header32  && ptr <= (PBYTE)_Header32 + GetSizeOfHeaders() )
			return CRELAITIVE_VIRTUAL_ADDRESS(0);
		
		printf("ptr:0x%p not in image %s -- fatal error.\n", ptr, Description.data() );

		for ( auto s: Sections ){
			for ( auto b: s->_Data ){
				printf ( "sec:<<%s>> va:0x%08X blk:<<%s>> va:0x%08x ptr: 0x%p - 0x%p\n", s->Record.Name, s->Record.VirtualAddress, b->Desc.data(),b->GetRelativeVirtialAddress().Get32(),b->GetBuffer(),b->GetBuffer()+b->GetByteSize());
			}
		}

		throw "Pointr not found";
	}

	CVIRTUAL_ADDRESS GetMinVA ( void );
	CVIRTUAL_ADDRESS GetMaxVA ( void );
	CRELAITIVE_VIRTUAL_ADDRESS GetMinRVA ( void );
	CRELAITIVE_VIRTUAL_ADDRESS GetMaxRVA ( void );

	// returns aligned value
	DWORD PEAlign ( DWORD dwTarNum, DWORD dwAlignTo )
	{
		return(((dwTarNum+dwAlignTo-1)/dwAlignTo)*dwAlignTo);
	}
	DWORD SectionAlignment ( const DWORD  s )
	{
		return PEAlign ( s, _SectionAlignment );
	}
	DWORD FileAlignment ( const DWORD s )
	{
		return PEAlign ( s, _FileAlignment );
	}

	bool GetFileOffsetFromRVA ( CRELAITIVE_VIRTUAL_ADDRESS VA, DWORD * RetPtr );
//	bool CopyData ( DWORD Dst, DWORD From, DWORD To  ); // хрен знает нужны эти функции или нет
//	bool CopyCode ( DWORD Dst, DWORD From, DWORD To  );
	PIMAGE_DATA_DIRECTORY GetDataDirectory ( size_t Dir );
	PIMAGE_DOS_HEADER GetFileImage ()
	{
		return (PIMAGE_DOS_HEADER)FileImage;
	}
	DWORD GetSectionCount () 
	{
		// dword here only for avoid unessesery conversion
		return (DWORD)Sections.size();
	}
	CSECTION * GetSection ( char * name )
	{

		for ( size_t i=0; i < Sections.size(); i++ ){
			if ( ! memcmp ( name, Sections[i]->Record.Name, IMAGE_SIZEOF_SHORT_NAME ) )
				return Sections[i];
		}
		return NULL;
	}
	CSECTION * GetSection ( size_t n )
	{
		if ( n < Sections.size() )
			return Sections[n];
		return NULL;
	}
	const vector<CSECTION*> & GetAllSection ( void )
	{
		return Sections;
	}
	const vector<CINTERNAL_RELOC*> & GetVARelocs (void )
	{
		return VARelocs;
	}
	
/*
	// write ref 
	void WriteRVA ( CINTERNAL_RVA_RELOC *r, CRELAITIVE_VIRTUAL_ADDRESS target_rva )
	{
		// get section of target
		CSECTION * s = GetSectionByRVA ( target_rva );
		if ( s ) {
			s->WriteRVA ( r, target_rva );
			return ;
		}

		// not target section then use image as place where reloc will storage

		//r->Write ( target_rva ); // *ptr = va;
		r->SetTargetPoint ( target_rva );

		RVARelocs.push_back ( r );
	}
	
*/
	CINTERNAL_RELOC * FindInternalRelocInMyVARelocs ( CVIRTUAL_ADDRESS va )
	{
		BLOCK<BYTE> * b = GetBlockByRVA ( GetRvaFromVA ( va ) );
		if ( b )			
			return b->FindInternalRelocInMyVARelocs ( va, GetImageBase() );

		auto rr = MyVARelocs.find(va);
		if ( rr == MyVARelocs.end() )
			return 0;

		return rr->second;
	}

	// set relation between one memory point to another 
	// The relation has two point 
	// 1 - source point 
	// 2 - target point 
	// in source point write value of VA
	// to target point add reference to this point

	bool CreateRelationViaVa ( CVIRTUAL_ADDRESS VaSource, CVIRTUAL_ADDRESS VaTarget, const string Descr )
	{
		CINTERNAL_RELOC * ir = FindInternalRelocInMyVARelocs ( VaSource );

		// source place is uniq key in image
		// one reloc per address
		// target is not uniq key

		if ( ir == NULL){

			// create link object
			ir = CreateNewInternalVaRelocFromPtr ( GetPtrFromVA ( VaSource ), Descr );

			// Write to source point value of the virtual address of the target point.
			// and add to contain target point block the 'refenece'
			WriteVA ( ir, VaTarget );

			// Add point with virtual address in source block 
			AddMyVAReloc ( ir );
			return true;
		}

		return false;

	}

	bool CreateRelationViaRip ( CVIRTUAL_ADDRESS VaSource, CVIRTUAL_ADDRESS ImageBase, CVIRTUAL_ADDRESS PlaceBase, DWORD Size,  CVIRTUAL_ADDRESS VaTarget, const string Descr )
	{
		CINTERNAL_RELOC * ir = FindInternalRelocInMyVARelocs ( VaSource );

		// source place is uniq key in image
		// one reloc per address
		// target is not uniq key

		if ( ir == NULL){

			// create link object
			ir = new CINTERNAL_RIP_RELOC ( VaSource, ImageBase, PlaceBase, GetPtrFromVA(VaSource), Size, Descr );
			
			// Write to source point value of displ to the target point.
			// and add to contain target point block the 'refenece'

			WriteVA ( ir, VaTarget );

			// Add point with virtual address in source block 
			AddMyVAReloc ( ir );
			return true;
		}

		return false;

	}
	
	void WriteVA ( CINTERNAL_RELOC *r, CVIRTUAL_ADDRESS target_va )
	{
		
		CSECTION * s = GetSectionByVA ( target_va );
		if ( s ) {
			s->WriteVA ( r, target_va );
			return;
		}

		// one target place can have multi reference
		// then we don't check for 

		r->SetTargetPoint ( target_va );
		VARelocs.push_back ( r );
	}
	
	// inverse operation for WriteVa 
	// remove 
	void RemoveVAReferece ( CINTERNAL_VA_RELOC * r )
	{
		CSECTION * s = GetSectionByVA ( r->GetTargetPoint() );

		if ( s ){
			s->RemoveVAReferece ( r, GetImageBase() );
			return;
		}

		auto item = std::find ( VARelocs.begin(), VARelocs.end(), r );

		if ( item == VARelocs.end() )
			throw "reloc not found";

		VARelocs.erase ( item );
	}

	void RemoveMyVAReloc ( CINTERNAL_VA_RELOC * r  )
	{
		CSECTION * s = GetSectionByPtr ( (PBYTE)r->GetPtr() );
		if ( s != NULL ){
			s->RemoveMyVAReloc (r);
			return;
		}
		
		auto item = MyVARelocs.find ( r->GetSourcePoint() );

		if ( item == MyVARelocs.end() )
			throw "reloc not found";
		MyVARelocs.erase ( item );
	}

	void AddMyVAReloc (  CINTERNAL_RELOC *r )
	{

		CSECTION * s = GetSectionByPtr ( (PBYTE)r->GetPtr() );
		if ( s ) {
			s->AddMyVAReloc ( r );
			return;
		}

		if ( MyVARelocs.find ( r->GetSourcePoint() ) != MyVARelocs.end() )
			throw "multiplay add";

		MyVARelocs.insert ( pair<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*>( r->GetSourcePoint() , r ) );
	}

	void ChangeImageBase ( CVIRTUAL_ADDRESS va, bool PrintDebugDetails )
	{

		CRELAITIVE_VIRTUAL_ADDRESS delt = va - GetImageBase() ;

		for ( auto & r: VARelocs ){
			r->SetTargetPoint ( r->GetTargetPoint() + delt );//	do : *p += delt;
		}
		for ( auto & rr: MyVARelocs ){
			auto r = rr.second;
			r->SetSourcePoint ( r->GetSourcePoint() + delt);
		}
		for ( auto &s: Sections )
			s->ChangeVirtualAddress ( GetRvaFromVA( va ), PrintDebugDetails );
	}

	DWORD GetSizeOfHeaders (void)
	{
		if ( _IsHeader64 )
			return _Header64->OptionalHeader.SizeOfHeaders;
		else
			return _Header32->OptionalHeader.SizeOfHeaders;
	}
	DWORD GetSizeOfImage ( void )
	{
		if ( _IsHeader64 )
			return _Header64->OptionalHeader.SizeOfImage;
		else
			return _Header32->OptionalHeader.SizeOfImage;
	}
	DWORD GetSizeOfInitializedData( void )
	{
		if ( _IsHeader64 )
			return _Header64->OptionalHeader.SizeOfInitializedData;
		else
			return _Header32->OptionalHeader.SizeOfInitializedData;
	}
	DWORD GetSizeOfUninitializedData( void )
	{
		if ( _IsHeader64 )
			return _Header64->OptionalHeader.SizeOfUninitializedData;
		else
			return _Header32->OptionalHeader.SizeOfUninitializedData;
	}
	DWORD GetSizeOfCode( void )
	{
		if ( _IsHeader64 )
			return _Header64->OptionalHeader.SizeOfCode;
		else
			return _Header32->OptionalHeader.SizeOfCode;
	}
	void SetSizeOfHeaders ( DWORD v )
	{
		if ( _IsHeader64 )
			_Header64->OptionalHeader.SizeOfHeaders = v;
		else
			_Header32->OptionalHeader.SizeOfHeaders = v;
	}
	void SetSizeOfImage ( DWORD v )
	{
		if ( _IsHeader64 )
			_Header64->OptionalHeader.SizeOfImage=v;
		else
			_Header32->OptionalHeader.SizeOfImage=v;
	}
	void SetSizeOfInitializedData( DWORD v )
	{
		if ( _IsHeader64 )
			_Header64->OptionalHeader.SizeOfInitializedData=v;
		else
			_Header32->OptionalHeader.SizeOfInitializedData=v;
	}
	void SetSizeOfUninitializedData( DWORD v )
	{
		if ( _IsHeader64 )
			_Header64->OptionalHeader.SizeOfUninitializedData=v;
		else
			_Header32->OptionalHeader.SizeOfUninitializedData=v;
	}
	void SetSizeOfCode( DWORD v )
	{
		if ( _IsHeader64 )
			_Header64->OptionalHeader.SizeOfCode=v;
		else
			_Header32->OptionalHeader.SizeOfCode=v;
	}
	void SetNumberOfSections ( DWORD v )
	{
		// no need to check but daixuysnim 
		if ( _IsHeader64 )
			_Header64->FileHeader.NumberOfSections =(WORD)v; // 
		else
			_Header32->FileHeader.NumberOfSections = (WORD)v;
	}

	bool IsAddressCanExecute ( CVIRTUAL_ADDRESS va )
	{
		CSECTION * s = GetSectionByVA ( va );
		if ( s )
			if ( s->Record.Characteristics & IMAGE_SCN_MEM_EXECUTE && s->Record.Characteristics & IMAGE_SCN_CNT_CODE )
				return true;
		return false;
	}

} ;

string * ToString ( const wstring & s );
