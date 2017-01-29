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
		
		CFuncsTable.h, CFuncsTable.cpp

	Environment:
		
		User-mode command line

	Comment:

		class for the implementation of functional work functions table for images

--*/

#include "stdafx.h"

//add  detail for next coders
// https://sourceware.org/ml/binutils/2009-04/msg00181.html
// http://www.codemachine.com/article_x64deepdive.html

void CFUNCSTABLE::UnwindCodePrint ( UNWIND_CODE  c )
{
	printf ( "{0x%02X,%s,%s}", c.OffsetInProlog, UnwindOpCodesToString ( c.UnwindOperationCode ), UnWindOpInfoToString(c.OperationInfo) );
}


void CFUNCSTABLE::RuntimeFunctionPrint ( _MY_RUNTIME_FUNCTION * Func, CIMAGE * img )
{
	printf("%08X %08X %08X ", Func->BeginAddress, Func->EndAddress,   Func->UnwindInfoAddress  );
	UNWIND_INFO * uinfo = (UNWIND_INFO *)img->GetPtrFromRVA ( Func->UnwindInfoAddress );
	
	if ( _PrintDetailInfo >= DetailInfo )
		UnwindInfoPrint (uinfo, img );	

	printf("\n");
}
void CFUNCSTABLE::UnwindInfoPrint ( UNWIND_INFO * uinfo, CIMAGE * img  )
{

	printf("Ver:%d,",uinfo->Version);
	printf("Flgs:[");
	if ( uinfo->Flags & UNW_FLAG_EHANDLER )
		printf("{UNW_FLAG_EHANDLER}");
	if ( uinfo->Flags & UNW_FLAG_UHANDLER  )
		printf("{UNW_FLAG_UHANDLER}");
	if ( uinfo->Flags & UNW_FLAG_CHAININFO   )
		printf("{UNW_FLAG_CHAININFO}");
	printf("],");
	printf("Ps:%03d,", uinfo->SizeOfProlog );
	printf("CC:%03d,", uinfo->CountOfCodes );
	printf("FReg:%d,", uinfo->FrameRegister );
	printf("FO:%d,", uinfo->FrameOffset );

	size_t ArraySize = uinfo->CountOfCodes;

	if ( ArraySize & 1 )
		ArraySize++;

	UNWIND_CODE* UnwindCodesArray = (UNWIND_CODE*)((PBYTE)uinfo + sizeof(UNWIND_INFO));

	printf("Codes:[");
	for ( size_t i=0; i < uinfo->CountOfCodes; i++ ){
		UnwindCodePrint ( UnwindCodesArray[i] );
		 
	}
	printf("],");
	if ( uinfo->Flags & UNW_FLAG_CHAININFO   ){
		RuntimeFunctionPrint ( (_MY_RUNTIME_FUNCTION *) & UnwindCodesArray [ArraySize], img );
	}
	else if ( uinfo->Flags & UNW_FLAG_EHANDLER || uinfo->Flags & UNW_FLAG_UHANDLER ){
		DWORD ExHandlerRva = *(PDWORD) &UnwindCodesArray [ArraySize];
		printf( "EH:0x%08X(%p)", ExHandlerRva, img->GetPtrFromRVA(ExHandlerRva) );
	}
}

CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY* CFUNCSTABLE::UnPackRuntimeFunction ( _MY_RUNTIME_FUNCTION * fun,  CIMAGE * img )
{

	BLOCK<BYTE> * FunctionBlock = img->GetBlockByRVA ( fun->BeginAddress );

	RUNTIME_FUNCTION_ENTRY * OldEntry =  FunctionBlock->GetFuncsTableEntry ( fun->BeginAddress );

	if ( OldEntry )
		return OldEntry;

	if ( _PrintDetailInfo >= BaseInfo )
		RuntimeFunctionPrint ( fun, img );

	RUNTIME_FUNCTION_ENTRY * NewEntry = new RUNTIME_FUNCTION_ENTRY();

	NewEntry->BeginAddress = CRELAITIVE_VIRTUAL_ADDRESS ( fun->BeginAddress );
	//NewEntry->EndAddress   = CRELAITIVE_VIRTUAL_ADDRESS ( );

	NewEntry->Length = fun->EndAddress - fun->BeginAddress;// 

	UNWIND_INFO * uinfo = (UNWIND_INFO *)img->GetPtrFromRVA ( fun->UnwindInfoAddress );

	NewEntry->Version = uinfo->Version;
	NewEntry->Flags = uinfo->Flags;
	NewEntry->SizeOfProlog = uinfo->SizeOfProlog ;

	//NewEntry->c= uinfo->CountOfCodes );

	NewEntry->FrameRegister= uinfo->FrameRegister ;
	NewEntry->FrameOffset = uinfo->FrameOffset ;

	size_t ArraySize = uinfo->CountOfCodes;

	if ( ArraySize & 1 )
		ArraySize++;

	UNWIND_CODE* UnwindCodesArray = (UNWIND_CODE*)((PBYTE)uinfo + sizeof(UNWIND_INFO));

	for ( size_t i=0; i <  uinfo->CountOfCodes; i++ ){
		NewEntry->UnwindCodesArray.push_back ( UnwindCodesArray[i] );
	}

	if ( uinfo->Flags & UNW_FLAG_CHAININFO ){
	//	throw "not check yet!, check it yourself, now :-)";
		NewEntry->Chaings = UnPackRuntimeFunction ( (_MY_RUNTIME_FUNCTION *) & UnwindCodesArray [ArraySize], img );
		NewEntry->ExceptionHandler =  CRELAITIVE_VIRTUAL_ADDRESS (0);
	}
	else {

		NewEntry->Chaings = 0;

		if ( uinfo->Flags & UNW_FLAG_EHANDLER || uinfo->Flags & UNW_FLAG_UHANDLER )
			NewEntry->ExceptionHandler = CRELAITIVE_VIRTUAL_ADDRESS ( *(PDWORD) &UnwindCodesArray [ArraySize] );
		else
			NewEntry->ExceptionHandler =  CRELAITIVE_VIRTUAL_ADDRESS (0);
	}

	FunctionBlock = img->GetBlockByRVA ( NewEntry->BeginAddress );
	if ( ! FunctionBlock )
		throw "Block not found";
	
	if ( FunctionBlock->GetFuncsTableEntry ( fun->BeginAddress ) )
		throw "Multy creazy!";

	FunctionBlock->AddFuncsTableEntry ( NewEntry ) ;

	char str[44];

	_itoa_s ( CRELAITIVE_VIRTUAL_ADDRESS::Get32(NewEntry->BeginAddress), str, 16 );
	CINTERNAL_RVA_RELOC * RelBeginAddress = new CINTERNAL_RVA_RELOC( CRELAITIVE_VIRTUAL_ADDRESS(0), img->GetImageBase(), &NewEntry->BeginAddress, string("FuncsArray.FunBegin") + "(" + str + ")" );
	RelBeginAddress->SetTag ( FUNCS_TABLE_REFERENCE );

//	_itoa_s ( CRELAITIVE_VIRTUAL_ADDRESS::Get32(NewEntry->EndAddress), str, 16 );
//	CINTERNAL_RVA_RELOC * RelEndAddress   = new CINTERNAL_RVA_RELOC( CRELAITIVE_VIRTUAL_ADDRESS(0), img->GetImageBase(), &NewEntry->EndAddress,   string("FuncsArray.FunEnd")   + "(" + str + ")" );
//	RelEndAddress->SetTag ( FUNCS_TABLE_REFERENCE );

	FunctionBlock->WriteRVA ( RelBeginAddress, NewEntry->BeginAddress );
//	FunctionBlock->WriteRVA ( RelEndAddress,   NewEntry->EndAddress   );

	if ( NewEntry->ExceptionHandler != 0 ){
		_itoa_s ( CRELAITIVE_VIRTUAL_ADDRESS::Get32(NewEntry->ExceptionHandler), str, 16 );
		CINTERNAL_RVA_RELOC * RelExceptionAddress = new CINTERNAL_RVA_RELOC( CRELAITIVE_VIRTUAL_ADDRESS(0), img->GetImageBase(), & NewEntry->ExceptionHandler, string("FuncsArray.FunExceptionHandler") + "(" + str + ")" );
		RelExceptionAddress->SetTag(FUNCS_TABLE_REFERENCE);
		FunctionBlock = img->GetBlockByRVA ( NewEntry->ExceptionHandler );
		if ( ! FunctionBlock )
			throw "Block not found";

		FunctionBlock->WriteRVA ( RelExceptionAddress,   NewEntry->ExceptionHandler   );
	}

	return NewEntry;
}

bool CFUNCSTABLE::Create ( CIMAGE * img )
{
	printf("-------------------Load function table ----------------------\n");
	PIMAGE_DATA_DIRECTORY excs = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_EXCEPTION );

	if ( excs->VirtualAddress != 0 ){
		
		_MY_RUNTIME_FUNCTION * Funcs = (_MY_RUNTIME_FUNCTION*)img->GetPtrFromRVA ( excs->VirtualAddress );
		
		size_t FuncsSize = excs->Size /sizeof( _MY_RUNTIME_FUNCTION);
		
		for ( size_t i=0; i< FuncsSize; i++ ){
			if ( Funcs[i].BeginAddress )// skip 
				UnPackRuntimeFunction ( &Funcs[i], img ) ;
		}
	}
	else{ // IMAGE_DIRECTORY_ENTRY_EXCEPTION
		printf ( "No IMAGE_DIRECTORY_ENTRY_EXCEPTION\n" );
	}


	printf("--------------------------Done----------------------\n");
	return true;
}

void CFUNCSTABLE::PackRuntimeFunction ( IN RUNTIME_FUNCTION_ENTRY * e, OUT _MY_RUNTIME_FUNCTION * rf, OUT UNWIND_INFO * uinfo, IN size_t BufferMaxSize, OUT size_t * BufferSize )
{
	rf->BeginAddress = CRELAITIVE_VIRTUAL_ADDRESS::Get32( e->BeginAddress );
	//rf->EndAddress = CRELAITIVE_VIRTUAL_ADDRESS::Get32( e->EndAddress );

	rf->EndAddress =  CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( e->BeginAddress + e->Length );// CRELAITIVE_VIRTUAL_ADDRESS::Get32( e->EndAddress );

	rf->UnwindInfoAddress=0;

	size_t ArraySize = (BYTE)e->UnwindCodesArray.size();
	if ( ArraySize & 1 )
		ArraySize++;

	size_t OutBuferSize  = sizeof(UNWIND_INFO) + sizeof(UNWIND_CODE) * ArraySize;

	if ( e->Flags & UNW_FLAG_EHANDLER || e->Flags & UNW_FLAG_UHANDLER )
		OutBuferSize += sizeof(DWORD);

	if (  e->Flags & UNW_FLAG_CHAININFO )
		OutBuferSize += 3 * sizeof(DWORD);

	if ( BufferMaxSize < OutBuferSize )
		throw "size";

	uinfo->CountOfCodes = (BYTE)e->UnwindCodesArray.size();
	uinfo->Flags = e->Flags;
	uinfo->FrameOffset=e->FrameOffset;
	uinfo->FrameRegister=e->FrameRegister;
	uinfo->SizeOfProlog=e->SizeOfProlog;
	uinfo->Version=e->Version;
	
	UNWIND_CODE* UnwindCodesArray = (UNWIND_CODE*)((PBYTE)uinfo + sizeof(UNWIND_INFO));

	for ( size_t i=0; i < uinfo->CountOfCodes; i++ ){
		UnwindCodesArray[i] = e->UnwindCodesArray[i];
	}

	PDWORD Tail = (PDWORD) &UnwindCodesArray [ArraySize] ;

	if ( e->Flags & UNW_FLAG_EHANDLER || e->Flags & UNW_FLAG_UHANDLER )
		*Tail = CRELAITIVE_VIRTUAL_ADDRESS::Get32(e->ExceptionHandler);
	else{
		if (  e->Flags & UNW_FLAG_CHAININFO ){
			if ( ! e->Chaings )
				throw "bad unpack was";

	//		throw "this must be implemented yet";

			*Tail++ =  CRELAITIVE_VIRTUAL_ADDRESS::Get32(e->Chaings->BeginAddress);
			*Tail++ =  CRELAITIVE_VIRTUAL_ADDRESS::Get32(e->Chaings->BeginAddress + e->Length);		//e->Chaings->EndAddress
			*Tail++ =  0;																			//CRELAITIVE_VIRTUAL_ADDRESS::Get32(e->Chaings->);

		}
	}
	*BufferSize = OutBuferSize;

}


// Define a functor.

struct UnwindInfoTest
{
    bool operator()(CFUNCSTABLE::UNWIND_INFO const * a, CFUNCSTABLE::UNWIND_INFO const * b) const
    {

		if (a->CountOfCodes < b->CountOfCodes)
			return true;
		if (a->CountOfCodes > b->CountOfCodes)
			return false;
		if (a->Flags < b->Flags)
			return true;
		if (a->Flags > b->Flags)
			return false;
		if (a->FrameOffset < b->FrameOffset)
			return true;
		if (a->FrameOffset > b->FrameOffset)
			return false;
		if (a->SizeOfProlog < b->SizeOfProlog)
			return true;
		if (a->SizeOfProlog > b->SizeOfProlog)
			return false;
		if (a->FrameRegister < b->FrameRegister)
			return true;
		if (a->FrameRegister > b->FrameRegister)
			return false;
		if (a->Version < b->Version)
			return true;
		if (a->Version > b->Version)
			return false;

		PBYTE a_UnwindCodesArray = (((PBYTE)a) + sizeof(CFUNCSTABLE::UNWIND_INFO));
		PBYTE b_UnwindCodesArray = (((PBYTE)b) + sizeof(CFUNCSTABLE::UNWIND_INFO));

		size_t s = a->CountOfCodes *  sizeof(CFUNCSTABLE::UNWIND_CODE);

		for (size_t i = 0; i < s; i++){
			if (a_UnwindCodesArray[i] < b_UnwindCodesArray[i])
				return true;
			if (a_UnwindCodesArray[i] > b_UnwindCodesArray[i])
				return false;
		}
		// '=='

	return false;
    }
};

size_t CFUNCSTABLE::Pack ( CSECTION * out_s, CIMAGE * img, bool PrintDebugDetails )
{
//	vector<_MY_RUNTIME_FUNCTION> FuncsTable;

	map <UNWIND_INFO const *, CRELAITIVE_VIRTUAL_ADDRESS,UnwindInfoTest> ded;
	
	BYTE WorkBuffer[230];

//	_MY_RUNTIME_FUNCTION TableRecord;

	printf("-------------------Pack function table ----------------------\n");

	const vector<CSECTION*> & ss = img->GetAllSection();

	size_t FuncsCount = 0;

	for ( auto & s : ss )	{
		for ( auto & b: s->_Data ){
			const map <CRELAITIVE_VIRTUAL_ADDRESS, CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY*> & Funcs = b->GetFuncsTable();
			FuncsCount +=Funcs.size();
		}
	}

	if ( FuncsCount == 0 ) 
		return 0;

	BLOCK<_MY_RUNTIME_FUNCTION> * FuncTableBlk = new BLOCK<_MY_RUNTIME_FUNCTION>( "FUNCTION TABLE" );
	FuncTableBlk->Alloc ( FuncsCount );
	
	_MY_RUNTIME_FUNCTION * FuncTableBlkBuffer = FuncTableBlk->GetBuffer();
	
	out_s->AddBlock ( (BLOCK<BYTE>*) FuncTableBlk, PrintDebugDetails );

	size_t FuncNumber=0;
		
	char StrUnWindNum[32];
	for ( auto & s : ss )	{
		for ( auto & b: s->_Data ){

			const map <CRELAITIVE_VIRTUAL_ADDRESS, CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY*> & Funcs = b->GetFuncsTable();

			for ( auto & ff: Funcs ){

				auto & f = ff.second;

				size_t PackBufferSize;
				
				PackRuntimeFunction ( f, &FuncTableBlkBuffer[FuncNumber], (UNWIND_INFO *)WorkBuffer, sizeof(WorkBuffer), &PackBufferSize );

				BLOCK<BYTE> * FuncBlock = img->GetBlockByRVA ( FuncTableBlkBuffer[FuncNumber].BeginAddress );
				if ( FuncBlock == 0 ){
					fflush(stdout);
					throw "FuncBlock block no pasaran!";
				}

				BLOCK<BYTE> * FuncEndBlock = img->GetBlockByRVA ( FuncTableBlkBuffer[FuncNumber].EndAddress -1 );
				if ( FuncBlock != FuncEndBlock ){
					fflush(stdout);
					throw "FuncBlock 2 block no pasaran!";
				}

				CINTERNAL_RVA_RELOC * RelStartFuncRva = new CINTERNAL_RVA_RELOC ( 0, img->GetImageBase(), &FuncTableBlkBuffer[FuncNumber].BeginAddress, "PackFuncTable.StartFunc" );
				CINTERNAL_RVA_RELOC * RelEndFuncRva   = new CINTERNAL_RVA_RELOC ( 0, img->GetImageBase(), &FuncTableBlkBuffer[FuncNumber].EndAddress,   "PackFuncTable.EndFunc"   );

				RelStartFuncRva->SetTag (FUNCS_TABLE_REFERENCE);
				RelEndFuncRva->SetTag   (FUNCS_TABLE_REFERENCE);

				FuncBlock->WriteRVA ( RelStartFuncRva, FuncTableBlkBuffer[FuncNumber].BeginAddress );
				FuncBlock->WriteRVA ( RelEndFuncRva,   FuncTableBlkBuffer[FuncNumber].EndAddress   );

				auto entry = ded.find ( ( UNWIND_INFO *)WorkBuffer );

				BLOCK<BYTE> * UnwindInfoBlock;
				CRELAITIVE_VIRTUAL_ADDRESS UnwindInfoAddress;

				if ( entry == ded.end() ){ 
					_itoa_s ( (int)ded.size(), StrUnWindNum, 10 );

					UnwindInfoBlock = new BLOCK<BYTE>( string ( "UNWIND_INFO" ) + "#(" + StrUnWindNum + ")" ) ;
					
					UnwindInfoBlock->Alloc ( PackBufferSize );

					UNWIND_INFO * StoreUi  = (UNWIND_INFO *)UnwindInfoBlock->GetBuffer ();

					memcpy ( StoreUi, WorkBuffer, PackBufferSize );

					out_s->AddBlock ( UnwindInfoBlock, PrintDebugDetails );
					
					UnwindInfoAddress =  UnwindInfoBlock->GetRelativeVirtialAddress() ;

					ded.insert ( pair< UNWIND_INFO const *, CRELAITIVE_VIRTUAL_ADDRESS> ( StoreUi, UnwindInfoAddress ) );
					
				}
				else{
					UnwindInfoBlock = out_s->GetBlockByPtr ( (void*)entry->first );
					if ( UnwindInfoBlock == 0 )
						throw "UnwindInfoBlock block no pasaran!";
						
					UnwindInfoAddress = entry->second;
				}

				CINTERNAL_RVA_RELOC * RelUnwindFuncRva   = new CINTERNAL_RVA_RELOC ( 0, img->GetImageBase(), &FuncTableBlkBuffer[FuncNumber].UnwindInfoAddress, "PackFuncTable.UnwindInfoAddress" );
				RelUnwindFuncRva->SetTag (FUNCS_TABLE_REFERENCE );

				UnwindInfoBlock->WriteRVA ( RelUnwindFuncRva, UnwindInfoAddress );

				FuncNumber++;
				
			}
		}
	}

	PIMAGE_DATA_DIRECTORY excs = img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_EXCEPTION );
	
	FuncTableBlk->WriteRVA ( new CINTERNAL_RVA_RELOC( 0, img->GetImageBase(), &excs->VirtualAddress, "IMAGE_DIRECTORY_ENTRY_EXCEPTION" ), FuncTableBlk->GetRelativeVirtialAddress()  );
		
	excs->Size = FuncTableBlk->GetByteSize();

	printf("--------------------------Done----------------------\n");
	return FuncsCount;
}

CFUNCSTABLE::RUNTIME_FUNCTION_ENTRY * CFUNCSTABLE::GetFunctionByVA ( CIMAGE * img, CRELAITIVE_VIRTUAL_ADDRESS rva )
{
	BLOCK<BYTE> * FunctionBlock = img->GetBlockByRVA ( rva );

	if ( ! FunctionBlock )
		throw "Block not found";

	return FunctionBlock->GetFuncsTableEntry ( rva ) ;
}