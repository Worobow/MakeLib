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

		CSections.h, CSections.cpp

	Environment:

		User-mode command line

	Comment:

		class for the implementation of functional work image sections 

--*/

class CIMAGE;
class CSECTION;

#define HARD_CHECK_RVA false

template <class T>
class BLOCK {
	
public:

	// info for debug purpouse only
	// comment it or use define 
	
	string Desc;					// description of block. For debug purpouse only
	CSECTION * MySection;			// for debug purpouse only

private:

	vector <CPDB::CSECTION*>	ObjSections;
	vector <CPDB::CSYMBOL*>		ObjSymbols;

	map <CRELAITIVE_VIRTUAL_ADDRESS, CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY*> FuncsTable;

	map<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*> MyVARelocs;//
	vector <CINTERNAL_RELOC*> VARelocs;			//
	vector<T> Buffer; 
	CRELAITIVE_VIRTUAL_ADDRESS VirtialAddress;	// VA this block

public:

	const map <CRELAITIVE_VIRTUAL_ADDRESS, CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY*> & GetFuncsTable ( void )
	{
		return FuncsTable;
	}
	CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY* GetFuncsTableEntry ( CRELAITIVE_VIRTUAL_ADDRESS rva )
	{
		auto e = FuncsTable.find ( rva );

		if ( e == FuncsTable.end() )
			return 0;

		return e->second;
	}
	void AddFuncsTableEntry ( CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY * NewEntry )
	{
		FuncsTable.insert ( pair<CRELAITIVE_VIRTUAL_ADDRESS, CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY *>( NewEntry->BeginAddress, NewEntry ) );
	}
	map<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*> & GetMyVARelocs()
	{
		return MyVARelocs;
	}
	vector <CINTERNAL_RELOC*> & GetVARelocs()
	{
		return VARelocs;
	}
	CPDB::CSYMBOL* GetSymbolByName ( wstring name )
	{
		for ( auto & s : ObjSymbols ){
			wchar_t udn[265];
			if ( ! UnDecorateSymbolNameW ( s->Name.data(), udn, sizeof(udn)/sizeof(udn[0]) , UNDNAME_NAME_ONLY ) )
				continue;
			if ( ! wcscmp( udn, name.data() ) )
				return s;
		}
		return 0;
	}
	void AddPdbSection ( CPDB::CSECTION * s )
	{
		ObjSections.push_back ( s );
	}
	void AddPdbSymbol ( CPDB::CSYMBOL* s )
	{
		ObjSymbols.push_back (s);
	}
	void BuildRelocs ( vector<CRELOCS::UNPACKED_RELOC*> & rels, CIMAGE * Img, bool PrintDetailInfo );
	DWORD GetByteSize ()
	{
		return (DWORD)( Buffer.size() * sizeof(T) );
	}
	CRELAITIVE_VIRTUAL_ADDRESS GetRelativeVirtialAddress ( size_t i=0 )
	{
		return VirtialAddress + (DWORD)( sizeof(T)*i );
	}
	void Print ( const char * pr )
	{
		printf ( "%s -- %s 0x%08X, size: 0x%08X\n", pr, Desc.data(), VirtialAddress.Get32(), GetByteSize()  );
	}
	T * GetBuffer ( void )
	{
		return Buffer.data();
	}
	size_t GetSize (void )
	{
		return Buffer.size();
	}
	bool IsPtrInsize ( void * ptr )
	{
		if ( ptr >= &*Buffer.begin() )
			if ( ptr <= &*Buffer.rbegin() )
				return true;
		return false;
	}
	//bool IsVAInsize ( DWORD va ); // <-------------------- this function for block not exist, use images function for this purpose
	bool IsRVAInsize ( CRELAITIVE_VIRTUAL_ADDRESS rva )
	{
		if ( rva >= VirtialAddress && rva < VirtialAddress + GetByteSize() )
			return true;

		return false;
	}
	//TODO: change PBYTE to void * and remove anywhere in code where occurs typecast
	size_t GetByteOffset ( PBYTE Ptr )
	{
		return Ptr - (PBYTE)Buffer.data();
	}
	void * GetPtrFromRva ( CRELAITIVE_VIRTUAL_ADDRESS va ) // was T
	{
		CRELAITIVE_VIRTUAL_ADDRESS ia = va - VirtialAddress;
		if ( ia >= GetByteSize() )
			return NULL;
		return (void*) ( (PBYTE)Buffer.data() + CRELAITIVE_VIRTUAL_ADDRESS::Get32(ia) );
	}
	//TODO: change type T to void * , and remove in code implicied conversion
	CRELAITIVE_VIRTUAL_ADDRESS GetRvaFromPtr ( void * ptr )
	{
		PBYTE s = (PBYTE)Buffer.data();
		PBYTE e = s + GetByteSize();

		if ( (PBYTE)ptr < s || (PBYTE)ptr >= e )
			throw "ptr not my";

		size_t Offset = (PBYTE)ptr - (PBYTE)Buffer.data();

		return  VirtialAddress + (DWORD)Offset ;
	}
	CINTERNAL_RELOC * FindInternalRelocInMyVARelocs ( CVIRTUAL_ADDRESS va, CVIRTUAL_ADDRESS base )
	{
		CRELAITIVE_VIRTUAL_ADDRESS rva = va - base;

		if ( ! IsRVAInsize ( rva ) )
			throw "rva invalid";

		auto rr = MyVARelocs.find( va );
		if ( rr == MyVARelocs.end() )
			return 0;
		return rr->second;
	}
	CINTERNAL_RVA_RELOC * CreateNewInternalRelocFromPtr ( void * ptr, const string desc, CVIRTUAL_ADDRESS Base )
	{
		return new CINTERNAL_RVA_RELOC ( GetRvaFromPtr((T*)ptr) , Base, ptr, desc );
	}
	void Alloc ( size_t s )
	{
		if ( Buffer.size() != 0 )
			throw "Buffer already allocated";

		Buffer.resize(s);
	}
	void Alloc ( size_t s , T init  )
	{
		if ( Buffer.size() != 0 )
			throw "Buffer already allocated";

		Buffer.resize(s, init );
	}

	//
	// ATTENTION !
	//
	// This function allocates memory and copies the parameter pointer into this memory.
	// To get the pointer to the new buffer using any way of receiv address.
	// This function does not set geting pointer but only copies its contents!!!
	void SetBuffer ( T * buf,  size_t size, CRELAITIVE_VIRTUAL_ADDRESS VirtAdr )
	{
		if ( buf == NULL )
			throw "SetBuffer buffer is NULL";

		Buffer.resize ( size );

		memcpy ( Buffer.data(), buf, sizeof(T)* size );

		if ( size == 0 )
			throw "SetBuffer fuffer size is 0";

		VirtialAddress = VirtAdr;

	
	}
	// 
	// This function allocate memory for block, and initialize it. 
	// unlike SetBuffer function, this only use for allocate empty memory, for uninitialized sections usualy.
	//
	void AllocBuffer( size_t size, CRELAITIVE_VIRTUAL_ADDRESS VirtAdr)
	{
		Buffer.resize(size,0);

		if (size == 0)
			throw "SetBuffer fuffer size is 0";

		VirtialAddress = VirtAdr;

	}


	T& At ( size_t i )
	{
		return Buffer[i];
	}
	
	void WriteRVA ( CINTERNAL_RELOC * r, CRELAITIVE_VIRTUAL_ADDRESS target )// write value of VA on ptr and add ptr to reloc array
	{
		// check whether it's my address.
		if ( ! IsRVAInsize( target ) ){
			//throw "WriteRVA target invalid"; // 
			
			CRELAITIVE_VIRTUAL_ADDRESS RvaMax    = VirtialAddress + GetByteSize() -1;
			bool DoWarning = false;

			if ( target > RvaMax ){
				if ( HARD_CHECK_RVA || target > RvaMax + 1 )
					DoWarning = true;
			}
			else
				DoWarning = true;

			if ( DoWarning )
				printf("\nWARNING: WriteRVA target 0x%08X out of block:\"%s\", 0x%08X:0x%08X.(it posible, it's not error.)\n", target.Get32(), Desc.data(), VirtialAddress.Get32(), (VirtialAddress + GetByteSize()).Get32()  );
		}

		// check whether it's already here.

		r->SetTargetPoint ( target ); // *ptr = target;

		VARelocs.push_back( r );//
#ifdef _MAX_DEBUG_PRINT
		const char * sn="null";
		if ( MySection )
			sn = (const char *)MySection->Record.Name;

		printf ( "Write RVA    Sec:<<%8.8s>> Block:<<%s>>. Add RVA ref[%d] :\"%s\"\n", sn, Desc.data(), VARelocs.size() - 1, r->GetDescription().data() );
#endif
	}
	// write va value such target to me into memory
	// and add reference to my in my reference array (VARelocs)
	void WriteVA ( CINTERNAL_RELOC * r, CVIRTUAL_ADDRESS target )// write value of VA on ptr and add ptr to reloc array
	{
		
		// check whether it's my address.
		if ( ! IsRVAInsize ( r->GetRvaFromVa ( target ) ) ){
		//	throw "WriteRVA target invalid"; // 

			CRELAITIVE_VIRTUAL_ADDRESS RvaTarget = r->GetRvaFromVa ( target ) ;
			CRELAITIVE_VIRTUAL_ADDRESS RvaMax    = VirtialAddress + GetByteSize() -1;

			bool DoWarning = false;

			if ( RvaTarget > RvaMax ){
				if ( HARD_CHECK_RVA || RvaTarget > RvaMax + 1 )
					DoWarning = true;
			}
			else
				DoWarning = true;

			if ( DoWarning )
				printf("\nWARNING: WriteRVA target 0x%08X out of block:\"%s\", 0x%08X:0x%08X.(it posible, it's not error.)\n", target.Get32(), Desc.data(), VirtialAddress.Get32(), (VirtialAddress + GetByteSize()).Get32() );
		}

		r->SetTargetPoint ( target ); // *ptr = target;

		VARelocs.push_back( r );

#ifdef _MAX_DEBUG_PRINT
		const char * sn="null";
		if ( MySection )
			sn = (const char *)MySection->Record.Name;

		printf("Write  VA    Sec:<<%8.8s>> Block:<<%s>>. Add VA ref [%d] :\"%s\"\n", sn, Desc.data(), VARelocs.size()-1, r->GetDescription().data() );
#endif
	}

	// inverse operation for WriteVa 
	// remove 
	vector <CINTERNAL_RELOC*>::iterator RemoveVAReferece ( CINTERNAL_RELOC * r, CVIRTUAL_ADDRESS base )
	{
		CRELAITIVE_VIRTUAL_ADDRESS rva = r->GetTargetPoint() - base;

		// check whether it's my address.
		if ( ! IsRVAInsize ( rva ) ){
		//	throw "VA target invalid"; //

			CRELAITIVE_VIRTUAL_ADDRESS RvaTarget = r->GetTargetPointRva(); //r->GetRvaFromVa ( target ) ;
			CRELAITIVE_VIRTUAL_ADDRESS RvaMax    = VirtialAddress + GetByteSize() -1;

			bool DoWarning = false;

			if ( RvaTarget > RvaMax ){
				if ( HARD_CHECK_RVA || RvaTarget > RvaMax + 1 )
					DoWarning = true;
			}
			else
				DoWarning = true;

			if ( DoWarning )
				printf("\nWARNING: RemoveVAReferece 0x%16I64X from block:\"%s\", 0x%08X:0x%08X.(it posible, it's not error.)\n", r->GetTargetPoint().Get64(), Desc.data(), VirtialAddress.Get32(), (VirtialAddress + GetByteSize()).Get32() );

		}
		
		auto item = std::find ( VARelocs.begin(), VARelocs.end(), r );

		if ( item == VARelocs.end() )
			throw "reloc not found";

		return VARelocs.erase ( item );
	}
	void SetBase ( CVIRTUAL_ADDRESS b )
	{
		for ( auto &r:VARelocs ){
			r->SetBase ( b );
		}

		for ( auto & r: MyVARelocs ){
			r.second->SetBase ( b );
		}
	}

	void ChangeVirtualAddress ( CRELAITIVE_VIRTUAL_ADDRESS NewRVa, bool PrintDebugDetails ) // move block in virtual address space 
	{
		const char * ImgDesc = "null";
		const char * SecDescr = "null";

		CIMAGE * img = NULL;
		
		if ( MySection ){
			SecDescr = (const char*)MySection->Record.Name;
			img = MySection->MyImage;
			if ( img )
				ImgDesc = img->Description.data();
		}
		printf ( "Change Virtual Address for block <<%s>> (sec:<<%s>>,img<<%s>>),  was 0x%08x new 0x%08x\n", Desc.data(), SecDescr, ImgDesc, VirtialAddress.Get32(), NewRVa.Get32() );

		CRELAITIVE_VIRTUAL_ADDRESS delta = NewRVa -  VirtialAddress ;

		for ( auto &r:VARelocs ){
			if ( PrintDebugDetails )
				printf ( "%s Change 0x%p(0x%016I64X) --> was %016I64X new %016I64X\n", r->GetDescription().data(), r->GetPtr(), r->GetSourcePoint().Get64(),  r->GetTargetPoint().Get64(),  (r->GetTargetPoint() + delta).Get64() );

			r->SetTargetPoint ( r->GetTargetPoint() + delta );
		}
		for ( auto & r: MyVARelocs ){
			auto ri = r.second;
			if ( PrintDebugDetails )
				printf ( "%s Change 0x%p(was 0x%016I64X new 0x%016I64X) --> %016I64X\n", ri->GetDescription().data(), ri->GetPtr(), ri->GetSourcePoint().Get64(), (ri->GetSourcePoint() + delta).Get64(), ri->GetTargetPoint().Get64() );
			ri->SetSourcePoint ( ri->GetSourcePoint() + delta );
		}

		VirtialAddress = NewRVa;
	}
	void AddMyVAReloc ( CINTERNAL_RELOC * r )
	{
		if ( IsPtrInsize ( r->GetPtr() ) ){

			CVIRTUAL_ADDRESS Va = r->GetSourcePoint();
			if ( MyVARelocs.find ( Va ) != MyVARelocs.end() )
				throw "multiplay add";
			
			MyVARelocs.insert ( pair<CVIRTUAL_ADDRESS, CINTERNAL_RELOC*>( Va, r ) );
			
			return;
		}
		throw "no my ptr";
	}
	map<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*>::iterator RemoveMyVAReloc ( CINTERNAL_RELOC * r  )
	{
		if ( IsPtrInsize ( r->GetPtr() ) ){

			auto item = MyVARelocs.find ( r->GetSourcePoint() );

			if ( item == MyVARelocs.end() )
				throw "reloc not found";

			
			return MyVARelocs.erase ( item );
		}
		throw "no my ptr";
	}
	void AddMyRIPReloc ( CINTERNAL_RIP_RELOC * r )
	{
		if ( IsPtrInsize ( r->GetPtr() ) ){

			CVIRTUAL_ADDRESS Va = r->GetSourcePoint();  
			if ( MyRIPRelocs.find ( Va ) != MyRIPRelocs.end() )
				throw "multiplay add";
			
			MyRIPRelocs.insert ( pair<CVIRTUAL_ADDRESS, CINTERNAL_RIP_RELOC*>( Va, r ) );
			
			return;
		}
		throw "no my ptr";
	}
	void RemoveMyRIPReloc ( CINTERNAL_RIP_RELOC * r  )
	{
		if ( IsPtrInsize ( r->GetPtr() ) ){

			auto item = MyRIPRelocs.find ( r->GetSourcePoint() );

			if ( item == MyRIPRelocs.end() )
				throw "reloc not found";

			MyRIPRelocs.erase ( item );
			return;
		}
		throw "no my ptr";
	}
	BLOCK(string d)
		: VirtialAddress(0), Desc(d), MySection(0)
	{}
};


// Section is container for any data. 
// Image is container for sections. 
// Sections may remove and add to the container.
// Section is fragment of virtual address space 
// Align between fragmets is SectionAlign in header
//
class CSECTION
{
	vector <CPDB::CSECTION*> ObjSections;
	vector <CPDB::CSYMBOL*> ObjSymbols;

	map<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*>  MyVARelocs;//
	vector<CINTERNAL_RELOC*> VARelocs;//

	map <CRELAITIVE_VIRTUAL_ADDRESS, CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY*> FuncsTable;

	string Name;

public:

	// only for debug
	CIMAGE * MyImage;
	vector <BLOCK<BYTE>*> _Data;
	IMAGE_SECTION_HEADER Record;

	const map<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*>  & GetMyVARelocs()
	{
		return MyVARelocs;
	}
	CPDB::CSYMBOL* GetSymbolByName ( wstring name )
	{
		for ( auto & s : ObjSymbols ){
			if ( ! s->Name.compare ( name) )
				return s;
		}

		for ( auto & b: _Data ){
			CPDB::CSYMBOL* r = b->GetSymbolByName ( name );
			if ( r )
				return r;
		}

		return 0;
	}

	// create section from pointer to section content and section header
	// usualy use when load image
	CSECTION ( PIMAGE_SECTION_HEADER pSectionHdr, BLOCK<BYTE> * Ptr, CIMAGE * img  );

 	// create section from componets
	CSECTION ( const char _Name[IMAGE_SIZEOF_SHORT_NAME], DWORD Flg );

	void AddPdbSection ( CPDB::CSECTION * s )
	{
		ObjSections.push_back ( s );
	}
	void AddPdbSymbol ( CPDB::CSYMBOL* s )
	{
		ObjSymbols.push_back (s);
	}
	const string & GetName ( )
	{
		return Name;
	}

	void SetName ( const string & n )
	{
		if ( n.length() > IMAGE_SIZEOF_SHORT_NAME )
			throw "too much";
		memset ( Record.Name,0, IMAGE_SIZEOF_SHORT_NAME );
		memcpy ( Record.Name, n.data(), n.length() );
		Name = n;
	}
 
	// add new block of data to secion 
	// change section size
	// setup virtual address for block
	template <class T>void AddBlock ( BLOCK<T> * blk, bool PrintDebugDetails )
	{

		BLOCK<T> * ob = GetBlockByPtr (blk->GetBuffer());

		if ( ob != NULL )
			throw "block already in section.";

		string c ( "Add block to secion :" );
		c += (char*)Record.Name;

		blk->Print(c.data());

		blk->MySection = this;

		_Data.push_back ( blk );
		
		blk->ChangeVirtualAddress ( Record.VirtualAddress + Record.Misc.VirtualSize, PrintDebugDetails );

		Record.Misc.VirtualSize += blk->GetByteSize();//BufferSize;
	}
	void SetBase ( CVIRTUAL_ADDRESS b );
	//TODO: Убрать Record в закрытое место и добавить GetName и GetRelativeVirtualAddress
	void ChangeVirtualAddress ( CRELAITIVE_VIRTUAL_ADDRESS va, bool PrintDebugDetails );
	void WriteRVA ( CINTERNAL_RELOC * r, CRELAITIVE_VIRTUAL_ADDRESS target_rva )//
	{
		// search my block by ref target
		BLOCK<BYTE> * b = GetBlockByRVA ( target_rva );
		if ( b != NULL ){
			b->WriteRVA ( r, target_rva );
		}
		else{
			r->SetTargetPoint ( target_rva );
			
			VARelocs.push_back( r );
#ifdef _MAX_DEBUG_PRINT
			printf("Write RVA(s) Sec:<<%s>> outsize all blocks. Add RVA ref[%d] :\"%s\"\n", Record.Name, VARelocs.size()-1, r->GetDescription().data() );
#endif
		}
	}
	CINTERNAL_RELOC * FindInternalRelocInMyVARelocs ( CVIRTUAL_ADDRESS va, CVIRTUAL_ADDRESS base )
	{
		CRELAITIVE_VIRTUAL_ADDRESS rva = va - base;

		BLOCK<BYTE> * b = GetBlockByRVA ( rva );
		if ( b )			
			return b->FindInternalRelocInMyVARelocs ( va, base );

		auto rr = MyVARelocs.find ( va );
		if ( rr == MyVARelocs.end() )
			return 0;

		return rr->second;
	}
	void WriteVA ( CINTERNAL_RELOC * r, CVIRTUAL_ADDRESS target )//
	{
		CRELAITIVE_VIRTUAL_ADDRESS rva = r->GetRvaFromVa (target);

		// search my block by ref target 
		BLOCK<BYTE> * b = GetBlockByRVA ( rva );
		if ( b != NULL ){
			b->WriteVA ( r, target );
		}
		else{

			r->SetTargetPoint (target);

			VARelocs.push_back( r );

#ifdef _MAX_DEBUG_PRINT
			printf("Write  VA(s) Sec:<<%s>> outsize all blocks. Add RVA ref[%d] :\"%s\"\n", Record.Name, VARelocs.size()-1, r->GetDescription().data() );
#endif
		}
	}
	void RemoveVAReferece ( CINTERNAL_RELOC * r, CVIRTUAL_ADDRESS base )
	{
		CRELAITIVE_VIRTUAL_ADDRESS rva = r->GetTargetPoint() - base;

		BLOCK<BYTE> * b = GetBlockByRVA ( rva );
		if ( b != NULL ){
			b->RemoveVAReferece ( r, base );
			return;
		}

		// check whether it's my address.
		if ( ! IsRVAInsize ( rva ) )
			throw "VA target invalid"; //
		
		auto item = std::find( VARelocs.begin(), VARelocs.end(), r);

		if ( item == VARelocs.end() )
			throw "reloc not found";

		VARelocs.erase ( item );
	}
	void RemoveMyVAReloc ( CINTERNAL_RELOC * r  )
	{

		BLOCK<BYTE> * b = GetBlockByPtr ( r->GetPtr() );
		if ( b != NULL ){
			b->RemoveMyVAReloc (r);
			return;
		}
		auto item =  MyVARelocs.find ( r->GetSourcePoint() );
		if ( item == MyVARelocs.end() )
			throw "reloc not found";
		MyVARelocs.erase ( item );
	}
	void AddMyVAReloc ( CINTERNAL_RELOC * r )
	{
		BLOCK<BYTE> * b = GetBlockByPtr ( r->GetPtr() );
		if ( b != NULL ){
			b->AddMyVAReloc ( r );
			return;
		}

		if ( MyVARelocs.find ( r->GetSourcePoint() ) != MyVARelocs.end() )
			throw "multi reloc";

		MyVARelocs.insert ( pair<CVIRTUAL_ADDRESS,CINTERNAL_RELOC*> ( r->GetSourcePoint(), r ) );
	}
	CRELAITIVE_VIRTUAL_ADDRESS GetRVAFromPtr ( void * ptr )
	{
		BLOCK<BYTE> * b = GetBlockByPtr ( ptr );
		if ( b )
			return b->GetRvaFromPtr ( ptr );

		throw "no my ptr";
	}
#if 0
	// 
	CINTERNAL_RVA_RELOC * CreateNewInternalRelocFromPtr (PBYTE ptr, const string desc )
	{
		BLOCK<BYTE> * b = GetBlockByPtr ( ptr );
		if ( b )
			return b->CreateNewInternalRelocFromPtr ( ptr, desc );

		throw "pointer outside of datas";
	}
#endif
	void BuildRelocs ( vector<CRELOCS::UNPACKED_RELOC*> & rels, CIMAGE * Img, bool PrintDetailInfo );
	// check range of address use header but not blocks 
	bool IsRVAInsize ( CRELAITIVE_VIRTUAL_ADDRESS va )
	{
		// crazy MS linker sucks 
		DWORD ma = max ( Record.SizeOfRawData, Record.Misc.VirtualSize);
		// in section record sored relaitive VA
		if ( va >= CRELAITIVE_VIRTUAL_ADDRESS ( Record.VirtualAddress ) && va < CRELAITIVE_VIRTUAL_ADDRESS ( Record.VirtualAddress + ma ) )
			return true;
#if 0
		for ( auto & b : _Data )
			if ( b->IsVirtualAddressInsize(va) )
				return true;
#endif
		return false;
	}

	// this function for section not exist!
	// use images function for this
	//BLOCK<BYTE>* GetBlockByRVA ( DWORD va )
	BLOCK<BYTE>* GetBlockByRVA ( CRELAITIVE_VIRTUAL_ADDRESS va )
	{
		for ( auto b : _Data ){
			if ( b->IsRVAInsize (va) )
				return b;
		}
		return NULL;
	}
	BLOCK<BYTE>* GetBlockByPtr ( void * ptr )
	{
		for ( auto b : _Data ){
			if ( b->IsPtrInsize (ptr) )
				return b;
		}
		return NULL;
	}

	
};
