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

#include "stdafx.h"


// pack Relocs, allocate memory for new packed relocs and return it with its size
// 
// use virtual addres of block as VA for records
// 

bool CRELOCS::Pack ( CSECTION * sec, CIMAGE * img, bool PrintDebugDetails )
{
	printf("------------------ Pack Relocs -----------------------\n");
	
	if ( Relocs.size() == 0 )
		return true;

	sort ( Relocs.begin(), Relocs.end(), [] (UNPACKED_RELOC * const& a, UNPACKED_RELOC * const& b) { return a->Rva < b->Rva; } );

	CRELAITIVE_VIRTUAL_ADDRESS VA = 0;

	DWORD NeedSize=0;

	for ( auto & r : Relocs ){
		CRELAITIVE_VIRTUAL_ADDRESS CurVA = r->Rva & ~0xFFF;
		if ( VA != CurVA ){
			NeedSize+= sizeof(IMAGE_BASE_RELOCATION) + sizeof(PACKEDRELOC);
			VA = CurVA;
		}
		NeedSize += sizeof ( PACKEDRELOC );
	}


	if ( NeedSize == 0 ){
		return true;
	}

	BLOCK<BYTE> * Rels = new BLOCK<BYTE> ( "Relocs table" );
	
	Rels->Alloc ( NeedSize );
	
	PIMAGE_BASE_RELOCATION Head = (PIMAGE_BASE_RELOCATION) Rels->GetBuffer();

	Head->VirtualAddress =  CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( Relocs[0]->Rva & ~0xFFF );

	if ( PrintDebugDetails )
		printf("-----[%08X]----\n", Head->VirtualAddress );

	PWORD StartBlock = (PWORD)((char*)Head + sizeof(IMAGE_BASE_RELOCATION));
	
	PACKEDRELOC * CurRec = (PACKEDRELOC *)StartBlock;

	for ( auto & r : Relocs ){
		CRELAITIVE_VIRTUAL_ADDRESS CurVA =  ( r->Rva & ~0xFFF);// set VA

		if ( Head->VirtualAddress != CRELAITIVE_VIRTUAL_ADDRESS::Get32 (CurVA) ){
		
			CurRec->Offset = 0;
			CurRec->RelocType = IMAGE_REL_BASED_ABSOLUTE;
			CurRec++ ;
						
			Head->SizeOfBlock = (DWORD)((PWORD)CurRec - StartBlock) * sizeof(PACKEDRELOC) + sizeof(IMAGE_BASE_RELOCATION);

			Head = (PIMAGE_BASE_RELOCATION)CurRec;
			Head->VirtualAddress = CRELAITIVE_VIRTUAL_ADDRESS::Get32 (CurVA);
			
			if ( PrintDebugDetails )
				printf("-----[%08X]----\n", Head->VirtualAddress );

			StartBlock = (PWORD)((char*)Head + sizeof(IMAGE_BASE_RELOCATION));
			CurRec = (PACKEDRELOC *)StartBlock;
		}
		CurRec->Offset = CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( r->Rva & 0xFFF );
		CurRec->RelocType = r->Type;
		CurRec++ ;

		if ( PrintDebugDetails )
			printf("           %03X\t\t\t:%s\n", r->Rva.Get32() & 0xFFF, r->Description.data() );
	}
	
	CurRec->Offset = 0;
	CurRec->RelocType = IMAGE_REL_BASED_ABSOLUTE;
	CurRec++ ;

	Head->SizeOfBlock = (DWORD)((PWORD)CurRec - StartBlock) * sizeof(PACKEDRELOC) + sizeof(IMAGE_BASE_RELOCATION);

	sec->AddBlock ( Rels, PrintDebugDetails );

	// set size field 
	img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC)->Size = Rels->GetByteSize(); 

	// write value to field
	Rels->WriteRVA ( 
			new CINTERNAL_RVA_RELOC ( 0, img->GetImageBase(), &img->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC)->VirtualAddress,  "IMAGE_DIRECTORY_ENTRY_BASERELOC" ), 
			Rels->GetRelativeVirtialAddress() 
		); 

	printf("------------------ End Pack Relocs -------------------\n");

	return true;
}

// return VA to target on such reloc reference
// 
CVIRTUAL_ADDRESS CRELOCS::GetRelocTarget ( UNPACKED_RELOC * r, CIMAGE * img  )
{
	PBYTE ptr = (PBYTE)img->GetPtrFromRVA (r->Rva);

	switch ( r->Type ){
		
		case IMAGE_REL_BASED_ABSOLUTE :				break;
		case IMAGE_REL_BASED_HIGH:		
													throw "realocation not check yet. debug/check it yourself";
													return  CVIRTUAL_ADDRESS ( *(LONG UNALIGNED*) ptr ); 
													break;
		case IMAGE_REL_BASED_LOW:
													throw "realocation not check yet. debug/check it yourself";
													return CVIRTUAL_ADDRESS ( *(LONG UNALIGNED*) (ptr-2)  ); 
										
													break;
		case IMAGE_REL_BASED_HIGHLOW:
													return  CVIRTUAL_ADDRESS ( *(LONG UNALIGNED*) ptr ); 
		case IMAGE_REL_BASED_HIGHADJ:	 
													throw "This realocations aren't support";
													break;
		case IMAGE_REL_BASED_DIR64:
													return  CVIRTUAL_ADDRESS ( *(INT_PTR *)ptr ); 
													break;

		case IMAGE_REL_BASED_MACHINE_SPECIFIC_5:									//the same as : IMAGE_REL_BASED_MIPS_JMPADDR, IMAGE_REL_BASED_ARM_MOV32
		case IMAGE_REL_BASED_RESERVED:
		case IMAGE_REL_BASED_MACHINE_SPECIFIC_7:									//the same as : IMAGE_REL_BASED_THUMB_MOV32
		case IMAGE_REL_BASED_MACHINE_SPECIFIC_8:
		case IMAGE_REL_BASED_MACHINE_SPECIFIC_9:									//the same as: IMAGE_REL_BASED_IA64_IMM64, IMAGE_REL_BASED_MIPS_JMPADDR16
		default:
													printf("Rel type:%d\n",r->Type);
													throw "This realocation aren't support";
	}
	throw "The realocation isn't check yet. debug/check it yourself";
	return 0;
}

// build unpacked reloc array 
// this array may be packed 
//
bool CRELOCS::Build ( CIMAGE * img, bool PrintDetailInfo )
{

	printf("-------------------Build relocs ----------------------\n");

	// build relocs for all section
	//
	for ( auto & s: img->GetAllSection() ){
		s->BuildRelocs ( Relocs, img, PrintDetailInfo );
	}

	char RelocType;

	if ( img->IsImage64() )
		RelocType = IMAGE_REL_BASED_DIR64;
	else
		RelocType = IMAGE_REL_BASED_HIGHLOW;

	// build relocs that point to target outsize raw data of sections
	//
	for (  auto &r: img->GetVARelocs() ){
		if ( r->GetType() == CINTERNAL_RELOC::FULL_VA ){
			CRELAITIVE_VIRTUAL_ADDRESS rva = img->GetRvaFromPtr ( r->GetPtr() );
			UNPACKED_RELOC * n = new UNPACKED_RELOC ( RelocType, rva, r->GetDescription() );
			Relocs.push_back (n);
			if ( PrintDetailInfo )
				printf ( "%03d VA:0x%016I64X 0x%016I64X %s\n", (int)(Relocs.size()-1) , r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );
		}
	}

	// build relocs for import functions
	// 
	for ( auto & dll : img->GetAllImports() ){
		for ( auto & i: dll->Funcs ){
			for ( auto & r : i->Refs ){
				if (  r->GetType() == CINTERNAL_RELOC::FULL_VA ){
					CRELAITIVE_VIRTUAL_ADDRESS rva = img->GetRvaFromPtr ( r->GetPtr() );
					UNPACKED_RELOC * n = new UNPACKED_RELOC(RelocType, rva,r->GetDescription());
					Relocs.push_back (n);
					if ( PrintDetailInfo )
						printf ( "%03d VA:0x%016I64X 0x%016I64X %s\n", (int)(Relocs.size()-1),  r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );
				}
			}
		}
	}

	printf("------------------ End Build relocs ------------------\n");
	
	return true;
}
bool CRELOCS::Create ( CIMAGE * img, bool PrintDetailInfo )
{
	 
	// load relocs from image to array of unpack Relocs. This is internal functon. For use it when func 'Load' worked 
	//
	PIMAGE_DATA_DIRECTORY dd = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_BASERELOC );

	if ( dd->VirtualAddress == 0 )
		return true;

	PIMAGE_BASE_RELOCATION  RelocTable;  
	
	RelocTable = (PIMAGE_BASE_RELOCATION) img->GetPtrFromRVA ( dd->VirtualAddress );

	PIMAGE_BASE_RELOCATION  RelocTableMax =  (PIMAGE_BASE_RELOCATION)((char*)RelocTable + dd->Size); 

	while ( RelocTable->VirtualAddress != 0 && RelocTable < RelocTableMax) {

		size_t RelocsCount = ( RelocTable->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION) )/ sizeof(PACKEDRELOC);
		
		PACKEDRELOC * Reloc = (PACKEDRELOC*)((PCHAR)RelocTable + sizeof(IMAGE_BASE_RELOCATION) );

		for (size_t i=0;i<RelocsCount;i++){
			if ( Reloc[i].RelocType != IMAGE_REL_BASED_ABSOLUTE ){
				UNPACKED_RELOC * r= new UNPACKED_RELOC ( Reloc[i].RelocType, Reloc[i].Offset + RelocTable->VirtualAddress, "Reloc from reloc table" );

				Relocs.push_back(r);
			}
		}
		RelocTable = (PIMAGE_BASE_RELOCATION)((char*)RelocTable + RelocTable->SizeOfBlock);
	};

	// change reference to IAT
	//
	PIMAGE_DATA_DIRECTORY iat = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_IAT );

	printf("-------------------- Load relocs.--------------------\n");

	int RelocNumber=0; // for debug only - use in description

	char RelocNumberStr[32];

	for ( auto & r: Relocs ){

		_itoa_s ( RelocNumber, RelocNumberStr, sizeof(RelocNumberStr), 10 );

		// throw on error
		 void * ptr = img->GetPtrFromRVA ( r->Rva ); // 0x1003000

		 CVIRTUAL_ADDRESS Target = GetRelocTarget ( r, img ) ;			// 0x3000
		 CVIRTUAL_ADDRESS Source = img->GetVaFromRVA ( r->Rva );

		 CSECTION *TargetSec = img->GetSectionByVA  ( Target );	
		 CSECTION *SourceSec = img->GetSectionByVA ( Source );

		 // a reloc have two points 
		 // 1 ) "source" reference, this point is address where some reference to tagrget
		 // 2)  "target" reference, this point is address on such source reference

		 // We have sections such have can to move, move it means to change its virtual address.
		 // And when virtual address to changes, then we must change all that points such target to address in this section.
		 // Because this, we will add to sections all such reference who target to it. 
		 // And when section to move at anouther address, we go thru all reference list and change 
		 // value of memory to new target address at this references.

		 // The Section have two kind of reference on it. First RVA ( relative virtual address ) and second is the VA (real virtual address)
		 // The Relocs is direct virtual address. 

		 // Exist two kind if reference direct and indirect reference
		 // indirect reference is reference via tables such locate in datas

		 // if target reference to IAT 
		 // then find import in image and add relocs to it

		CINTERNAL_VA_RELOC * r = new CINTERNAL_VA_RELOC ( Source, img->GetImageBase(), ptr, img->GetRelocSize(), "" );
		SourceSec->AddMyVAReloc ( r );

		CRELAITIVE_VIRTUAL_ADDRESS TargetRva = img->GetRvaFromVA (Target);
		CRELAITIVE_VIRTUAL_ADDRESS IatRva = iat->VirtualAddress;

		 if ( TargetRva >= IatRva && TargetRva < IatRva + iat->Size ){
//			if ( ! ptr )
//				throw "ref to import function outsize of any sections";

			CIMPORTS::IMPORT * f =  img->GetImportFuncByIatOffset ( CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( TargetRva - IatRva) );

			f->AddRef ( r );
			string d("Reference to fununction via IAT, from :");
			r->SetDescription ( d + img->Description + "@" + (const char * )SourceSec->Record.Name  + ",  (#" + RelocNumberStr + ")"  );
			if ( PrintDetailInfo )
				printf ( "%s:0x%016I64X(0x%016I64X) -> %s:0x%08x import func %s::%s (descr:%s)\n", SourceSec->Record.Name, Source.Get64(), Target.Get64() ,TargetSec->Record.Name, TargetRva.Get32(), f->Dll->DllName->data(), f->GetFuncName(),r->GetDescription().data() );
			
		 }
		 else { 
			 if ( TargetSec ){
				TargetSec->WriteVA ( r, r->GetTargetPoint() );
				string d ( "Reference to ");
				r->SetDescription ( d + (const char * )TargetSec->Record.Name + ", from " + img->Description + "@" + (const char * )SourceSec->Record.Name  + ",  (#" + RelocNumberStr + ")"   );
				if ( PrintDetailInfo )
					printf("%s:0x%016I64X(0x%016I64X) -> %s:0x%08X, Descr:%s\n", SourceSec->Record.Name, Source.Get64(), Target.Get64(), TargetSec->Record.Name, TargetRva.Get32(), r->GetDescription().data() );
				
			 }
			 else{
				img->WriteVA ( r, r->GetTargetPoint() );
				r->SetDescription ( "Reference outsize any sectons, from:" + img->Description + "@" + (const char * )SourceSec->Record.Name  + ",  (#" + RelocNumberStr + ")"   );
				if ( PrintDetailInfo )
					printf("%s:0x%016I64X(0x%016I64X) -> (img):0x%08X Descr:%s\n", SourceSec->Record.Name, Source.Get64(),  Target.Get64(), TargetRva.Get32(), r->GetDescription().data() );
			 }
		 }

		 RelocNumber++;
	}

	printf("-------------------- End Load relocs.--------------------\n");

	return true;
}