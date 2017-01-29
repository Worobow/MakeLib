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
			
			ImageMerge2.cpp

		Environment:
			
			User-mode command line

		Comment:

			The main module of the program to inject code into the base code.

--*/


#include "stdafx.h"

// global definition

template<class T>
bool DoExternLinking ( CIMAGE * ejimg, CIMAGE * target  )
{
	printf ("---------Resolve name -------\n");

	CSECTION * ExbldrSec = ejimg->GetSection (".exbldr.");

	if ( ! ExbldrSec )
		return true;
	
	CPDB::CSYMBOL TargetFuncByAddr(wstring(L""),wstring(L""),0,0,0,0,0,0,true,true);

	// through all blocks
	for ( auto &b :ExbldrSec->_Data ){

		T * ex = (T *)b->GetBuffer();
		size_t exsize = b->GetByteSize() / sizeof(T);
		
		printf   ( "#   Seg:Address  Lenght   Name\n" );
		printf   ( "--- ---:---------- ---------- --\n" );
		
		for ( size_t i=0; i < exsize; i++ ){
			
			if ( ex[i].Action != hook && ex[i].Action != call )
				continue;

			CPDB::CSYMBOL* TargetFunc;
			const wchar_t * _TargetName = 0;

			if ( ex[i].OrgName ){

				_TargetName = (const wchar_t *) ejimg->GetPtrFromVA ( (DWORD) ex[i].OrgName );

				if ( !_TargetName ){
					wprintf ( L"Can't get name . Error\n" );
					return false;
				}

				TargetFunc = target->GetFunctionByName ( _TargetName );
				if ( !TargetFunc ){
					wprintf ( L"<<%s>> function in base image was not found.\n", _TargetName );
					return false;
				}
			}
			else{
				TargetFunc = &TargetFuncByAddr;
				TargetFunc->RVA = CRELAITIVE_VIRTUAL_ADDRESS((DWORD)ex[i].Org);
				TargetFunc->Lenght = ex[i].Lenght;
				_TargetName = L"**Function by Address**";
			}

			const wstring TargetName = _TargetName;

			wprintf ( L"%03zd %03d:0x%08X 0x%08x %s\n", i, TargetFunc->Seg, TargetFunc->RVA.Get32(), (DWORD)TargetFunc->Lenght, TargetName.data() );

			CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY * FuncTableEntry;

			FuncTableEntry = CFUNCSTABLE::GetFunctionByVA ( target, TargetFunc->RVA ) ;

			if ( FuncTableEntry ){

				if ( FuncTableEntry->BeginAddress != TargetFunc->RVA  ){
					wprintf( L"<<%s>> Function BeginAddress from PDB is not as function table.\n", TargetName.data());
					return false;
				}

				if ( FuncTableEntry->Length != (DWORD)TargetFunc->Lenght ){
					wprintf( L"<<%s>> Function EndAddress from PDB is not as function table.\n", TargetName.data());
				}
			}

			CVIRTUAL_ADDRESS TargetFuncVa = target->GetVaFromRVA ( TargetFunc->RVA );

			string * aTargetName = ToString ( TargetName );

			//CINTERNAL_RELOC * nr = ExbldrSec->CreateNewInternalRelocFromPtr ( &ex[i].Org, ".exbldr.:" + *aTargetName );

			CRELAITIVE_VIRTUAL_ADDRESS rva = ExbldrSec->GetRVAFromPtr ( &ex[i].Org );
			CVIRTUAL_ADDRESS va = target->GetVaFromRVA ( rva );

			CINTERNAL_VA_RELOC * nr = new CINTERNAL_VA_RELOC ( va, target->GetImageBase(), &ex[i].Org, target->GetRelocSize(), ".exbldr.Org:" + *aTargetName );

			delete aTargetName;

			nr->SetTag ( NOT_COMPLETE_RELOC_MARKER ); // my tag

			ExbldrSec->AddMyVAReloc (nr);

			//ex[i].Org = TargetFuncVa;
			//nr->Write ( TargetFuncVa );
			nr->SetTargetPoint ( TargetFuncVa );
			ex[i].Lenght = (DWORD)TargetFunc->Lenght;
		}
		printf ( "\nTotal:%d\n", (int)exsize );

	}
	printf ("---------End Resolve name -------\n");
	return true;
}

bool IsInsize ( CVIRTUAL_ADDRESS a, CVIRTUAL_ADDRESS s, CVIRTUAL_ADDRESS e )
{
	if ( a >= s && a <= e )
		return true;

	return false;

}
 
// set hook to function
// and move old code to new place
// never to call moved code in this place
// will be bsod
// I can do so that would be moved code to work properly, but not in this life, and not for these poor money
template<class T>
bool MoveFunction ( CIMAGE * img, const T * ex, bool PrintDetailInfo )
{

	printf ( "Move function :%S, 0x%016I64X, 0x%016I64X  \n", (const wchar_t*)img->GetPtrFromVA (  ex->OrgName ), (__int64)ex->Org, (__int64)ex->Org + ex->Lenght );

	BLOCK<BYTE> * HookedBlock = img->GetBlockByRVA ( img->GetRvaFromVA ( CVIRTUAL_ADDRESS( ex->Hook ) ) );

	CVIRTUAL_ADDRESS Start = ex->Org;
	DWORD Lenght = ex->Lenght ;

	// 1) create new block for func
	// 2) copy body into new block
	// 3) add block to image
	// 4) move (remove and add) all relocs to new place 
	// 5) fill old place zeros
	
	string BlockName = "Block for Save org func:";

	if ( ex->OrgName ){
		string * exName = ToString ( wstring ( (const wchar_t *) img->GetPtrFromVA ( CVIRTUAL_ADDRESS ( ex->OrgName ) ) ) );

		BlockName += *exName;
	}
	else{
		BlockName += string ("**Function by Address**");
	}

	BLOCK<BYTE> * TargetNewBodyBlock = new BLOCK<BYTE>( BlockName );
			
	// get old place of func
	PBYTE TargetOldPlace = (PBYTE)img->GetPtrFromVA ( Start );		// (DWORD)ex[i].Org 

	if ( ! TargetOldPlace )
		return false;

	// copy content to new place
	TargetNewBodyBlock->SetBuffer (  TargetOldPlace, Lenght , 0 );	//ex[i].Lenght
				
	// get func section
	CSECTION * TargetTextSection = img->GetSection ( ".orgfun." );	// ".text" 
	if ( ! TargetTextSection )
		return false;

	// add new buffer to section
	TargetTextSection->AddBlock ( TargetNewBodyBlock, PrintDetailInfo );

	// get new ptr
	PBYTE TargetNewPlace =TargetNewBodyBlock->GetBuffer();

	//get old place block
	BLOCK<BYTE> * TargetOldBodyBlock = img->GetBlockByPtr ( TargetOldPlace );

	// get va of new place
	CVIRTUAL_ADDRESS TargetNewVA =  img->GetVaFromRVA ( img->GetRvaFromPtr ( TargetNewPlace ) ); 

	// get va of old place
	CVIRTUAL_ADDRESS TargetOldVA = img->GetVaFromRVA ( img->GetRvaFromPtr( TargetOldPlace ) ); 

	if ( TargetOldVA != Start )							//(DWORD)ex[i].Org
		throw "debug error";

	CRELAITIVE_VIRTUAL_ADDRESS va_delt = TargetNewVA - TargetOldVA;

	CVIRTUAL_ADDRESS VAStart = Start;					//(DWORD)ex[i].Org
	CVIRTUAL_ADDRESS VAEnd	 = Start + (Lenght -1);		//(DWORD)ex[i].Org + ex[i].Lenght;

	CRELAITIVE_VIRTUAL_ADDRESS RVAStart = img->GetRvaFromVA ( VAStart );
	CRELAITIVE_VIRTUAL_ADDRESS RVAEnd   = img->GetRvaFromVA ( VAEnd );


	// search all references such target to moved function start and move its to new block
	//
	
	printf("Searching references to moved function...\n");

	auto ri = TargetOldBodyBlock->GetVARelocs().begin();

	while (  ri != TargetOldBodyBlock->GetVARelocs().end() ){

		CINTERNAL_RELOC * r = *ri;
		ri++ ;

		// modify relocs where target inside of body

		if ( IsInsize ( r->GetTargetPoint(), VAStart, VAEnd ) ){

			// check if source inside then skip this reloc
		 

			if ( r->GetTargetPoint() != VAStart && r->GetTargetPoint() != VAEnd && ! IsInsize ( r->GetSourcePoint(), VAStart, VAEnd ) ){
				printf("WARNING: Seems you are move a not function code, because i see references (va:%016I64X)->(va:%016I64X) inside to midle of this code\n",  r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64() );
			}

			if ( r->GetTargetPoint() == VAStart && ( r->GetTag() & (FUNCS_TABLE_REFERENCE | EXBLDR_ORG_REFERENCE) ) == 0 ){
				// not from funcs table and not from exb loader
				// 1) get hooked func block
				// 2) if exist block of this relocs is different with hooked block
				// 3) then move this reloc from one section to anouther
				// 4) change target point of this reloc to hooked funcs

				if ( TargetOldBodyBlock == HookedBlock ){ // if mystic happened, then simply set target to hook
					r->SetTargetPoint ( CVIRTUAL_ADDRESS(ex->Hook) );
				}
				else{ 
					// usually everything goes as usual.
					// remove ref from old block 

					ri = TargetOldBodyBlock->RemoveVAReferece ( r, img->GetImageBase() );
					printf("remove reloc p:%ph va:%016I64X val:%016I64X %s\n", r->GetPtr(), r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );
					// and move it at new hooked block, and setup by the way target to hook.
					HookedBlock->WriteVA ( r, CVIRTUAL_ADDRESS(ex->Hook) );
					printf("new    reloc p:%ph va:%016I64X val:%016I64X %s\n", r->GetPtr(), r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );
					continue;
				}
				printf("Done.\n");
			}
			else {
				ri= TargetOldBodyBlock->RemoveVAReferece ( r, img->GetImageBase() );
				printf("remove reloc p:%ph va:%016I64X val:%016I64X %s\n", r->GetPtr(), r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );

				TargetNewBodyBlock->WriteVA ( r,  r->GetTargetPoint() + va_delt );
				printf("new    reloc p:%ph va:%016I64X val:%016I64X %s\n", r->GetPtr(), r->GetSourcePoint().Get64(), r->GetTargetPoint().Get64(), r->GetDescription().data() );
				continue;
			}
		}
	}

	printf("Done.\n");
	
	PBYTE PtrStart = TargetOldPlace;
	PBYTE PtrEnd = PtrStart + Lenght;//ex[i].Lenght

	printf("Modify references from moved function...\n");

	auto rj = TargetOldBodyBlock->GetMyVARelocs().begin();

	while ( rj != TargetOldBodyBlock->GetMyVARelocs().end() ){

		CINTERNAL_RELOC * r = rj->second;
		rj++;

		if ( IsInsize ( r->GetSourcePoint(), VAStart, VAEnd ) ) {

			// modify relocs where source point inside body 

			CRELAITIVE_VIRTUAL_ADDRESS shift = (PBYTE)r->GetPtr() - TargetOldPlace;
			PDWORD NewPtr = PDWORD ( TargetNewPlace + CRELAITIVE_VIRTUAL_ADDRESS::Get32(shift) );
			CVIRTUAL_ADDRESS NewVa = TargetNewVA + shift;
			
			rj = TargetOldBodyBlock->RemoveMyVAReloc ( r );

			printf ("Remove reloc ptr:%p(%016I64X), VA:%016I64X, %s from old function place.\n", r->GetPtr(), r->GetTargetPoint().Get64(), r->GetSourcePoint().Get64(), r->GetDescription().data() );

			r->SetPtr ( NewPtr );
	 
			r->SetSourcePoint ( NewVa );
			 
			TargetNewBodyBlock->AddMyVAReloc ( r );

			printf("new    reloc ptr:%p(%016I64X), VA:%016I64X, %s to new function place.\n",r->GetPtr(), r->GetTargetPoint().Get64(), r->GetSourcePoint().Get64(), r->GetDescription().data() );
			continue;
		}

	
	}
	printf("\n");

	memset ( TargetOldPlace, 0, Lenght );


	// need add code jmp ds:[table.hook] 
	// because in extern link table already exist full set of relocs ( and myrelocs and ref relocs ) 
	// thet no need add any one new else
	// need only add reloc for new generaged code.
	// 

	BLOCK<BYTE> * TableBlock = img->GetBlockByPtr ( (PBYTE)&ex->Org );

	CVIRTUAL_ADDRESS TableVA = img->GetVaFromRVA ( TableBlock->GetRvaFromPtr ( (PBYTE) &ex->Hook ) );

	TargetOldPlace[0] = 0xFF;
	TargetOldPlace[1] = 0x25;

	if ( img->IsImage64() ){
		CINTERNAL_RIP_RELOC * nr = img->CreateNewInternalRipRelocFromPtr ( &TargetOldPlace[2], 4 , "jmp qword ptr cs:[table.hook]" );
		TableBlock->WriteVA ( nr, TableVA );
		TargetOldBodyBlock->AddMyVAReloc ( nr );
	}
	else {
		CINTERNAL_VA_RELOC * nr = img->CreateNewInternalVaRelocFromPtr  ( &TargetOldPlace[2], "jmp dword ptr ds:[table.hook]" );
		TableBlock->WriteVA ( nr, TableVA );
		TargetOldBodyBlock->AddMyVAReloc ( nr );
	}

	return true;
}
template<class T>
bool DoExternLinking2 ( CIMAGE * img, bool PrintDetailInfo )
{
	bool ret = true;

	CSECTION * ExbldrSec = img->GetSection (".exbldr.");

	if ( ! ExbldrSec )
		return ret;

	// do second stage of link

	for ( auto & b : ExbldrSec->_Data ){
		for ( auto & r : b->GetMyVARelocs() ){
			auto ri = r.second;
			if ( ri->GetTag() == NOT_COMPLETE_RELOC_MARKER ){
				ri->SetTag ( EXBLDR_ORG_REFERENCE ); // remove marker
				img->WriteVA ( ri, ri->GetTargetPoint() );
			}
		}
	}

	// add special section
	 
	CSECTION * ns = new CSECTION ( ".orgfun.", IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE );

	img->AddSection ( ns, PrintDetailInfo );

	// all blocks
	for ( auto &b :ExbldrSec->_Data ){
		T * ex = (T *)b->GetBuffer();
		size_t exsize = b->GetByteSize() / sizeof(T);
		for ( size_t i=0; i < exsize; i++ ){

			// only for hooks
			if ( ex[i].Action != hook )
				continue;

			if ( ! MoveFunction<T> ( img, &ex[i], PrintDetailInfo ) ){
				ret = false;
				break;
			}
		}
	}

	ns->Record.SizeOfRawData = img->FileAlignment ( ns->Record.Misc.VirtualSize );

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// main route of the demo
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Merge two images into one 
//
int 
ImageMerge ( 
	wchar_t * BaseImageName,		// file name of base image
	wchar_t * BaseImagePdbName, 
	wchar_t * EjectImageName,		// image path. Code from this image insert into base
	wchar_t * EjectImagePdbName,	// image pdb.
	wchar_t * OutImageName, 
	bool PrintDetailInfo			// produce more detail info to screen
)
{
	// Open two images
	// the first image is that such inserted into tagret
	// the second is a target

	CIMAGE BaseImage;
	CIMAGE EjectImage;

	wprintf( L"Try base opening image %s\n", BaseImageName );

	DWORD Status;

//	bool PrintDetailInfo = false; // set true for more detail

	if ( ! BaseImage.OpenExist ( BaseImageName, &Status, string("BaseImage"), PrintDetailInfo ) ){
		printf("Open base image failed: %d\n", Status );
		return -1;
	}

	if ( ! BaseImage.LoadPdb(BaseImagePdbName,PrintDetailInfo) ){
		printf("Open pbd base image failed: %d\n", Status );
		return -1;
	}

	if ( ! BaseImage.MakeCodeAnalize ( PrintDetailInfo ) ){
		printf("Code analize base image failed: %d\n", Status );
		return -1;
	}

	wprintf( L"Try eject opening image %s\n", EjectImageName );

	if ( ! EjectImage.OpenExist ( EjectImageName, &Status, string("EjectImage"), PrintDetailInfo ) ){
		printf("Open eject image failed: %d\n", Status );
		return -1;
	}

	if ( ! EjectImage.LoadPdb ( EjectImagePdbName, PrintDetailInfo) ){
		printf("Open pbd base image failed: %d\n", Status );
		return -1;
	}
	
	if ( ! EjectImage.MakeCodeAnalize ( PrintDetailInfo ) ){
		printf("Code analize base image failed: %d\n", Status );
		return -1;
	}

	if ( BaseImage.IsImage64() != EjectImage.IsImage64()  ){
		printf("Base image and eject have different type (64/32). ");
		return -1;
	}

	wprintf( L"Succesful\n");

	void * pSupperInitEntry = 0;

	CPDB::CSYMBOL*  SupperInitEntry =  EjectImage.GetFunctionByName ( L"SuperInit" );

	if ( SupperInitEntry ){
		CRELAITIVE_VIRTUAL_ADDRESS RVaSupperInitEntry = SupperInitEntry->RVA;
		pSupperInitEntry = EjectImage.GetPtrFromRVA (RVaSupperInitEntry);
	}

	wprintf (L"Create new image\n");

// create new 
	CIMAGE NewImage;
	NewImage.CreateNew ( BaseImage.GetFileImage(), NULL , string("outImage") );
	wprintf( L"Succesful\n");

	wprintf( L"Resolve names in exbldr tables\n" );

	if ( ! EjectImage.IsImage64() ){
		if (! DoExternLinking<EXTERN_HOOK32> ( &EjectImage, & BaseImage ) ){ // resolve names in tables
			printf("Link fail.\n" );
			return -1;
		}
	}
	else{
		if (! DoExternLinking<EXTERN_HOOK64> ( &EjectImage, & BaseImage ) ){ // resolve names in tables
			printf("Link fail.\n" );
			return -1;
		}
	}

	wprintf( L"Succesful\n");

	wprintf( L"Add all section from Base image.\n");
	// take all setions

	const vector<CSECTION*> & BaseImageSecions =  BaseImage.GetAllSection();

	// start virtual address
	DWORD va = BaseImageSecions[0]->Record.VirtualAddress;

	// add all sections from base image to new
	// skip  reloc and exbldr, if last exist it's in realy already error :)
	// clear PointerToRawData fields that the system do recalc positions 
	for ( auto & s: BaseImageSecions ){

		if ( ! strncmp (".reloc", (const char*)s->Record.Name, IMAGE_SIZEOF_SHORT_NAME ) )
			continue;

		if ( ! strncmp (".exbldr.", (const char*)s->Record.Name, IMAGE_SIZEOF_SHORT_NAME ) )
			continue;

		s->Record.PointerToRawData=0;
	
		s->ChangeVirtualAddress (va, PrintDetailInfo );

		NewImage.AddSection ( s, PrintDetailInfo );

		va += s->Record.Misc.VirtualSize;
		va = NewImage.SectionAlignment (va);
	}

	wprintf( L"Succesful\n");
	wprintf( L"Add all section from Eject image.\n");

	// add contents of sections of this image to the appropriate sections of the new image.
	// also skeep reloc and store exbldr

	DWORD StartEjectImage = va;

	void * HeaderEntryPoint = EjectImage.GetPtrFromRVA (EjectImage.GetEntryPoint());

	const vector<CSECTION*> & EjectImageSecions =  EjectImage.GetAllSection();
	for ( auto & s : EjectImageSecions ){
		
		if ( ! strncmp (".reloc", (const char*)s->Record.Name, IMAGE_SIZEOF_SHORT_NAME ) )
			continue;

		s->Record.PointerToRawData=0;

		s->ChangeVirtualAddress (va, PrintDetailInfo );

		NewImage.AddSection (s, PrintDetailInfo );

		va += s->Record.Misc.VirtualSize;
		va = NewImage.SectionAlignment (va);

	}

	wprintf( L"Succesful\n");

	wprintf( L"Add all imports from base image.\n" );
	// collect all imports of base image
	for ( const  auto &i: BaseImage.GetAllImports() ){
		NewImage.AddImportDll ( i );
	}

	wprintf( L"Succesful\n");
	wprintf( L"Add all imports from eject image.\n" );
	// and eject image
	for ( const  auto &i: EjectImage.GetAllImports() ){
		NewImage.AddImportDll ( i );
	}
	wprintf( L"Succesful\n");

	wprintf( L"Add all relocs outside all sections\n" );

	//this relocs such outside from all sections
	//pay attension that in section also exist relosc such outside from all its blocs, but then sections moves this relocs moves too.

	for (  auto &r: BaseImage.GetVARelocs() ){

		void * p = r->GetPtr();

		CRELAITIVE_VIRTUAL_ADDRESS SourcePointRva = NewImage.GetRvaFromPtr ( p );
		CVIRTUAL_ADDRESS SourcePointVa = NewImage.GetVaFromRVA ( SourcePointRva );

		r->SetSourcePoint ( SourcePointVa );

		NewImage.WriteVA ( r, r->GetTargetPoint() );
	}

	// The absolutlly sames work as done above, but need add shift for virtual address to new place of eject image
	for (  auto &r: EjectImage.GetVARelocs() ){

		void * p = r->GetPtr();

		CRELAITIVE_VIRTUAL_ADDRESS SourcePointRva = NewImage.GetRvaFromPtr ( p );
		CVIRTUAL_ADDRESS SourcePointVa = NewImage.GetVaFromRVA ( SourcePointRva );

		r->SetSourcePoint ( SourcePointVa );

		NewImage.WriteVA ( r, r->GetTargetPoint() );
	}
	
	wprintf( L"Succesful\n");

	wprintf( L"Set entry point.\n" );
	
	if ( pSupperInitEntry ){
		wprintf ( L"Set entry point to SupperInitEntry\n" );
		CRELAITIVE_VIRTUAL_ADDRESS VaSupperInitEntry = NewImage.GetRvaFromPtr ( pSupperInitEntry );
		NewImage.SetEntryPoint ( VaSupperInitEntry );
	}
	else{
		wprintf ( L"Set entry point to Entry point from eject image\n" );
		NewImage.SetEntryPoint ( NewImage.GetRvaFromPtr ( HeaderEntryPoint) );
	}

	wprintf( L"Set hooks and move functions.\n" );
	
	if ( ! EjectImage.IsImage64() ){
		if ( ! DoExternLinking2<EXTERN_HOOK32> ( &NewImage, PrintDetailInfo ) ){
			wprintf( L"Error.\n");
			return -1;
		}
	}
	else{
		if ( ! DoExternLinking2<EXTERN_HOOK64> ( &NewImage, PrintDetailInfo ) ){
			wprintf( L"Error.\n");
			return -1;
		}
	}

	wprintf( L"Succesful\n");

	wprintf( L"Build.....\n" );
	if ( ! NewImage.Build ( PrintDetailInfo ) ){
		wprintf( L"Error.\n");
		return -1;
	}
	wprintf( L"Succesful\n");

	wprintf( L"Write file.....\n" );
	if ( ! NewImage.Save ( OutImageName ) ){
		wprintf( L"Error.\n");
		return -1;
	}
	wprintf( L"Succesful\n");

	//... in next live for such money from altell 

/*	NewImage.WritePdb ( OutImagePdbName ); */

	return 0;
}

WCHAR ExeName[MAX_PATH];

WCHAR EjectImageName[MAX_PATH];
WCHAR EjectImagePdbName[MAX_PATH];
WCHAR BaseImageName[MAX_PATH];
WCHAR BaseImagePdbName[MAX_PATH];

WCHAR OutImageName[MAX_PATH];

void ArgPrint (int argc, _TCHAR* argv[] )
{
	for (size_t i = 0; i < argc; i++) {
		printf("\narg[%zd]:%S", i, argv[i]);
	}
}



int _tmain ( int argc, _TCHAR* argv[] )
{

	if ( argc < 6 ) {// 
		printf("count args error %d- use Eject.exe Eject.pdb Base.exe Base.pdb out.exe di \n", argc );
		ArgPrint ( argc, argv );
		return -1;
	}
	bool PrintDetailInfo = false;
	wcscpy_s(ExeName, argv[0]);


	wcscpy_s ( EjectImageName,			argv[1] );
	wcscpy_s ( EjectImagePdbName,		argv[2] );
	wcscpy_s ( BaseImageName,			argv[3] );
	wcscpy_s ( BaseImagePdbName,		argv[4] );
	wcscpy_s ( OutImageName,			argv[5] );


	if (argc == 7) {
		if (argv[6][0] = '1') {
			PrintDetailInfo = true;
		}
		else if (argv[6][0] = '0') {
			PrintDetailInfo = false;
		}
		else {
			printf("PrintDetailInfo must be 1 or 0\n");
			return -1;
		}
	}

	int ret = -1;
#ifndef _DEBUG
	try {
#endif
		ret = ImageMerge ( BaseImageName, BaseImagePdbName, EjectImageName, EjectImagePdbName, OutImageName, PrintDetailInfo );

#ifndef _DEBUG
	}
	catch ( const char * err ){
		printf("\nERRRO:%s\n",err);
	}
#endif
	return ret;
}

