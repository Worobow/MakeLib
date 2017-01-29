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

#include "stdafx.h"

// 
// create section 
// use usually when create section from of exist images
CSECTION::CSECTION ( PIMAGE_SECTION_HEADER pSectionHdr, BLOCK<BYTE> * Ptr, CIMAGE * img  )
{
		
	if ( pSectionHdr->PointerToRelocations != 0 ||  pSectionHdr->PointerToLinenumbers != 0 
		 || pSectionHdr->NumberOfRelocations != 0 ||  pSectionHdr->NumberOfLinenumbers != 0 ){
		// throw from constructor :) but it's not a comercial software
			throw "We don't know as work with this image"; // do it all youself.
	}

	MyImage = img;

	// we store raw header
	Record = *pSectionHdr;

	Ptr->MySection = this;

	_Data.push_back ( Ptr ); // storage of contens the section

	Name = (const char *)Record.Name;
	if ( Name.size() > IMAGE_SIZEOF_SHORT_NAME )
		Name.resize(IMAGE_SIZEOF_SHORT_NAME);
}
//
// create new empty section
CSECTION::CSECTION ( const char _Name[IMAGE_SIZEOF_SHORT_NAME], DWORD Flg )
{

	memcpy ( &Record.Name, _Name, sizeof(Record.Name) );
	Record.Characteristics = Flg;

	Record.PointerToRelocations =0;
    Record.PointerToLinenumbers =0;
    Record.NumberOfRelocations  =0;
    Record.NumberOfLinenumbers  =0;

	Record.VirtualAddress = 0; 

	Record.Misc.VirtualSize  = 0  ;// actual size
	Record.SizeOfRawData = 0; // SizeOfRawData;

	Record.PointerToRawData =0;

	MyImage = 0;

	Name = _Name;

}
void CSECTION::SetBase ( CVIRTUAL_ADDRESS b )
{
	for ( auto & bls: _Data ){
		bls->SetBase(b);
	}

	for ( auto &r:VARelocs ){
		r->SetBase(b);
	}
	for ( auto & r: MyVARelocs ){
		r.second->SetBase(b);
	}
}
//
// Move section into virtual address space
void CSECTION::ChangeVirtualAddress ( CRELAITIVE_VIRTUAL_ADDRESS va, bool PrintDebugDetails )
{

	CRELAITIVE_VIRTUAL_ADDRESS delta = va - CRELAITIVE_VIRTUAL_ADDRESS ( Record.VirtualAddress );
	if ( delta == 0 ){
		return ;
	}

	const char * ImgDesc="null";
	if ( MyImage ){
		ImgDesc = MyImage->Description.data();
	}

	printf ( "Change Virtual Address for sec <<%8s>> (<<%s>>), was 0x%08X new 0x%08X\n", Record.Name, ImgDesc, Record.VirtualAddress, va.Get32() );
		
	for ( auto b : _Data ){
		CRELAITIVE_VIRTUAL_ADDRESS offset = b->GetRelativeVirtialAddress() - CRELAITIVE_VIRTUAL_ADDRESS(Record.VirtualAddress) ;
		b->ChangeVirtualAddress ( va + offset, PrintDebugDetails  );
	}

	for ( auto & r: VARelocs ){
		if ( PrintDebugDetails )
			printf ( "%s Change 0x%p(0x%016I64X) --> was %016I64X new %016I64X\n", r->GetDescription().data(), r->GetPtr(), r->GetSourcePoint().Get64(),  r->GetTargetPoint().Get64(),  (r->GetTargetPoint() + delta).Get64() );
		r->SetTargetPoint ( r->GetTargetPoint() + delta );
	}

	for ( auto & r: MyVARelocs ){
		auto ri = r.second;
		if ( PrintDebugDetails )
			printf ( "%s Change 0x%p(was 0x%016I64X new 0x%016I64X) --> %016I64X\n", ri->GetDescription().data(), ri->GetPtr(), ri->GetSourcePoint().Get64(), (ri->GetSourcePoint() + delta).Get64() , ri->GetTargetPoint().Get64() );
		ri->SetSourcePoint ( ri->GetSourcePoint() + delta );
	}

	Record.VirtualAddress = CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( va );
}
// Convert from internal relocs to unpacked relocs such can be packed for save into image file
// 
void CSECTION::BuildRelocs ( vector<CRELOCS::UNPACKED_RELOC*> & rels, CIMAGE * img, bool PrintDetailInfo )
{
	for ( auto b : _Data ){
		b->BuildRelocs ( rels, img, PrintDetailInfo );
	}

	char RelocType;

	if ( img->IsImage64() )
		RelocType = IMAGE_REL_BASED_DIR64;
	else
		RelocType = IMAGE_REL_BASED_HIGHLOW;

	for ( auto &r: VARelocs ){
		if ( r->GetType() == CINTERNAL_RELOC::FULL_VA ){ // only normal relocs will be upload
			CVIRTUAL_ADDRESS va = r->GetSourcePoint();  
			CRELOCS::UNPACKED_RELOC * n = new CRELOCS::UNPACKED_RELOC ( RelocType, img->GetRvaFromVA(va), r->GetDescription() );
			rels.push_back (n);
			if ( PrintDetailInfo )
				printf ( "%03d VA:0x%016I64X 0x%016I64X %s\n", (int)(rels.size()-1), r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );
		}
	}
}
template <class T> void BLOCK<T>::BuildRelocs ( vector<CRELOCS::UNPACKED_RELOC*> & rels, CIMAGE * img, bool PrintDetailInfo )
{
	char RelocType;

	if ( img->IsImage64() )
		RelocType = IMAGE_REL_BASED_DIR64;
	else
		RelocType = IMAGE_REL_BASED_HIGHLOW;

	for ( auto &r: VARelocs ){
		if ( r->GetType() == CINTERNAL_RELOC::FULL_VA ){
			CRELOCS::UNPACKED_RELOC * n = new CRELOCS::UNPACKED_RELOC ( RelocType, img->GetRvaFromVA ( r->GetSourcePoint() ), r->GetDescription() );
			rels.push_back (n);
			if ( PrintDetailInfo )
				printf ( "%03d VA:0x%016I64X 0x%016I64X %s\n", (int)(rels.size()-1), r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );
		}
	}
}
