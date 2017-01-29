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

#include "stdafx.h"

// Get address of the mem operand type, if gets imposible, return false
// else return true and set value of 'Ret'
//
// Also return RIP true if use relative EIP address
bool CODE_ANALIZE::GetMemOperandAddress ( x86_op_mem * mem, CVIRTUAL_ADDRESS ip_reg_value, CVIRTUAL_ADDRESS * Ret, bool * RetRip, bool warnings )
{
	CVIRTUAL_ADDRESS Va = 0;

	CRELAITIVE_VIRTUAL_ADDRESS disp  = mem->disp;

	//CVIRTUAL_ADDRESS Base = 0;

//  now no use seg and index
//	CRELAITIVE_VIRTUAL_ADDRESS Seg	 = 0;

	CRELAITIVE_VIRTUAL_ADDRESS Index = 0;

	*RetRip = false;

	if ( mem->base != X86_REG_INVALID ){
		if ( mem->base != X86_REG_RIP && mem->base != X86_REG_EIP){
			// You need make instruction simulatuion for group such load its register.
			unsigned __int64 _Base;
			
			if ( Cpu.GetReg ( mem->base, &_Base ) ){
				Va = _Base;
				*RetRip = false;
			}
			else{
				if ( mem->base != X86_REG_RBP && mem->base != X86_REG_EBP && mem->base != X86_REG_RSP && mem->base != X86_REG_ESP ){
					if ( warnings && ( WarningsSet & InvolvedBadBaseRegister ) ){
						InstrWasWarnings |= true;
						CodeComment += ";Involved base register. Differen then (ip, bp, sp), its value is unknown to me.";
					}
				}
				return false;
			}
		}
		else{ // reg == ip 
			Va = ip_reg_value;
			*RetRip = true;
		}
	}

	if ( mem->index != X86_REG_INVALID ){
		// You need make instruction simulatuion for group such load its register.
		unsigned __int64 _Index;

		if ( Cpu.GetReg ( mem->index, &_Index ) ){
			Index = (DWORD) (_Index *mem->scale);
		}
		else{
			if ( warnings && ( WarningsSet & InvolvedBadBaseRegister ) ){
				InstrWasWarnings |= true;
				CodeComment += ";Involved index register, its value is unknown to me.";
			}
		//	return false;
		}
	}

	Va = Va + disp + Index;

	bool _IsAddresValid = IsAddressValid ( Va );

	if ( mem->segment != X86_REG_INVALID ){
		//printf("S[%s]", cs_reg_name ( cs_handle, insn->detail->x86.operands[i].mem.segment ) );
		//throw "seg ment";
		if ( warnings && (WarningsSet & InvolvedSegmentRegister) ){
			InstrWasWarnings |= true;
			// You need make instruction simulatuion for group such load its register.
			CodeComment += ";Involved segment register, its value is unknown to me.";
		}
	}

	if ( ! _IsAddresValid )
		return false; 

	*Ret = Va;

	return true;
}

bool Cmps ( DWORD64 v, PBYTE p, size_t s )
{
	switch ( s ){
	case 0:throw "this size is imposible";
	case 1:return *((PBYTE)p) == (BYTE)v;
	case 2:return *((PWORD)p) == (WORD)v;
	case 3:throw "this size is imposible";
	case 4:return *((PDWORD)p) == (DWORD)v;
	case 5:throw "this size is imposible";
	case 6:throw "this size is imposible";
	case 7:throw "this size is imposible";
	case 8:return *((PDWORD64)p) == v;
	}
	throw "this size is imposible";
}

static CVIRTUAL_ADDRESS GetValue ( CVIRTUAL_ADDRESS * ptr, size_t s )
{
	switch ( s ){
	case 0:throw "this size is imposible";
	case 1:return CVIRTUAL_ADDRESS ( *(const __int8 *)ptr );
	case 2:return CVIRTUAL_ADDRESS ( *(const __int16 *)ptr );
	case 3:throw "this size is imposible";
	case 4:return CVIRTUAL_ADDRESS ( *(PDWORD32)ptr );
	case 5:throw "this size is imposible";
	case 6:throw "this size is imposible";
	case 7:throw "this size is imposible";
	case 8:return CVIRTUAL_ADDRESS ( *(PDWORD64)ptr );
	}
	throw "";
}
void CPU::ResetReg ( x86_reg reg )
{
	switch ( reg )
	{
	case X86_REG_AH:	   
	case X86_REG_AL:	   
	case X86_REG_AX:	   
	case X86_REG_EAX:	   
	case X86_REG_RAX:		rax.ValidValue= false;return;

	case X86_REG_BH:	  
	case X86_REG_BL:	  
	case X86_REG_BX:	  
	case X86_REG_EBX:	  
	case X86_REG_RBX:		rbx.ValidValue = false;return;

	case X86_REG_CH:	  
	case X86_REG_CL:	  
	case X86_REG_CX:	  
	case X86_REG_ECX:	  
	case X86_REG_RCX:		rcx.ValidValue = false;return;

	case X86_REG_DH:	   
	case X86_REG_DL:	   
	case X86_REG_DX:	   
	case X86_REG_EDX:	   
	case X86_REG_RDX:		rdx.ValidValue = false;return;

	case X86_REG_DI:      
	case X86_REG_EDI:	  
	case X86_REG_RDI:		rdi.ValidValue = false;return;

	case X86_REG_RSI:	   
	case X86_REG_ESI:	   
	case X86_REG_SI:		rsi.ValidValue = false;return;
	}
}
bool CPU::SetReg ( x86_reg reg, unsigned __int64 Val )
{
	switch ( reg )
	{
	case X86_REG_AH:	  rax.l32.l16.h8.v8	= (unsigned __int8 ) Val; rax.ValidValue = true;return true;
	case X86_REG_AL:	  rax.l32.l16.l8.v8	= (unsigned __int8 ) Val; rax.ValidValue = true;return true;
	case X86_REG_AX:	  rax.l32.l16.v16	= (unsigned __int16) Val; rax.ValidValue	= true;
	case X86_REG_EAX:	  rax.l32.v32		= (unsigned __int32) Val; rax.ValidValue		= true;return true;
	case X86_REG_RAX:	  rax.v64			=                    Val; rax.ValidValue			= true;return true;
						  
	case X86_REG_BH:	  rbx.l32.l16.h8.v8	= (unsigned __int8 ) Val; rbx.ValidValue = true;return true;
	case X86_REG_BL:	  rbx.l32.l16.l8.v8	= (unsigned __int8 ) Val; rbx.ValidValue = true;return true;
	case X86_REG_BX:	  rbx.l32.l16.v16	= (unsigned __int16) Val; rbx.ValidValue	= true;return true;
	case X86_REG_EBX:	  rbx.l32.v32		= (unsigned __int32) Val; rbx.ValidValue		= true;return true;
	case X86_REG_RBX:	  rbx.v64			=                    Val; rbx.ValidValue			= true;return true;
						  										
	case X86_REG_CH:	  rcx.l32.l16.h8.v8	= (unsigned __int8 ) Val; rcx.ValidValue = true;return true;
	case X86_REG_CL:	  rcx.l32.l16.l8.v8	= (unsigned __int8 ) Val; rcx.ValidValue = true;return true;
	case X86_REG_CX:	  rcx.l32.l16.v16	= (unsigned __int16) Val; rcx.ValidValue	= true;return true;
	case X86_REG_ECX:	  rcx.l32.v32		= (unsigned __int32) Val; rcx.ValidValue		= true;return true;
	case X86_REG_RCX:	  rcx.v64			=                    Val; rcx.ValidValue			= true;return true;
						 										
	case X86_REG_DH:	  rdx.l32.l16.h8.v8	= (unsigned __int8 ) Val; rdx.ValidValue = true;return true;
	case X86_REG_DL:	  rdx.l32.l16.l8.v8	= (unsigned __int8 ) Val; rdx.ValidValue = true;return true;
	case X86_REG_DX:	  rdx.l32.l16.v16	= (unsigned __int16) Val; rdx.ValidValue	= true;return true;
	case X86_REG_EDX:	  rdx.l32.v32		= (unsigned __int32) Val; rdx.ValidValue		= true;return true;
	case X86_REG_RDX:	  rdx.v64			=                    Val; rdx.ValidValue			= true;return true;
						 						 				
	case X86_REG_DI:      rdx.l32.l16.v16	= (unsigned __int16) Val; rdx.ValidValue	= true;return true;
	case X86_REG_EDI:	  rdx.l32.v32		= (unsigned __int32) Val; rdx.ValidValue		= true;return true;
	case X86_REG_RDI:	  rdx.v64			=                    Val; rdx.ValidValue			= true;return true;
						 										
	case X86_REG_RSI:	  rdx.l32.l16.v16	= (unsigned __int16) Val; rdx.ValidValue	= true;return true;
	case X86_REG_ESI:	  rdx.l32.v32		= (unsigned __int32) Val; rdx.ValidValue		= true;return true;
	case X86_REG_SI:	  rdx.v64			=                    Val; rdx.ValidValue			= true;return true;
	}
	return false;
}
bool  CPU::GetReg ( x86_reg reg, unsigned __int64 *Val )
{
	switch ( reg )
	{
	case X86_REG_AH:	 if ( ! rax.ValidValue ) return false;*Val = rax.l32.l16.h8.v8		; return true;
	case X86_REG_AL:	 if ( ! rax.ValidValue ) return false;*Val = rax.l32.l16.l8.v8		; return true;
	case X86_REG_AX:	 if ( ! rax.ValidValue ) return false;*Val = rax.l32.l16.v16		; return true;
	case X86_REG_EAX:	 if ( ! rax.ValidValue ) return false;*Val = rax.l32.v32			; return true;
	case X86_REG_RAX:	 if ( ! rax.ValidValue ) return false;*Val = rax.v64				; return true;

	case X86_REG_BH:	 if ( ! rbx.ValidValue ) return false;*Val = rbx.l32.l16.h8.v8		; return true;
	case X86_REG_BL:	 if ( ! rbx.ValidValue ) return false;*Val = rbx.l32.l16.l8.v8		; return true;
	case X86_REG_BX:	 if ( ! rbx.ValidValue ) return false;*Val = rbx.l32.l16.v16		; return true;
	case X86_REG_EBX:	 if ( ! rbx.ValidValue ) return false;*Val = rbx.l32.v32			; return true;
	case X86_REG_RBX:	 if ( ! rbx.ValidValue ) return false;*Val = rbx.v64				; return true;

	case X86_REG_CH:	 if ( ! rcx.ValidValue ) return false;*Val = rcx.l32.l16.h8.v8		; return true;
	case X86_REG_CL:	 if ( ! rcx.ValidValue ) return false;*Val = rcx.l32.l16.l8.v8		; return true;
	case X86_REG_CX:	 if ( ! rcx.ValidValue ) return false;*Val = rcx.l32.l16.v16		; return true;
	case X86_REG_ECX:	 if ( ! rcx.ValidValue ) return false;*Val = rcx.l32.v32			; return true;
	case X86_REG_RCX:	 if ( ! rcx.ValidValue ) return false;*Val = rcx.v64				; return true;

	case X86_REG_DH:	 if ( ! rdx.ValidValue ) return false;*Val = rdx.l32.l16.h8.v8		; return true;
	case X86_REG_DL:	 if ( ! rdx.ValidValue ) return false;*Val = rdx.l32.l16.l8.v8		; return true;
	case X86_REG_DX:	 if ( ! rdx.ValidValue ) return false;*Val = rdx.l32.l16.v16		; return true;
	case X86_REG_EDX:	 if ( ! rdx.ValidValue ) return false;*Val = rdx.l32.v32			; return true;
	case X86_REG_RDX:	 if ( ! rdx.ValidValue ) return false;*Val = rdx.v64				; return true;

	case X86_REG_DI:     if ( ! rdi.ValidValue ) return false;*Val = rdi.l32.l16.v16		; return true;
	case X86_REG_EDI:	 if ( ! rdi.ValidValue ) return false;*Val = rdi.l32.v32			; return true;
	case X86_REG_RDI:	 if ( ! rdi.ValidValue ) return false;*Val = rdi.v64				; return true;

	case X86_REG_RSI:	 if ( ! rsi.ValidValue ) return false;*Val = rsi.l32.l16.v16		; return true;
	case X86_REG_ESI:	 if ( ! rsi.ValidValue ) return false;*Val = rsi.l32.v32			; return true;
	case X86_REG_SI:	 if ( ! rsi.ValidValue ) return false;*Val = rsi.v64				; return true;
	}
	return false;
}

bool CODE_ANALIZE::GetOperand ( cs_insn *insn, cs_x86_op & op, unsigned __int64 * Value, bool MemAsImm )
{
	
	CVIRTUAL_ADDRESS Dest;
	bool Rip;

	switch ( op.type ){
		case X86_OP_MEM:
			if ( GetMemOperandAddress ( &op.mem, insn->address + insn->size, &Dest, &Rip, true ) ){
				if ( !MemAsImm ){
					CVIRTUAL_ADDRESS * AddrDest = (CVIRTUAL_ADDRESS *) Img->GetPtrFromVA ( Dest );
					if ( AddrDest ){
						CVIRTUAL_ADDRESS OpValue = GetValue ( AddrDest, op.size );	// need load by size
						*Value = OpValue.Get64 ();
						return true;
					}
				}
				else{
					*Value = Dest.Get64 ();
					return true;
				}
			}
			return false;

		case X86_OP_REG:	
			if ( !Cpu.GetReg ( op.reg, Value ) )
				return false;
			return true;
		case X86_OP_IMM:	
			*Value = op.imm; 
			return true;

		default:
			return false;
	}
}

void CODE_ANALIZE::Emulator ( cs_insn *insn )
{
	CVIRTUAL_ADDRESS Dest = 0;

	cs_x86 & x86 = insn->detail->x86; 

	if ( x86.operands[0].type != X86_OP_REG )
		return;
	
	unsigned __int64 op0;
	unsigned __int64 op1;

	switch ( insn->id ){
	case X86_INS_SUB:

		if ( !Cpu.GetReg ( x86.operands[0].reg, &op0 ) )
			return;
		if ( !GetOperand ( insn, x86.operands[1], &op1, false ) )
			return;

		op0 = op0 - op1;
		Cpu.SetReg ( x86.operands[0].reg, op0 );
		return;

	case X86_INS_ADD:
	
		if ( !Cpu.GetReg ( x86.operands[0].reg, &op0 ) )
			return;
		if ( !GetOperand ( insn, x86.operands[1], &op1, false ) )
			return;

		op0 = op0 + op1;
		Cpu.SetReg ( x86.operands[0].reg, op0 );
		return;

	case X86_INS_LEA:
		if ( !GetOperand ( insn, x86.operands[1], &op1, true ) )
			return;
		Cpu.SetReg ( x86.operands[0].reg, op1 );
		return;

	case X86_INS_MOV:
		if ( !GetOperand ( insn, x86.operands[1], &op1, false ) )
			return;
		Cpu.SetReg ( x86.operands[0].reg, op1 );
		return;
	default:
		Cpu.ResetReg ( x86.operands[0].reg );
		return;
	}
}

// do analize of operands 
//
void CODE_ANALIZE::ProcessOperands ( cs_insn *insn, const BYTE * Place, CSECTION * InstrSection, ProcessOperandsOptions Opts, CREFERENCE * Refs, size_t * _RefsCount )
{

	CVIRTUAL_ADDRESS Dest = 0;

	cs_x86 & x86 = insn->detail->x86; 

	size_t rc = 0;
	
	bool UseMemRip = false;// declare here because of the warnings in a switch. See case X86_OP_MEM

	for ( size_t i=0; i< x86.op_count; i++ ){
//		printf("  Op %d:",i);
		switch ( x86.operands[i].type ){

			case X86_OP_REG:
	//			printf("Reg [%s]", cs_reg_name( cs_handle, insn->detail->x86.operands[i].reg ) );
				break;

			case X86_OP_IMM:
	//			printf ( "Imm[%08X]", insn->detail->x86.operands[i].imm );
				
				Dest = x86.operands[i].imm;
				
				if ( IsAddressValid ( Dest )  ){ // crazy check - suddenly is it an address?
					CSECTION * s = Img->GetSectionByVA ( Dest );
					if ( s != 0 ){
						if ( s != InstrSection || Opts & DoReferenceInsideOneSections ){
							char msg[256];
							sprintf_s ( msg,  ";Ref to: %8.8s:%016I64X", s->Record.Name , Dest.Get64() );
							CodeComment += msg;
							
							if ( x86.HasImmediate )
								RefFromImmediate (Refs[rc],x86, Place, insn );//	printf("\n immediateOffset ");
							else
								throw "Oops - too strange";

							_ASSERTE ( rc < 2);

							Refs[rc].TargetVa = Dest;
							Refs[rc].Operation = CREFERENCE::Offset;
							Refs[rc].TargetSize = x86.operands[i].size;
							Refs[rc].Place.Rip = false;
							Refs[rc].Place.RipBase = 0;
							rc++;
							
						}
					}
				}

				break;
			case X86_OP_MEM:
				if ( GetMemOperandAddress ( &x86.operands[i].mem, insn->address + insn->size, &Dest, &UseMemRip, true ) ){
					CSECTION * s = Img->GetSectionByVA ( Dest );
					
					if ( s != InstrSection || Opts & DoReferenceInsideOneSections ){
						// references add only from memory
						// reference from register value jet not implemented
						if ( x86.operands[i].mem.disp ){
							char msg[256];

							if ( s == 0 ){
								sprintf_s ( msg, ";Ref out of all section: %016I64X", Dest.Get64 () );
								InstrWasWarnings |= true;
							} else{
								sprintf_s ( msg, ";Ref to: %8.8s:%016I64X", s->Record.Name, Dest.Get64 () );
							}

							CodeComment += msg;

							_ASSERTE ( rc < 2 );

							Refs[rc].TargetSize = x86.operands[i].size;
							Refs[rc].Operation = OperationTypesCvt ( (cs_ac_type) x86.operands[i].access );
							Refs[rc].TargetVa = Dest;
							Refs[rc].Place.Rip = UseMemRip;
							Refs[rc].Place.RipBase = insn->address + insn->size;


							if ( !x86.HasDisplacement ){
								if ( x86.HasImmediate ){
									RefFromImmediate ( Refs[rc], x86, Place, insn );
									_ASSERTE ( Cmps ( x86.operands[i].mem.disp, (PBYTE) Place + x86.immediateOffset, x86.immediateSize ) );
								} else{
									throw "both not!, but op displ exist!";
								}
							} else{
								RefFromDisplacement ( Refs[rc], x86, Place, insn );
								_ASSERTE ( Cmps ( x86.operands[i].mem.disp, (PBYTE) Place + x86.displacementOffset, x86.displacementSize ) );
							}

							// add ref
							rc++;

						}
					}
				}

				break;
		}
	}

	*_RefsCount = rc;

}

// return true if after this interrupt need terminate the analysis
bool CODE_ANALIZE::IsTerminateInterrupt ( cs_insn *insn )
{
	cs_x86 & x86 = insn->detail->x86; 

	if ( x86.op_count > 1 )
		throw "too mach"; // too mach for 'int xx'

	if ( x86.op_count == 1 ){
		if ( x86.operands[0].type == X86_OP_IMM ){
			BYTE imm = (BYTE)x86.operands[0].imm;

			for ( auto i:NoTerminateInterrupts ){
				if ( i == imm )
					return false;
			}

			return true;
		}
		else{
			throw "need do code for this";
		}
	}

	// 0 

	if ( insn->detail->x86.opcode[0] == 0xCC )
		return false;

	throw "need do code for this";
}

bool CODE_ANALIZE::GetCallJmpDestinationAddress (  cs_insn *insn, size_t n, CVIRTUAL_ADDRESS * RetVaDest )
{
	if ( n >= insn->detail->x86.op_count )
		throw "too mach";

	CVIRTUAL_ADDRESS VaDest = 0; // ret value

	CVIRTUAL_ADDRESS VaAddrDest = 0;

	cs_x86_op * op = &insn->detail->x86.operands[n];

	bool Ret; // do not jump
	unsigned __int64 RegValue;
	switch ( op->type ){

		case X86_OP_REG:
			if ( !Cpu.GetReg ( op->reg, &RegValue  ) ){
				if ( WarningsSet & UseRegisterInCallJump ){
					CodeComment += ";Posible need manual analize -- there use reg as jump/call operand (UseRegisterInCallJump)";
					InstrWasWarnings |= true;
				}
				Ret = false;
			}
			VaDest = RegValue;
			Ret = Img->IsAddressCanExecute ( VaDest );
			break;

		case X86_OP_IMM:
			VaDest = op->imm;
			Ret = Img->IsAddressCanExecute ( VaDest );
			break;

		case X86_OP_MEM: // call/jump via prt in memory
			if ( op->mem.base !=  X86_REG_INVALID ){
				if ( op->mem.base !=  X86_REG_RIP ){
					if ( WarningsSet & BadBaseInCallJump ){
						CodeComment += ";Posible need manual analize -- there use base reg different then IP, as jump/call operand (BadBaseInCallJump)";
						InstrWasWarnings |= true;
					}
					return false; //throw "base not rip";
				}
				VaAddrDest = CVIRTUAL_ADDRESS ( insn->address + insn->size );
			}

			if (op->mem.disp == 0 ){
				if ( WarningsSet & BadDisplInCallJump ) {
					CodeComment += ";Posible need manual analize -- I don know as get target address of jump/call operand (BadDisplInCallJump)";
					InstrWasWarnings |= true;
				}
				return false; //throw "Displacement == 0 ";
			}

			VaAddrDest = VaAddrDest + CRELAITIVE_VIRTUAL_ADDRESS( op->mem.disp );
			CRELAITIVE_VIRTUAL_ADDRESS RVaAddrDest = Img->GetRvaFromVA ( VaAddrDest );

			if ( RVaAddrDest >= iat->VirtualAddress && RVaAddrDest < iat->VirtualAddress + iat->Size ){ // was =
				CIMPORTS::IMPORT * f =  Img->GetImportFuncByIatOffset ( CRELAITIVE_VIRTUAL_ADDRESS::Get32 ( RVaAddrDest - iat->VirtualAddress) );
				char msg[256];
				
				sprintf_s ( msg, ";%s::%s",  f->Dll->DllName->data(), f->GetFuncName() );
				
				CodeComment += msg;
				VaDest = 0; // this import no need goto 
				Ret = false;
			}
			else {
				
				CVIRTUAL_ADDRESS * AddrDest = (CVIRTUAL_ADDRESS *)Img->GetPtrFromVA ( VaAddrDest );
				// if valid ok
				if ( AddrDest ){ 
					//throw "this need to check at 64 and 32 bits";
					VaDest = GetValue ( AddrDest, op->size );	// need load by size
					Ret = Img->IsAddressCanExecute ( VaDest );//
				}
				else{// else problems
					if (  WarningsSet & CallJumpToJunk ){
						CodeComment += ";Posible need manual analize -- Jump/call to junk";
						InstrWasWarnings |= true;
					}
					VaDest = 0; // ret 0 and no goto
					Ret = false;
				}
			}
			break;
	}
	
	*RetVaDest = VaDest;
	
	return Ret;
}
#define MAX_INSTRUCTION_SIZE 16
bool CODE_ANALIZE::Analize ( CVIRTUAL_ADDRESS StartVa, bool Sub )
{

	if ( IsAddressWasAnalized ( StartVa ) )
		return true;

	_ASSERTE ( FindLabel ( StartVa ) == 0 );

	CODELABEL * nl = new CODELABEL ( StartVa, Sub );

	AddCodeLable ( nl );

	const BYTE * Place = (PBYTE)Img->GetPtrFromVA ( nl->Va );

	if ( ! Place )
		throw "invalid va";
	
	cs_insn *insn = cs_malloc ( cs_handle );

	size_t code_size = MAX_INSTRUCTION_SIZE;
	
	if ( DisAssembleProduce )
		if ( Sub )
			printf ( "\n--Sub--"); 
		else
			printf ( "\n--Lab--"); 
	
	Cpu.Reset ();
	
	CVIRTUAL_ADDRESS address = StartVa; 

	bool AnalizeStop = false;
	CVIRTUAL_ADDRESS Dest = 0;

	CSECTION * CodeSection = Img->GetSectionByVA ( StartVa );

	char msg[256];

	size_t InstrRefsCount=0;
	CREFERENCE InstrRefs[5];// need only two - but asserts stay here, - then to get a little bit space left :)

	while ( 1 ) {
 
		if ( ! cs_disasm_iter ( cs_handle, &Place, &code_size, (uint64_t*)&address, insn) ){
			printf("Disassembly failed");
			return true;
		}

		InstrWasWarnings = false;
		InstrWasError = false;

		CleanPrintOut();

		sprintf_s ( msg,  "0x%I64X:\t%s\t%s", insn->address, insn->mnemonic, insn->op_str );
		DiassembleText = msg;

		SetAddressWasAnalized  ( insn->address, insn->size );

		ProcessOperands ( insn, Place - insn->size , CodeSection,  DoReferenceInsideOneSections, InstrRefs, &InstrRefsCount );

		Emulator ( insn );

		if ( insn->detail->groups_count ){

			for ( size_t n = 0; n < insn->detail->groups_count && !AnalizeStop; n++ ) {
			
				switch ( insn->detail->groups[n] ){
				case CS_GRP_JUMP:
					
					if ( GetCallJmpDestinationAddress ( insn, 0, &Dest ) ){
						if ( InstrRefsCount ){
							InstrRefs[0].Type = CREFERENCE::Jump;
							InstrRefs[0].Operation = CREFERENCE::Offset;
							// 
							InstrRefs[0].Place.RipBase = address;
							InstrRefs[0].Place.Rip=true;
						} 
						_ASSERTE ( InstrRefsCount <= 1);
						PrintOut();
						Analize ( Dest, false );
						CleanPrintOut();
					}
					
					// if instruction is 'unconditional jump',
					// then next command, after this is never execute.
					// More precisely, from this thread it will never ever be executed, 
					// but the jump to next instruction from the other thread of course possible.
					// But for us, it's now, it does not matter. 
					// Other words, the continue analysis will stoped.
					if ( insn->id == X86_INS_JMP )// if instruction is unconditional 
						AnalizeStop = true;
					
					break;
				case CS_GRP_CALL:

					if ( GetCallJmpDestinationAddress ( insn, 0, &Dest ) ){
						if ( InstrRefsCount ){
							InstrRefs[0].Type = CREFERENCE::Call;
							InstrRefs[0].Operation = CREFERENCE::Offset;
							//
							InstrRefs[0].Place.RipBase = address;
							InstrRefs[0].Place.Rip=true;
						}
						_ASSERTE ( InstrRefsCount <= 1);

						PrintOut();
						Analize ( Dest, true );
						CleanPrintOut();
					}
					break;
				case CS_GRP_RET:

					AnalizeStop = true;
					break;

					// Oh, yeah, this's was a great mistake!
					// Terminate analysis by 'INT' instruction!!
					// 
				case CS_GRP_INT:

					if ( IsTerminateInterrupt(insn) )
						AnalizeStop = true;
					break;
				default:
					_ASSERTE ( InstrRefsCount <= 2 );
					for ( size_t i=0; i<InstrRefsCount;i++ ){
						InstrRefs[i].Type =  CREFERENCE::Data;
					}
					break;
				}
			}
		}
		else{ // gropes
			 _ASSERTE ( InstrRefsCount <= 2 );
			 for ( size_t i=0; i<InstrRefsCount;i++ ){
				 InstrRefs[i].Type =  CREFERENCE::Data;
			 }
		}

		for ( size_t i=0; i<InstrRefsCount; i++ ){
			 References.push_back ( InstrRefs[i] );
		}

		PrintOut();

		WasError |= InstrWasError;
		WasWarnings |= InstrWasWarnings;

		code_size=MAX_INSTRUCTION_SIZE;

		if ( AnalizeStop )
			break;

		if ( IsAddressWasAnalized ( address ) )
			break;
	}
	if ( DisAssembleProduce )
		printf ( "\n--");
	 
	cs_free(insn, 1);
	return true;
}
void CODE_ANALIZE::PrintOut ( void )
{
	if ( DisAssembleProduce || InstrWasWarnings || InstrWasError ){
		if ( ! DiassembleText.empty() )
			printf ( "\n%s", DiassembleText.data() );
		if ( ! CodeComment.empty() )
			printf ( "\t\t%s", CodeComment.data() );
	}
}
void CODE_ANALIZE::CleanPrintOut ( void )
{
	DiassembleText.clear();
	CodeComment.clear();
}
