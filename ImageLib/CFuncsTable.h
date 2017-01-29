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



class CFUNCSTABLE
{

	enum PRINT_INFO_SET{
		No=1,
		BaseInfo=2,
		DetailInfo=4
	};

	PRINT_INFO_SET _PrintDetailInfo = No;

public:
	
	static const char * UnwinfFlagsToString ( BYTE f ) 
	{
		switch ( f ){
			case UNW_FLAG_NHANDLER:		return "UNW_FLAG_NHANDLER";
			case UNW_FLAG_EHANDLER:		return "UNW_FLAG_EHANDLER";
			case UNW_FLAG_UHANDLER:		return "UNW_FLAG_UHANDLER";
			case UNW_FLAG_CHAININFO:	return "UNW_FLAG_CHAININFO";
		}
		throw "flag invalid";
	}
	static const char * UnwindOpCodesToString ( BYTE o )
	{
		switch ( o )
		{
		case UWOP_PUSH_NONVOL:		return "UWOP_PUSH_NONVOL";
		case UWOP_ALLOC_LARGE:		return "UWOP_ALLOC_LARGE";
		case UWOP_ALLOC_SMALL:		return "UWOP_ALLOC_SMALL";
		case UWOP_SET_FPREG:		return "UWOP_SET_FPREG";
		case UWOP_SAVE_NONVOL:		return "UWOP_SAVE_NONVOL";
		case UWOP_SAVE_NONVOL_FAR:	return "UWOP_SAVE_NONVOL_FAR";
		case UWOP_SAVE_XMM:			return "UWOP_SAVE_XMM";
		case UWOP_SAVE_XMM_FAR:		return "UWOP_SAVE_XMM_FAR";
		case UWOP_SAVE_XMM128:		return "UWOP_SAVE_XMM128";
		case UWOP_SAVE_XMM128_FAR:	return "UWOP_SAVE_XMM128_FAR";
		case UWOP_PUSH_MACHFRAME:	return "UWOP_PUSH_MACHFRAME";
		}
		throw "Op code invalid";
	}
	static const char * UnWindOpInfoToString ( BYTE i )
	{
		if ( i > 15 )
			throw "Info invalid";
		static const char *s[]={ "RAX","RCX","RDX","RBX","RSP","RBP","RSI","RDI","R8","R9","R10","R11","R12","R13","R14","R15"};

		return s[i];
	}
	enum UNWIND_OPERATION_INFO {
		RAX	=0,
		RCX	=1,
		RDX	=2,
		RBX	=3,
		RSP	=4,
		RBP	=5,
		RSI	=6,
		RDI	=7,
		R8	=8,
		R9	=9,
		R10	=10,
		R11	=11,
		R12	=12,
		R13	=13,
		R14	=14,
		R15	=15
	};
	enum UNWIND_OP_CODES{
		UWOP_PUSH_NONVOL =0,
		UWOP_ALLOC_LARGE =1,
		UWOP_ALLOC_SMALL =2,
		UWOP_SET_FPREG =3,
		UWOP_SAVE_NONVOL =4,
		UWOP_SAVE_NONVOL_FAR =5,
		UWOP_SAVE_XMM =6,
		UWOP_SAVE_XMM_FAR= 7,
		UWOP_SAVE_XMM128=8,
		UWOP_SAVE_XMM128_FAR =9,
		UWOP_PUSH_MACHFRAME =10
	};
	struct UNWIND_INFO {
		BYTE Version:3;
		BYTE Flags:5;
		BYTE SizeOfProlog;
		BYTE CountOfCodes;
		BYTE FrameRegister:4;
		BYTE FrameOffset:4;
	//	USHORT UnwindCodesArray[0];
	//	ULONG ExceptionHandler;
	//	ULONG ExceptionData[1];
	};
	struct UNWIND_CODE{
		BYTE OffsetInProlog;
		BYTE UnwindOperationCode:4;
		BYTE OperationInfo:4;
	};
	struct _MY_RUNTIME_FUNCTION {
		DWORD BeginAddress;
		DWORD EndAddress;
		union {
			DWORD UnwindInfoAddress;
			DWORD UnwindData;
		} DUMMYUNIONNAME;
	};
	struct RUNTIME_FUNCTION_ENTRY
	{
		CRELAITIVE_VIRTUAL_ADDRESS BeginAddress;
		CRELAITIVE_VIRTUAL_ADDRESS Length;	//EndAddress;
		BYTE Version;
		BYTE Flags;
		BYTE SizeOfProlog;
		//BYTE CountOfCodes;
		BYTE FrameRegister;
		BYTE FrameOffset;
		vector<UNWIND_CODE> UnwindCodesArray;
		RUNTIME_FUNCTION_ENTRY * Chaings;
		CRELAITIVE_VIRTUAL_ADDRESS ExceptionHandler;
	};

	CFUNCSTABLE ( ){	};

	void SetPrintDetailInfo ( bool PrintDetailInfo )
	{
		if ( PrintDetailInfo )
			_PrintDetailInfo = BaseInfo;
	}

	static RUNTIME_FUNCTION_ENTRY * GetFunctionByVA ( CIMAGE * , CRELAITIVE_VIRTUAL_ADDRESS rva );

	void RuntimeFunctionPrint ( _MY_RUNTIME_FUNCTION * Funcs, CIMAGE * img );
	void UnwindInfoPrint ( UNWIND_INFO * uinfo , CIMAGE * img);
	void UnwindCodePrint ( UNWIND_CODE  c );
	RUNTIME_FUNCTION_ENTRY* UnPackRuntimeFunction ( _MY_RUNTIME_FUNCTION *, CIMAGE * img );
	void PackRuntimeFunction ( IN RUNTIME_FUNCTION_ENTRY * e, OUT _MY_RUNTIME_FUNCTION * rf, OUT UNWIND_INFO * uinfo, IN size_t SizeOfArrayUInfo, OUT size_t * PackedSizeOfArrayUInfo );
	bool Create ( CIMAGE * img );
	size_t Pack ( CSECTION * s , CIMAGE * img, bool PrintDebugDetails );
#if 0
	const map <CRELAITIVE_VIRTUAL_ADDRESS, RUNTIME_FUNCTION_ENTRY*> & GetTable()
	{
		return _Table;
	};
#endif // 0

private:

	
};