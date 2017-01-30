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

			TestCopyImage.cpp

		Environment:

			User-mode command line

		Comment:

			The main module of the program to copy old code into new image.

--*/



#include "stdafx.h"

//
// create fake section
// 
CRELAITIVE_VIRTUAL_ADDRESS CreateFakeSection ( CIMAGE * img, CRELAITIVE_VIRTUAL_ADDRESS rva, size_t size, const char * name )
{
	// create section
	CSECTION * ns = new CSECTION ( name, 0 );

	// Section is container for blocks array.
	// The array can contain only one block, and maybe a lot of blocks.

	// create one block
	BLOCK<BYTE> * nb = new BLOCK<BYTE>( "Block for ass" );

	// alloc memory with init 
	nb->Alloc ( size, 'a' );

	// put the block to section
	ns->AddBlock ( nb, true );

	// put new section to its place
	ns->ChangeVirtualAddress ( rva, true );

	//add new created section to image
	img->AddSection ( ns, true );

	// cacl new free place pointer
	rva = rva + ns->Record.Misc.VirtualSize;
	rva = img->SectionAlignment ( CRELAITIVE_VIRTUAL_ADDRESS::Get32( rva ) );

	// 
	return rva;
}

//
// This is an example of how from several pieces of old executable image, you can build a completely new one.
// 
// Usually build a new executable images much easier by changing parts of the old executable image.

// So do easier and more reliable against possible errors, since we basically maintain 
// the integrity of the old executable image.
// Old executable image in a consistent form having transferred to the new with a few changes.
// But here, in this example, we want to show how to build an entirely new image from scratch.
// Using only some parts of the old one.
// In other words, it is only some isolated fragments of the old executable image will be included in the new one.
// The rest of the old executable image will be droped.

// In this example, we move all sections, prepend every one of them fucking empty section with garbage.
// Size of fake section is 'hins'.
// Garbage is char 'a'.

bool TestCopyImage ( wchar_t * InImageName, wchar_t * PdbName, wchar_t * OutImageName, size_t hins ) 
{

	CIMAGE OldImage;

	wprintf( L"Try opening image %s\n", InImageName );

	DWORD Status;

	if ( ! OldImage.OpenExist ( InImageName, &Status, string ( "inImage" ), false ) ){
		printf("Map failed: %d\n", Status );
		return false;
	}

	OldImage.LoadPdb ( PdbName, false );

	wprintf( L"Succesful\n");

	OldImage.MakeCodeAnalize ( true );

	// 
	CRELAITIVE_VIRTUAL_ADDRESS ep = OldImage.GetEntryPoint();

	void * pep = OldImage.GetPtrFromRVA ( ep );

	// create a new 

	CIMAGE NewImage;

	// We use header from old image, but you can build new header manualy. 
	NewImage.CreateNew ( OldImage.GetFileImage(), NULL , string("outImage") );

	CRELAITIVE_VIRTUAL_ADDRESS rva = OldImage.GetAllSection()[0]->Record.VirtualAddress;

	
	// Now here we begin to destroy the old image. Completely, and then
	// We will built the new image -
	// Who was nothing will become everything. :)
	// 
	// If you want to save old executable image from destriction, 
	// will need exectly of the copy operation of those parts from of 
	// the old executable image that you need, into new executable image.
	//
	// And of those copies you build your new executable image.
	// Otherwise, those parts of the old executable image that you add to a new, 
	// without copying will automatically be updated in the old executable image. 
	// These fragments (parts) are no longer valid in the old executable image.
	// We're just doing a demonstration here, so we'll be from the pieces of 
	// the old executable image build a new executable image, without copying.

	for ( auto & s: OldImage.GetAllSection() ){

		// Skipping sections of those which will be built automatically when you save a new file.
		if ( s->GetName() == ".reloc" || s->GetName() == ".pdata" )
			continue;
	
		// create fake section size of hins and with garbage 'a' and with const name "ssas"
		// 
		rva = CreateFakeSection ( &NewImage, rva, hins, "ssas" );

		// clear old section file place for future correct moving
		s->Record.PointerToRawData = 0;

		// change section 
		s->ChangeVirtualAddress ( rva, true );

		// move virtual place pointer 
		rva = rva + s->Record.Misc.VirtualSize;
		// aligment new place
		rva = NewImage.SectionAlignment ( CRELAITIVE_VIRTUAL_ADDRESS::Get32( rva ) );
		// put section into new image
		NewImage.AddSection ( s, true );
	}

	// all done

	// magic 
	// auto calc new virtual place of old buffer pointer. Real magic. ;-)
	NewImage.SetEntryPoint ( NewImage.GetRvaFromPtr ( pep ) );
	
	// copy all imports
	for ( const  auto &i: OldImage.GetAllImports() ){
		NewImage.AddImportDll ( i );
	}

	// copy all exports
	for ( const auto &e : OldImage.GetAllExports() ){
		NewImage.AddExport ( e );
	}

	NewImage.SetExportName ( OldImage.GetExportName() );

	// for all relocs from old image, create new and add to new image
	for (  auto &r: OldImage.GetVARelocs() ){
		CINTERNAL_RELOC * nr = r->CreateNew ( NewImage.GetImageBase() );
		NewImage.WriteVA ( nr, nr->GetTargetPoint() );
	}

	// Pooh! Too much heavy work!

	// now build result
	NewImage.Build(true);


	wprintf( L"\n\nTry write of the new image to :%s", OutImageName );

	// try to save file, if file exist will error
	if ( ! NewImage.Save ( OutImageName ) )
		wprintf( L"\nNot Succesful\n");
	else
		wprintf( L"\nSuccesful\n");

	// always
	return true;
}
// size 
#define HOW_INSERT 200

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 4) {
		wprintf(L"Arguments error.\n\t Usage:\n\t\t %s <Base Image Name> <Base Image Pdb Name> <Out Image Name>\nExit -1\n",argv[0]);
		return -1;
	}
	return TestCopyImage  ( argv[1], argv[2], argv[3], HOW_INSERT ) ;
}

