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

		CRelocs.h, CRelocs.cpp

	Environment:

		User-mode command line

	Comment:

		class for the implementation of functional work image realocations 

--*/

class CIMAGE ;
class CSECTION;

class CINTERNAL_RELOC
{
public:

	enum RELOC_TYPES
	{
		FULL_VA,
		RIP,
		RVA
	};

private:	

	CVIRTUAL_ADDRESS			Base;							// This is the image base, RIP reloc has special PlaceBase 

	const RELOC_TYPES			Type;							// type of this reloc, 

	const DWORD 				Size;							// size of field where store or value displacement or value of target address
																// use on operation write
																// or read

	string						Description;					// for debug

	DWORD						Tag;							// Tag marker for serching relocs in an arrays.

	void*						Pointer;						// pointer to dword or dword64 where storage virtual address value (see val there).
																// On case if this pointer target to no initialized memory, then this field may be equale zero.
																// On this case use field of val. 
protected:

	CVIRTUAL_ADDRESS			SourcePointVirtualAddress;		// Equivalent of ptr. This field is equivalent of ptr in virtual address mode.
																// only one case use - when not memory where storage value of va.
	CVIRTUAL_ADDRESS			TargetPointVirtualAddress;		// 
	
	CRELAITIVE_VIRTUAL_ADDRESS  Displacement;					// distance between source and target points

	void Write ( DWORD64 Value ) 
	{
		switch ( Size )
		{
		case 1:		*((PBYTE)Pointer)		= (BYTE)Value;		return;
		case 2:		*((PWORD)Pointer)		= (WORD)Value;		return;
		case 3:		throw "size left!!";						
		case 4:		*((PDWORD)Pointer)		= (DWORD)Value;		return;
		case 6:		throw "size left!!";						
		case 7:		throw "size left!!";						
		case 8:		*((PDWORD64)Pointer)	= (DWORD64)Value;	return;
		default:	throw "size left!!";						
		}
	}
	DWORD64  Read ( void )
	{
		switch ( Size )
		{
		case 1:		return (__int64) *((char*)Pointer);
		case 2:		return (__int64) *((short*)Pointer);
		case 3:		throw "size left!!";						
		case 4:		return (__int64) *((long*)Pointer);
		case 6:		throw "size left!!";						
		case 7:		throw "size left!!";						
		case 8:		return (__int64) *((__int64*)Pointer);
		default:	throw "size left!!";						
		}
	}


public:

	CINTERNAL_RELOC ( CVIRTUAL_ADDRESS sp, CVIRTUAL_ADDRESS b, void * p, DWORD size, const string & descr, RELOC_TYPES ty )
		:SourcePointVirtualAddress(sp), Base(b), Pointer(p), Description(descr), Tag(0), Size(size), Type(ty)
	{
		
	}

	CINTERNAL_RELOC ( CVIRTUAL_ADDRESS sp, CVIRTUAL_ADDRESS b, void * p,  DWORD size, const char * descr, RELOC_TYPES ty )
		:CINTERNAL_RELOC (sp,b,p,size,string(descr),ty)	{}

	bool operator==(CINTERNAL_RELOC const& i ) const
	{
		return  i.Pointer == Pointer;
	};
	RELOC_TYPES GetType ()
	{
		return Type;
	}
	DWORD GetTag ( void ) const
	{
		return Tag;
	}
	void SetTag ( DWORD nt )
	{
		Tag = nt;
	}
	DWORD GetSize (void ) const 
	{
		return Size;
	}
	const string & GetDescription ( void ) const
	{
		return Description;
	}
	void SetDescription ( const string s )
	{
		Description = s;
	}
	void * GetPtr ( void ) const
	{
		return Pointer;
	}
	void SetPtr ( void * p )
	{
		Pointer = p;
	}
	CRELAITIVE_VIRTUAL_ADDRESS GetSourcePointRva ( void ) const 
	{
		return SourcePointVirtualAddress - GetBase();
	}
	CRELAITIVE_VIRTUAL_ADDRESS GetTargetPointRva ( void ) const 
	{
		return TargetPointVirtualAddress - GetBase();
	}
	CRELAITIVE_VIRTUAL_ADDRESS GetRvaFromVa ( CVIRTUAL_ADDRESS a ) const 
	{
		return a - GetBase();
	}
	CVIRTUAL_ADDRESS GetBase ( void ) const
	{
		return Base;
	}
	void SetBase ( CVIRTUAL_ADDRESS b )
	{
		Base = b;
	}
	CVIRTUAL_ADDRESS GetSourcePoint ( void ) const 
	{
		return SourcePointVirtualAddress;
	}
	virtual void SetSourcePoint ( CVIRTUAL_ADDRESS a ) 
	{
		throw "need overr";
	}
	CVIRTUAL_ADDRESS GetTargetPoint ( void ) const 
	{
		return TargetPointVirtualAddress;
	}
	virtual void SetTargetPoint ( CVIRTUAL_ADDRESS a )
	{
		throw "need overr";
	}
	virtual CINTERNAL_RELOC * CreateNew (  CVIRTUAL_ADDRESS b )
	{
		throw "need overr";
	}
	void SetSourcePoint ( CRELAITIVE_VIRTUAL_ADDRESS a ) 
	{
		SetSourcePoint ( GetBase() + a );
	}
	void SetTargetPoint ( CRELAITIVE_VIRTUAL_ADDRESS a ) 
	{
		SetTargetPoint ( GetBase() + a );
	}
};

class CINTERNAL_VA_RELOC : public CINTERNAL_RELOC
{
public:
	CINTERNAL_VA_RELOC ( CVIRTUAL_ADDRESS sp, CVIRTUAL_ADDRESS b, void * p, DWORD size, const string & descr )
		:CINTERNAL_RELOC(sp,b,p,size,descr, CINTERNAL_RELOC::FULL_VA)
	{
		TargetPointVirtualAddress = Read (); 
		Displacement = TargetPointVirtualAddress - SourcePointVirtualAddress;// no needs for this reloc type
	}
	void SetSourcePoint ( CVIRTUAL_ADDRESS a ) override final
	{
		SourcePointVirtualAddress = a;
		Displacement = TargetPointVirtualAddress - SourcePointVirtualAddress;
	}
	void SetTargetPoint ( CVIRTUAL_ADDRESS a ) override final
	{
		Write(CVIRTUAL_ADDRESS::Get64(a));
		TargetPointVirtualAddress = a;
		Displacement = TargetPointVirtualAddress - SourcePointVirtualAddress;
	}
	CINTERNAL_RELOC * CreateNew (  CVIRTUAL_ADDRESS b  ) override final
	{
		if ( GetType() != CINTERNAL_RELOC::FULL_VA )
			throw "invalid old";
		CINTERNAL_RELOC * ret = new CINTERNAL_VA_RELOC ( *this );
		
		ret->SetBase ( b );
		ret->SetTargetPoint ( GetTargetPointRva () );
		ret->SetSourcePoint ( GetSourcePointRva () );

		return ret;
	}
};

// Relocs exist in several forms. Depends from sorts of its addressings.
// 1. reloc for relative virtual address
// 2. reloc for full virtual address
// 3. reloc for RIP addressing.

// each reloc have field size where store size of value in image


class CINTERNAL_RIP_RELOC : public CINTERNAL_RELOC
{
	CVIRTUAL_ADDRESS PlaceBase;

public:

	CINTERNAL_RIP_RELOC ( CVIRTUAL_ADDRESS sp, CVIRTUAL_ADDRESS b, CVIRTUAL_ADDRESS plb, void * p,  DWORD size, const string & descr )
		:CINTERNAL_RELOC(sp,b,p,size,descr,CINTERNAL_RELOC::RIP), PlaceBase(plb)
	{
		Displacement =  (DWORD)Read();
		TargetPointVirtualAddress = PlaceBase + Displacement;
	}
	void SetSourcePoint ( CVIRTUAL_ADDRESS a ) override final
	{

		CRELAITIVE_VIRTUAL_ADDRESS delt = a - SourcePointVirtualAddress;// was  SourcePointVirtualAddress- a;

		PlaceBase = PlaceBase + delt;

		SourcePointVirtualAddress = SourcePointVirtualAddress + delt;

		Displacement = TargetPointVirtualAddress - PlaceBase;

		Write(CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( Displacement ));
	}
	void SetTargetPoint ( CVIRTUAL_ADDRESS a ) override final
	{
		TargetPointVirtualAddress = a;
		Displacement = TargetPointVirtualAddress - PlaceBase;
		Write(CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( Displacement ));
	}
	CINTERNAL_RELOC * CreateNew ( CVIRTUAL_ADDRESS b  ) override final
	{
		if ( GetType() != CINTERNAL_RELOC::RIP )
			throw "invalid old";

		CINTERNAL_RELOC * ret = new CINTERNAL_RIP_RELOC ( *this );

		ret->SetBase(b);

		CRELAITIVE_VIRTUAL_ADDRESS delt = GetBase() - PlaceBase;

		PlaceBase = b + delt;

		TargetPointVirtualAddress = PlaceBase + Displacement;

		delt = PlaceBase - SourcePointVirtualAddress;

		SourcePointVirtualAddress = PlaceBase + delt;

		return ret;
	}
};

class CINTERNAL_RVA_RELOC : public CINTERNAL_RELOC
{

public:

	CINTERNAL_RVA_RELOC ( CRELAITIVE_VIRTUAL_ADDRESS sp, CVIRTUAL_ADDRESS b, void * p, const string & descr )
		:CINTERNAL_RELOC ( 0, b, p, sizeof(CRELAITIVE_VIRTUAL_ADDRESS), descr, CINTERNAL_RELOC::RVA )
	{
		SourcePointVirtualAddress = GetBase() + sp;
		TargetPointVirtualAddress = (DWORD)Read();
		Displacement = TargetPointVirtualAddress - GetBase();  
	}
	CINTERNAL_RVA_RELOC (  CRELAITIVE_VIRTUAL_ADDRESS sp, CVIRTUAL_ADDRESS b, void * p, const char * descr )
		:CINTERNAL_RVA_RELOC(sp,b,p,string(descr))
	{
		
	}
	void SetSourcePoint ( CVIRTUAL_ADDRESS a ) override final // never used
	{
		SourcePointVirtualAddress = a;
	}
	// use when VA change
	void SetTargetPoint ( CVIRTUAL_ADDRESS a  )  override final
	{
		TargetPointVirtualAddress = a;
		Displacement = TargetPointVirtualAddress - GetBase(); 
		Write(CRELAITIVE_VIRTUAL_ADDRESS::Get32(Displacement));
	}
	CINTERNAL_RELOC * CreateNew ( CVIRTUAL_ADDRESS b ) override final
	{
		if ( GetType() != CINTERNAL_RELOC::RVA )
			throw "invalid old";
	
		CINTERNAL_RELOC *ret = new CINTERNAL_RVA_RELOC ( *this );

		ret->SetBase(b);
		ret->SetTargetPoint ( GetTargetPointRva () );
		ret->SetSourcePoint ( GetSourcePointRva () );

		return ret;
	}
};


class CRELOCS 
{

public:

	struct UNPACKED_RELOC	
	{
		string Description;
		BYTE Type;
		CRELAITIVE_VIRTUAL_ADDRESS Rva;
		UNPACKED_RELOC ( char t, CRELAITIVE_VIRTUAL_ADDRESS rva, const char * descr )
			:Type(t),Rva(rva),Description(descr)
		{}
		UNPACKED_RELOC ( char t, CRELAITIVE_VIRTUAL_ADDRESS rva, const string& descr )
			:Type(t),Rva(rva),Description(descr)
		{}
	};

private:

	vector<UNPACKED_RELOC*> Relocs;
	CVIRTUAL_ADDRESS GetRelocTarget ( UNPACKED_RELOC * ptr, CIMAGE * img  );

public:
	
	struct PACKEDRELOC {
		WORD Offset:12;
		WORD RelocType:4;
	};
	size_t Size()
	{
		return Relocs.size();
	}
	const vector<UNPACKED_RELOC*> GetAllRelocs ( void )
	{
		return Relocs;
	}
	bool Build ( CIMAGE * img, bool PrintDetailInfo );// build unpacked from section and image use its VARelocs 
	bool Create ( CIMAGE * img, bool PrintDetailInfo );// read from bin image. Unpack.
	bool Pack ( CSECTION * Sec, CIMAGE * img, bool PrintDetailInfo ); // pack Relocs, allocate memory for new packed relocs and return it with its size
};