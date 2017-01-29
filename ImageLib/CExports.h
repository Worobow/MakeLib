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

		CExports.h, CExports.cpp

	Environment:

		User-mode command line

	Comment:

		class for the implementation of functional work exports

--*/

class CEXPORTS 
{

public:

	typedef struct _EXPORT{
		char * Name;			// name of export func
		CRELAITIVE_VIRTUAL_ADDRESS VirtualAddress;	//RVA function start 
	}EXPORT,*PEXPORT;

private:

    vector<EXPORT> Exports;
    
public:

	char * Name;

	DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    
    DWORD   Base;

	bool Pack ( CSECTION * sec ,  CIMAGE * img, bool PrintDebugDetails );
	bool PackOld (  CSECTION * sec, CIMAGE * img );
	bool Create ( CIMAGE * img, bool PrintDetailInfo );
	size_t Size ()
	{
		return Exports.size();
	}
	EXPORT Get ( size_t i )
	{
		return Exports[i];
	}
	const vector <EXPORT> & GetAllExports ( void )
	{
		return Exports;
	}
	void Add ( char * name, CRELAITIVE_VIRTUAL_ADDRESS va )
	{
		EXPORT n;
		n.Name = name;
		n.VirtualAddress = va;
		Exports.push_back(n);
	}
	void Add ( const EXPORT &n )
	{
		Exports.push_back(n);
	}
};