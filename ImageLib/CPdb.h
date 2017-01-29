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

		CPdb.h, CPdb.cpp

	Environment:
		
		User-mode command line

	Comment:

		class for work with PDB 

--*/



class CPDB
{

public:

	class CSECTION {
		
	public:

		CRELAITIVE_VIRTUAL_ADDRESS dwRVA;
		DWORD dwSect;
		DWORD dwOffset;
		DWORD dwLen;
		CVIRTUAL_ADDRESS qwVA;

		wstring Modulename;
		// есть еще и идентификатор для компилянта 
	
		CSECTION ( DWORD s, DWORD off, DWORD l, DWORD rva, ULONGLONG va, wstring & m )
			:dwRVA(rva),dwSect(s),dwOffset(off),dwLen(l),qwVA(va),Modulename(m)
		{}
	};

	class CSYMBOL {

	public:

		DWORD SymTag;
		
		CRELAITIVE_VIRTUAL_ADDRESS RVA;
		DWORD Seg;
		DWORD Off;

		wstring Name;
		wstring Undname;

		ULONGLONG Lenght;
		CVIRTUAL_ADDRESS VA;

		BOOL Code;
		BOOL Function;
		
		CSYMBOL ( wstring & n, wstring & un, DWORD t, DWORD rva, DWORD s, DWORD of, ULONGLONG l, ULONGLONG va, BOOL code, BOOL funcs )
			:SymTag(t), RVA(rva), Seg(s), Off(of), Lenght(l), VA(va), Name(n), Undname(un), Code(code),Function(funcs)
		{}
	};

private:

	IDiaDataSource * Source;
	IDiaSession *	Session;
	IDiaSymbol *	Global;

	DWORD MachineType = CV_CFL_80386;
	HRESULT GetTable ( IDiaSession *pSession, REFIID iid, void **ppUnk );
	void LoadSection ( IDiaSectionContrib *SecContrib );
	void AddPublicSimbol ( IDiaSymbol *Symbol );

public:

	vector <wstring*> Modules;
	vector<CSECTION*> Sections;
	//vector<CSYMBOL*> PubSymbols;
	map<class CRELAITIVE_VIRTUAL_ADDRESS,CSYMBOL*>PubSymbols;

	CPDB ()
	{

		Source = 0;

		HRESULT hr = CoInitialize ( NULL );

		hr = CoCreateInstance ( __uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER, __uuidof(IDiaDataSource), (LPVOID*)&Source);

		if ( FAILED ( hr ) ) 
			throw "CoCreateInstance failed";

		Session = 0;
		Global = 0;

	}
	~CPDB()
	{
		if ( Global ) 
			Global->Release();

		if (Session) 
			Session->Release();
    
		CoUninitialize();
	}
	void LoadAllSections ( void );
	void LoadAllModules ( void );
	
	/*
		switch ( dwSymTag ){
			case SymTagNull:
			case SymTagExe:
			case SymTagCompiland:
			case SymTagCompilandDetails:
			case SymTagCompilandEnv:
			case SymTagAnnotation:
			case SymTagLabel:
			case SymTagUDT:
			case SymTagEnum:
			case SymTagFunctionType:
			case SymTagPointerType:
			case SymTagArrayType:
			case SymTagBaseType:
			case SymTagTypedef:
			case SymTagBaseClass:
			case SymTagFriend:
			case SymTagFunctionArgType:
			case SymTagFuncDebugStart:
			case SymTagFuncDebugEnd:
			case SymTagUsingNamespace:
			case SymTagVTableShape:
			case SymTagVTable:
			case SymTagCustom:
			case SymTagCustomType:
			case SymTagManagedType:
			case SymTagDimension:
			case SymTagCallSite:
			case SymTagInlineSite:
			case SymTagBaseInterface:
			case SymTagVectorType:
			case SymTagMatrixType:
			case SymTagHLSLType:
			case SymTagCaller:
			case SymTagCallee:
			case SymTagMax:
										
										break;
	
	*/
	void LoadAllPublicSymbols ( void );
	void Load ( const wstring & file );

	

#if 0

	void Save ( const wstring & file )
	{
		if ( ! initMsPdb () )
			throw "cannot load PDB helper DLL";

		mspdb::PDB *pdb;
		mspdb::DBI *dbi;
		mspdb::TPI *tpi;
		mspdb::Mod *mod;

		pdb = CreatePDB ( file.data() );

		if ( ! pdb )
			throw  "cannot create PDB file";

		int rc = pdb->CreateDBI("", &dbi);

		if ( rc <= 0 || ! dbi )
			throw "cannot create DBI";

		rc= dbi->OpenMod ("","",&mod);

		if ( rc <= 0 || ! mod )
			throw "cannot create MOD";

		rc = pdb->OpenTpi("", &tpi);

		if (rc <= 0 || !tpi)
			throw "cannot create TPI";

		if ( mod )
			mod->Close();
		if (dbi)
			dbi->Close();
		if (tpi)
			tpi->Close();
		if (pdb)
			pdb->Commit();
		if (pdb)
			pdb->Close();


	}
#endif
};