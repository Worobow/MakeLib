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

		CodeAnalize.cpp, h

	Environment:

		User-mode, c++
	
	Comment:

--*/


//LARGE_INTEGER 
//

struct CPU
{
	struct REG_8
	{
		unsigned __int8 v8;
		void Reset ()
		{
			v8 = 0;
		}
	};
	
	struct REG_16
	{
		union
		{
			struct
			{
				REG_8 l8;
				REG_8 h8;
			};
			unsigned __int16 v16;
		};
		void Reset ()
		{
			h8.Reset ();
			l8.Reset ();
			v16 = 0;
		}
	};

	struct REG_32
	{
		union
		{
			struct
			{
				REG_16 l16;
				REG_16 h16;
			};
			unsigned __int32 v32;
		};
		void Reset ()
		{
			v32 = 0;
			h16.Reset ();
			l16.Reset ();
		}
	};
	struct REG_64
	{
		union
		{
			struct
			{
				REG_32 l32;
				REG_32 h32;
			};
			unsigned __int64 v64;
		};
		bool ValidValue;
		void Reset ()
		{
			ValidValue = false;
			v64 = 0;
			h32.Reset ();
			l32.Reset ();
		}
	};

	REG_64 rax;
	REG_64 rbx;
	REG_64 rcx;
	REG_64 rdx;

	REG_64 rdi;
	REG_64 rsi;


	CPU ()
	{
		Reset ();
	}
	
	void Reset ( void )
	{
		rax.Reset();
		rbx.Reset();
		rcx.Reset();
		rdx.Reset();
		rdi.Reset();
		rsi.Reset();
	}
	void ResetReg ( x86_reg reg );

	bool SetReg ( x86_reg reg, unsigned __int64 Val );
	bool GetReg ( x86_reg reg, unsigned __int64 *Val );
};


struct CODE_ANALIZE 
{
	struct CREFERENCE;
	struct LABEL {
		enum LABELTYPES
		{
			Code,
			Data
		};
		LABELTYPES LabelType;
		CVIRTUAL_ADDRESS Va;

		CPDB::CSYMBOL* PubSymbol;
		
		vector<CREFERENCE*> References;
		void AddReference ( CREFERENCE * r )
		{
			References.push_back ( r );
		};
		void DelReference ( CREFERENCE * r )
		{
			auto i = std::find(References.begin(),References.end(),r);
			if ( i == References.end() ) 
				throw "invalid reference";
			References.erase(i);
		}
	protected:
		LABEL ( CVIRTUAL_ADDRESS _va, LABELTYPES t )  
			:Va(_va),LabelType(t){}
	};

	struct CODELABEL : LABEL
	{
		CODELABEL( CVIRTUAL_ADDRESS _va, bool _sub )
			: LABEL(_va,LABELTYPES::Code),Sub(_sub)	
		{}
		
		bool Sub; // true if use thru call
	};
	struct DATALABEL:LABEL
	{
		DATALABEL ( CVIRTUAL_ADDRESS _va, size_t _size )
			:LABEL(_va,Data),DataSize(_size){}
		size_t DataSize;
	};
	bool mod_lesser( LABEL* elem1, LABEL* elem2 )
	{
		return elem1->Va < elem2->Va;
	}
	enum ProcessOperandsOptions
	{
		DoReferenceInsideOneSections = 1, // bits sets!!!
	};
	enum WarringsEnabe  // bit set
	{
		InvolvedSegmentRegister = 1<<0,		// no
		InvolvedIndexRegister	= 1<<1,		// no
		InvolvedBadBaseRegister = 1<<2,		// no
		UseRegisterInCallJump	= 1<<3,
		BadBaseInCallJump		= 1<<4,	
		BadDisplInCallJump		= 1<<5,
		CallJumpToJunk			= 1>>6,

		EnableAll				= -1,
		DisableAll				= 0
	};
	struct CREFERENCE {
		enum REFERENCE_TYPES
		{
			Call,
			Jump,
			Data,
		};
		enum REFERENCE_OPERATION
		{
			Read,
			Write,
			ReadWrite,
			Offset
		};
		// ref from there
		struct CPLACE
		{
			const BYTE * Ptr; // poiner 
			size_t Size; // size in bytes
			CVIRTUAL_ADDRESS VA; // va of this place
			CVIRTUAL_ADDRESS RipBase;// RIP base if use RIP 
			bool Rip; // true if RIP
		}Place;
		// ref to there
		CVIRTUAL_ADDRESS TargetVa;
		size_t TargetSize;
		REFERENCE_TYPES Type;
		REFERENCE_OPERATION Operation;
		LABEL * ReferenceTo;
		
		static const char * TypeToString ( REFERENCE_TYPES t )
		{
			static const char * Names[]={"Call","Jump","Data"};
			return Names[t];
		}
		static const char * OperationToString ( REFERENCE_OPERATION o )
		{
			static const char * Names[]={"Read", "Write",	"ReadWrite", "Offset"};
			return Names[o];
		}
		CREFERENCE()
			:ReferenceTo(0){}
	};

	// on such ints, analysis will continue.
	vector<BYTE> NoTerminateInterrupts;

	WarringsEnabe WarningsSet;
	csh cs_handle;
	CIMAGE * Img;

	// set true if erros when was all analize
	bool WasError;
	// set true if warngs was when make  all analize.
	bool WasWarnings;

	// set true if erros when was analize one instruction, reset each time
	bool InstrWasError;
	// set true if warngs was when make analize one instruction, reset each time
	bool InstrWasWarnings;

	// text of deassemble instrs
	string DiassembleText;
	// comments put there
	string CodeComment;

	CBITARRAY * AnalizeSpace;

	map<CVIRTUAL_ADDRESS,LABEL*> Labels;

	vector <CREFERENCE> References;

	PIMAGE_DATA_DIRECTORY iat;

	CVIRTUAL_ADDRESS MinVa;
	CVIRTUAL_ADDRESS MaxVa;

	bool DisAssembleProduce;

	CODE_ANALIZE ( CIMAGE * i, csh h, bool distext )
	{
		DisAssembleProduce = distext;
		InstrWasError = false;
		InstrWasWarnings = false;
		
		MinVa =  i->GetImageBase ();//i->GetMinVA() ;
		MaxVa =  i->GetMaxVA() ;
		
		AnalizeSpace = new CBITARRAY (  CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( MaxVa - MinVa ) );

		Img=i;
		cs_handle = h;
		iat = Img->GetDataDirectory ( IMAGE_DIRECTORY_ENTRY_IAT );
	}
	bool IsAddressValid ( const CVIRTUAL_ADDRESS & va )
	{
		return ( va >= MinVa && va <= MaxVa );
	}
	bool IsAddressWasAnalized ( const CVIRTUAL_ADDRESS & va )
	{
		return AnalizeSpace->Test ( CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( va - MinVa ) );
	}
	void SetAddressWasAnalized ( const CVIRTUAL_ADDRESS & va, DWORD len  )
	{
		DWORD start =  CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( va - MinVa);
		DWORD end = start + len;

		for ( size_t i = start; i< end; i++ ){
			AnalizeSpace->Set( i );
		}
	}

	LABEL* FindLabel ( CVIRTUAL_ADDRESS Va )
	{
		auto r = Labels.find(Va);
		if ( r == Labels.end() )
			return 0;
		return (*r).second;
	}

	void AddCodeLable ( CODELABEL* l )
	{
		Labels.insert ( pair<CVIRTUAL_ADDRESS,CODELABEL*>( l->Va, l ) );
	}
	void AddLable ( LABEL* l )
	{
		Labels.insert ( pair<CVIRTUAL_ADDRESS,LABEL*>( l->Va, l ) );
	}

	bool GetCallJmpDestinationAddress ( cs_insn *insn, size_t n, CVIRTUAL_ADDRESS * ret );
	
	bool Analize ( CVIRTUAL_ADDRESS StartVa, bool Sub );
	
	CPU Cpu;
	bool GetOperand ( cs_insn *insn, cs_x86_op & op, unsigned __int64 * Value, bool MemAsImm );
	void Emulator ( cs_insn *insn );
	void ProcessOperands ( cs_insn *insn, const BYTE * Place, CSECTION * InstrSection, ProcessOperandsOptions Opts, CREFERENCE * Refs, size_t * RefsCount );
	bool GetMemOperandAddress ( x86_op_mem * mem, CVIRTUAL_ADDRESS IpAddr, CVIRTUAL_ADDRESS * Ret, bool * RetRip, bool warnings );
	bool IsTerminateInterrupt ( cs_insn *insn );// return true if after this interrupt need terminate the analysis

	PBYTE GetValPtr ( cs_insn *insn, size_t n, DWORD Val, bool NoSearch );
	size_t CODE_ANALIZE::GetDisplIndex ( cs_insn *insn  );
	size_t CODE_ANALIZE::GetImmIndex ( cs_insn *insn  );

	void PrintOut ( void );
	void CleanPrintOut ( void );

	void RefFromImmediate ( CREFERENCE & Ref, cs_x86 & x86, const BYTE * InstrStartPtr, cs_insn *insn )
	{
		Ref.Place.Ptr = InstrStartPtr + x86.immediateOffset;
		
		size_t DisplacementSize;
		size_t ImmediateSize;

		GetDispAndImmSizes (insn,&DisplacementSize,&ImmediateSize);

		if ( x86.HasImmediate ){
			if ( ImmediateSize != x86.immediateSize ){
				printf("\n0x%I64X : WARNING : correct Capstobe know issue, new immediateSize is %zu, was %d.\n", insn->address, ImmediateSize, x86.immediateSize );
				x86.immediateSize = (uint8_t)ImmediateSize;
			}
		}

		Ref.Place.Size = x86.immediateSize;
		Ref.Place.VA = Img->GetVaFromRVA ( Img->GetRvaFromPtr((PBYTE)Ref.Place.Ptr) );
	}

	void GetDispAndImmSizes ( cs_insn *insn, size_t * DisplacementSize, size_t * ImmediateSize )
	{

		if ( insn->detail->x86.HasDisplacement ){
			if ( insn->detail->x86.HasImmediate ){
				*DisplacementSize = insn->detail->x86.immediateOffset - insn->detail->x86.displacementOffset;
			}
			else
				*DisplacementSize = insn->size - insn->detail->x86.displacementOffset;
		}
		else{
			*DisplacementSize = 0;
		}


		if ( insn->detail->x86.HasImmediate ){
			*ImmediateSize = insn->size -insn->detail->x86.immediateOffset;
		}
		else{
			*ImmediateSize = 0;
		}

	}

	void RefFromDisplacement ( CREFERENCE & Ref, cs_x86 & x86, const BYTE * InstrStartPtr, cs_insn *insn )
	{
		Ref.Place.Ptr = InstrStartPtr + x86.displacementOffset;
		
		size_t DisplacementSize;
		size_t ImmediateSize;

		GetDispAndImmSizes (insn,&DisplacementSize,&ImmediateSize);

		if ( x86.HasDisplacement ){
			if ( DisplacementSize != x86.displacementSize ){
				printf("\n0x%I64X : WARNING : correct Capstobe know issue, new displacementSize is %zu, was %d.\n", insn->address, DisplacementSize, x86.displacementSize );
				x86.displacementSize = (uint8_t)DisplacementSize;
			}
		}

//		if ( x86.HasImmediate ){
//			if ( ImmediateSize != x86.immediateSize ){
//				printf("\n0x%I64X : WARNING : correct Capstobe know issue, new immediateSize is %d, was %d.\n", insn->address, ImmediateSize, x86.immediateSize );
//				x86.immediateSize = ImmediateSize;
//			}
//		}

		Ref.Place.Size = x86.displacementSize;
		Ref.Place.VA = Img->GetVaFromRVA ( Img->GetRvaFromPtr((PBYTE)Ref.Place.Ptr) );
	}
	CREFERENCE::REFERENCE_OPERATION OperationTypesCvt ( cs_ac_type t ) 
	{
		if ( t == CS_AC_INVALID )
			return CREFERENCE::Read;

		if ( ( t & CS_AC_READ) == CS_AC_READ )
			return CREFERENCE::Read;

		if ( (t & CS_AC_WRITE ) == CS_AC_WRITE )
			return CREFERENCE::Write;

		if ( (t & (CS_AC_WRITE|CS_AC_READ)) == (CS_AC_WRITE|CS_AC_READ) )
			return CREFERENCE::ReadWrite;

		throw "invalid type!";
	}
};