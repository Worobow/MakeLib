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

class CIMPORTS 
{

public:
	
	struct IMPORT_DLL;

	struct IMPORT
	{
		
		private :
		bool _IsOrdinal;
		string *_FuncName;				// NULL if import only by ordinal

		public:
		ULONGLONG Ordinal;
		DWORD Num;						// IAT num
		vector <CINTERNAL_RELOC*> Refs;
		IMPORT_DLL * Dll;				// dll from where
		const char * GetFuncName ()
		{
			if ( _FuncName == NULL ){
				throw "dddd";
				return "Ordinal";
			}
			return _FuncName->data();
		}
		bool IsOrdinal ()
		{
			return _IsOrdinal;
		}
		void SetIsOrdinal ( bool v )
		{
			_IsOrdinal = v;
		}
		size_t GetFuncNameLenght ()
		{
			if ( _FuncName )
				return _FuncName->length();
			return 0;
		}
		void SetFuncName ( string * p )
		{
			_FuncName = p;
		}
		void AddRef ( CINTERNAL_RELOC * r )
		{
			Refs.push_back(r);
		}
		void AddRef ( vector <CINTERNAL_RELOC*> & rs )
		{
			for ( auto & r :rs ){
				AddRef (r);
			}
		}
		
	};
	struct IMPORT_DLL
	{
		string * DllName;						// dll name
		DWORD   TimeDateStamp;                  // 0 if not bound,
												// -1 if bound, and real date\time stamp
												//     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
												// O.W. date/time stamp of DLL bound to (Old BIND)
		DWORD   ForwarderChain;                 // -1 if no forwarders

		vector<IMPORT*> Funcs;					// funcs from this dll 

		IMPORT_DLL ( char * n, DWORD ts, DWORD fc )
			:DllName(new string(n)),TimeDateStamp(ts),ForwarderChain(fc)
		{}

		// internal funcs for work 
		void AddAllThunk64 ( CIMAGE * img, IMPORT_DLL * Dll, DWORD IntCh, DWORD IatCh, PIMAGE_DATA_DIRECTORY  iat, bool PrintDetailInfo ); 
		void AddAllThunk32 ( CIMAGE * img, IMPORT_DLL * Dll, DWORD IntCh, DWORD IatCh, PIMAGE_DATA_DIRECTORY  iat, bool PrintDetailInfo ); 

		void AddThunk64 ( CIMAGE * img, IMPORT_DLL * Dll, PIMAGE_THUNK_DATA64 ch, DWORD Num, bool PrintDetailInfo );
		void AddThunk32 ( CIMAGE * img, IMPORT_DLL * Dll, PIMAGE_THUNK_DATA32 ch, DWORD Num, bool PrintDetailInfo );

		// add
		void AddFunc ( string * Name, ULONGLONG org );
		void AddFunc ( IMPORT * f );
		// find 
		IMPORT * Find ( IMPORT * f );
	};

private:

	vector <IMPORT_DLL*> Imports;

	bool PrintAllThunk64 ( CIMAGE * img, DWORD ch ); 
	bool PrintAllThunk32 ( CIMAGE * img, DWORD ch ); 

	bool PrintThunk64 ( CIMAGE * img, PIMAGE_THUNK_DATA64 ch );
	bool PrintThunk32 ( CIMAGE * img, PIMAGE_THUNK_DATA32 ch );

	
public:
	bool Create ( CIMAGE * img, bool PrintDetailInfo );
	template<class IMAGE_THUNK_DATA_BITS> 	bool Pack ( CSECTION * sec, CIMAGE * img, bool PrintDebugDetails );
	bool PrintSource ( CIMAGE * img );

	size_t Size ()
	{
		return Imports.size();
	}
	IMPORT_DLL * Get ( size_t i )
	{
		return Imports[i];
	}
	const vector <IMPORT_DLL*> &GetAllImports(void)
	{
		return Imports;
	}
	CIMPORTS::IMPORT_DLL * Find ( CIMPORTS::IMPORT_DLL *n  )
	{
		for ( auto & i: Imports ){
			if ( *i->DllName == *n->DllName )
				return i;
		}
		return 0;
	}
	void AddDll ( CIMPORTS::IMPORT_DLL *n )
	{
		// search dll 

		 CIMPORTS::IMPORT_DLL * m= Find (n);
		 if ( m == NULL ){
			Imports.push_back(n);
			return;
		 }
		 else {
			 for ( auto &nf : n->Funcs ){
				 IMPORT * mf = m->Find ( nf  );
				 if ( mf == NULL )
					 m->AddFunc ( nf );
				 else
					 mf->AddRef ( nf->Refs );
			 }
		 }
	}
	void AddImport ( IMPORT_DLL * Dll )
	{
		Imports.push_back(Dll);
	}
	IMPORT * GetImportFuncByNum ( DWORD num )
	{
		for ( auto & dll: Imports ){
			for ( auto &i: dll->Funcs )
				if ( i->Num == num )
					return i;
		}
		throw "requared offset of IAT invalid.";
	}
	
	template <class T> void SetThunkOrdinal  ( T th, ULONGLONG Ordinal )
	{}

	template<> void SetThunkOrdinal <IMAGE_THUNK_DATA32> ( IMAGE_THUNK_DATA32 th, ULONGLONG Ordinal )
	{
		th.u1.Ordinal = (DWORD)Ordinal | IMAGE_ORDINAL_FLAG32; 
	}
	template<> void SetThunkOrdinal <IMAGE_THUNK_DATA64> ( IMAGE_THUNK_DATA64 th, ULONGLONG Ordinal )
	{
		th.u1.Ordinal = Ordinal | IMAGE_ORDINAL_FLAG64; 
	}

};

template<class IMAGE_THUNK_DATA_BITS>
bool CIMPORTS::Pack ( CSECTION * sec, CIMAGE * img, bool PrintDebugDetails )
{

	if ( Imports.size() == 0 )
		return true;

	BLOCK<IMAGE_IMPORT_DESCRIPTOR> * BlkImportDescriptors = new BLOCK<IMAGE_IMPORT_DESCRIPTOR> ( "Import Descriptors" );

	BlkImportDescriptors->Alloc ( Imports.size() + 1 );

	size_t ImportTableSize = 0;

	for ( auto dll: Imports ){
		for ( auto & func: dll->Funcs ){
	//		printf ( "\n%d %d %s %s" , ImportTableSize, func->Num, func->GetFuncName(), func->Dll->DllName->data() );
			func->Num =	(DWORD)ImportTableSize++; // record
		}
		ImportTableSize++;// zero
	} 

//	fflush ( stdout );
	
	// simple 32-bit null
	// this is only for init array zeros
	IMAGE_THUNK_DATA_BITS nill={0};

	BLOCK<IMAGE_THUNK_DATA_BITS> * BlkImportByNameTable = new BLOCK<IMAGE_THUNK_DATA_BITS> ( "Import By Name Table INT" );
	BlkImportByNameTable->Alloc (  ImportTableSize, nill );// array of IMAGE_THUNK_DATA32/64

	BLOCK<IMAGE_THUNK_DATA_BITS> * BlkImportByAddressTable = new BLOCK<IMAGE_THUNK_DATA_BITS> ("Import By address Table IAT");
	BlkImportByAddressTable->Alloc ( ImportTableSize, nill );// array of IMAGE_THUNK_DATA32/64

	vector <BLOCK<BYTE> *> DataBlocks;
	
	// go thru all import dlls
	for ( size_t i=0; i < Imports.size(); i++){

		// create new block for dll name 
		BLOCK<BYTE> * DllName = new BLOCK<BYTE> ( "Import dll name" );

		// allocate memory for dll name
		DllName->Alloc ( Imports[i]->DllName->length() + 1 );

		// copy name to new mem
		memcpy ( DllName->GetBuffer(), Imports[i]->DllName->data(), DllName->GetByteSize() );

		// memorize this block for later add
		DataBlocks.push_back (DllName);

		// set ref in import descriptor name field to this buffer 
		DllName->WriteRVA ( BlkImportDescriptors->CreateNewInternalRelocFromPtr ( &BlkImportDescriptors->At(i).Name, "BlkImportDescriptors[*].Name", img->GetImageBase() ), DllName->GetRelativeVirtialAddress() );
		
		vector<IMPORT*>::iterator fi = Imports[i]->Funcs.begin();
		IMPORT *f =  *fi;

		// set ref to first thunk of IAT and INT 
		BlkImportByNameTable->WriteRVA    ( BlkImportDescriptors->CreateNewInternalRelocFromPtr ( &BlkImportDescriptors->At(i).OriginalFirstThunk, "BlkImportDescriptors[*].OriginalFirstThunk", img->GetImageBase() ), BlkImportByNameTable->GetRelativeVirtialAddress(f->Num) );
		BlkImportByAddressTable->WriteRVA ( BlkImportDescriptors->CreateNewInternalRelocFromPtr ( &BlkImportDescriptors->At(i).FirstThunk,         "BlkImportDescriptors[*].FirstThunk",		  img->GetImageBase()  ), BlkImportByAddressTable->GetRelativeVirtialAddress(f->Num)  );

		// fill thuks for this imports in IAT and INT
		for ( ; fi != Imports[i]->Funcs.end(); fi++ ){
			
			f = *fi;

			for ( auto & r : f->Refs ){
				BlkImportByAddressTable->WriteRVA ( r, BlkImportByAddressTable->GetRelativeVirtialAddress ( f->Num ) ); // block yet not added - need use block for write value
			}

			if ( f->IsOrdinal() ){
				throw "don't check yet. Do it now.";

#if 1
				IMAGE_THUNK_DATA_BITS th;
				SetThunkOrdinal<IMAGE_THUNK_DATA_BITS> ( th, f->Ordinal );// need fix  -- bits depends IMAGE_ORDINAL_FLAG32 or IMAGE_ORDINAL_FLAG64
				BlkImportByAddressTable->At(f->Num) =  th ;
				BlkImportByNameTable->At(f->Num).u1.AddressOfData = 0;
#endif
			}
			else{
				// ;)))
				if ( ! f->IsOrdinal() ){

					// create block of bytes but not IMAGE_IMPORT_BY_NAME 
					// IMAGE_IMPORT_BY_NAME is not full struct and only header

					BLOCK<BYTE> * BlkFunc  = new BLOCK<BYTE> ("Import by name");

					BlkFunc->Alloc ( sizeof(IMAGE_IMPORT_BY_NAME) + f->GetFuncNameLenght() ) ;// with trailing zero because IMAGE_IMPORT_BY_NAME have one char into its name );
				
					PIMAGE_IMPORT_BY_NAME Func = (PIMAGE_IMPORT_BY_NAME)BlkFunc->GetBuffer();

					Func->Hint = (WORD)f->Ordinal;
					memcpy ( &Func->Name, f->GetFuncName(), f->GetFuncNameLenght() );
					Func->Name[f->GetFuncNameLenght()] = 0;

					DataBlocks.push_back(BlkFunc);

					BlkFunc->WriteRVA ( BlkImportByNameTable->CreateNewInternalRelocFromPtr    ( &BlkImportByNameTable->At   (f->Num), "INT->Func descr", img->GetImageBase() ) , BlkFunc->GetRelativeVirtialAddress() );
					BlkFunc->WriteRVA ( BlkImportByAddressTable->CreateNewInternalRelocFromPtr ( &BlkImportByAddressTable->At(f->Num), "IAT->Func descr", img->GetImageBase() ) , BlkFunc->GetRelativeVirtialAddress() );
				}
			}
		}
	}

	sec->AddBlock ( (BLOCK<BYTE>*) BlkImportByAddressTable, PrintDebugDetails );
	sec->AddBlock ( (BLOCK<BYTE>*) BlkImportByNameTable, PrintDebugDetails );

	sec->AddBlock ( (BLOCK<BYTE>*) BlkImportDescriptors, PrintDebugDetails );

	for ( auto b : DataBlocks )
		sec->AddBlock ( b, PrintDebugDetails );
	
	
	
	BlkImportDescriptors->WriteRVA ( new CINTERNAL_RVA_RELOC ( 0, img->GetImageBase(), &img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT)->VirtualAddress,  "IMAGE_DIRECTORY_ENTRY_IMPORT" ), BlkImportDescriptors->GetRelativeVirtialAddress());
	img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_IMPORT )->Size = BlkImportDescriptors->GetByteSize();

	BlkImportByAddressTable->WriteRVA ( new CINTERNAL_RVA_RELOC ( 0, img->GetImageBase(), &img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IAT)->VirtualAddress, "IMAGE_DIRECTORY_ENTRY_IAT" ), BlkImportByAddressTable->GetRelativeVirtialAddress() );
	img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IAT)->Size = BlkImportByAddressTable->GetByteSize();


	img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT)->VirtualAddress =0;
	img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT)->Size =0;

	img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT)->VirtualAddress =0;
	img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT)->Size =0;


	return true;
}