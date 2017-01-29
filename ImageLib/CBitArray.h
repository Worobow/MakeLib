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
			
			CBitarray.cpp, h

	Environment:
		
		User-mode, c++
	
	Comment:

--*/


//Easy class implementation a bits-size-mapping marks.
//Use as marks where code analize was already done.


//This class still need because in debug version of std::vector<bool> use not a bits for each item array but bytes.
//But and in debuging time yet still a life. 

class CBITARRAY 
{
	PBYTE Buffer;
	size_t _Size;

public:

	CBITARRAY(size_t s)
		:Buffer(0)
	{
		_Size = s;
		size_t ByteSize = s/8 + 1;
		Buffer = new BYTE[ByteSize];
		memset(Buffer,0,ByteSize);
	}
	~CBITARRAY()
	{
		if ( Buffer )
			delete Buffer;
	}
	// return bitsize 
	size_t Size ()
	{
		return _Size;
	}
	// return value of bit by index
	bool Test ( size_t i )
	{
		size_t bi = i / 8;
		size_t zi = i % 8;
		return ( Buffer[bi] & ( 1 << zi ) ) != 0 ;
	}
	// set true by index 
	void Set ( size_t i )
	{
		size_t bi = i / 8;
		size_t zi = i % 8;
		Buffer[bi] = Buffer[bi] | ( 1 << zi );
	}
	// set false by index
	void Clear ( size_t i )
	{
		size_t bi = i / 8;
		size_t zi = i % 8;
		Buffer[bi] = Buffer[bi] & ~( 1 << zi );
	}
};