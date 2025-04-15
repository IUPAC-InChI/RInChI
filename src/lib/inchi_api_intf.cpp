#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <inchi_api_intf.h>

#include <string>
#include <stdexcept>

#include <rinchi_platform.h>
#include <rinchi_utils.h>

/** PLATFORMS BEGIN **/

/** PLATFORM: WINDOWS **/
#if defined(ON_WINDOWS)

	#include <windows.h>

	#define inchi_lib_handle_type HINSTANCE
	#define DO_INCHI_LOAD_LIBRARY LoadLibraryA(lib_filename.c_str())
	#define DO_INCHI_UNLOAD_LIBRARY FreeLibrary
	#define LIB_INCHI_RESOLVE_ADDRESS GetProcAddress

	const char* LIB_INCHI_FILENAME = "libinchi.dll";

	const size_t LIB_INCHI_ERRMSG_BUF_SIZE = 1024;
	char lib_inchi_errmsg[LIB_INCHI_ERRMSG_BUF_SIZE];

	#ifdef RINCHI_LOAD_INCHI_DYNAMICALLY
	const char* lib_inchi_sys_errmsg()
	{
		if (FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
			GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			lib_inchi_errmsg, LIB_INCHI_ERRMSG_BUF_SIZE, NULL
		) == 0)
			throw std::runtime_error ("Unable to retrieve OS error message");
		return lib_inchi_errmsg;
	}
	#endif

/** PLATFORM: LINUX + SOLARIS **/
#elif defined(ON_LINUX) || defined(ON_SOLARIS)

	#include <dlfcn.h>

	#define inchi_lib_handle_type void*
	#define DO_INCHI_LOAD_LIBRARY dlopen(lib_filename.c_str(), RTLD_NOW)
	#define DO_INCHI_UNLOAD_LIBRARY dlclose
	#define LIB_INCHI_RESOLVE_ADDRESS dlsym

	const char* LIB_INCHI_FILENAME = "libinchi.so.1.04.00";

#ifdef RINCHI_LOAD_INCHI_DYNAMICALLY
	const char* lib_inchi_sys_errmsg() { return dlerror(); }
#endif

/** PLATFORM: UNDEFINED **/
#else
	Unsupported_platform
#endif

/** PLATFORMS END **/

namespace rinchi {

#if defined(ON_WINDOWS)
	const char* INCHI_INPUT_OPTION_CHIRAL_FLAG_ON  = "/WarnOnEmptyStructure /ChiralFlagON";
	const char* INCHI_INPUT_OPTION_CHIRAL_FLAG_OFF = "/WarnOnEmptyStructure /ChiralFlagOFF";

	const char* INCHI_OUTPUT_OPTION_SDF                 = "/WarnOnEmptyStructure /OutputSDF";
	const char* INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_ON  = "/WarnOnEmptyStructure /OutputSDF /SUCF /ChiralFlagON";
	const char* INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_OFF = "/WarnOnEmptyStructure /OutputSDF /SUCF /ChiralFlagOFF";
#elif defined(ON_LINUX) || defined(ON_SOLARIS)
	const char* INCHI_INPUT_OPTION_CHIRAL_FLAG_ON  = "-WarnOnEmptyStructure -ChiralFlagON";
	const char* INCHI_INPUT_OPTION_CHIRAL_FLAG_OFF = "-WarnOnEmptyStructure -ChiralFlagOFF";

	const char* INCHI_OUTPUT_OPTION_SDF = "-WarnOnEmptyStructure -OutputSDF";
	const char* INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_ON  = "-WarnOnEmptyStructure -OutputSDF -SUCF -ChiralFlagON";
	const char* INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_OFF = "-WarnOnEmptyStructure -OutputSDF -SUCF -ChiralFlagOFF";
#else
	Unsupported_platform
#endif

#ifdef RINCHI_LOAD_INCHI_DYNAMICALLY

std::string inchi_lib_path;
char inchi_lib_path_c_str[INCHI_LIB_PATH_MAX] = "";

inchi_lib_handle_type inchi_lib_handle = 0;

// Lazy-load function forward definitions.
INCHIGEN_HANDLE lazyload_INCHIGEN_Create();
int lazyload_INCHIGEN_Setup( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Input * pInp);
int lazyload_INCHIGEN_DoNormalization( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData );
int lazyload_INCHIGEN_DoCanonicalization( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData );
int lazyload_INCHIGEN_DoSerialization(INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults );
void lazyload_INCHIGEN_Reset( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults);
void lazyload_INCHIGEN_Destroy( INCHIGEN_HANDLE HGen );
//
int lazyload_GetINCHIfromINCHI(inchi_InputINCHI *inpInChI, inchi_Output *out);
int lazyload_GetINCHIKeyFromINCHI(const char* szINCHISource, const int xtra1, const int xtra2, char* szINCHIKey, char* szXtra1, char* szXtra2);
//
int lazyload_GetStructFromINCHI(inchi_InputINCHI* inpInChI, inchi_OutputStruct* outStruct);
void lazyload_FreeStructFromINCHI( inchi_OutputStruct* out );
int lazyload_GetINCHI(inchi_Input *inp, inchi_Output *out);
void lazyload_FreeINCHI(inchi_Output *out);

// int INCHI_DECL Get_inchi_Input_FromAuxInfo(
//	char *szInchiAuxInfo, int bDoNotAddH, int bDiffUnkUndfStereo, InchiInpData *pInchiInp )
int lazyload_Get_inchi_Input_FromAuxInfo(char *szInchiAuxInfo, int bDoNotAddH, int bDiffUnkUndfStereo, InchiInpData *pInchiInp );

// void INCHI_DECL Free_inchi_Input( inchi_Input *pInp )
void lazyload_Free_inchi_Input( inchi_Input *pInp );

void load_inchi_library()
{
	if (inchi_lib_path.length() == 0) {
		inchi_lib_path = inchi_lib_path_c_str;
		// Strip filename from C-style path - it will contain a module filename if it is set by a DLL init function.
		size_t sep_pos = inchi_lib_path.find_last_of(DIR_SEPARATOR);
		if (sep_pos != std::string::npos)
			inchi_lib_path.erase(sep_pos + 1);
		else
			inchi_lib_path = "";
	}

	// Load library and resolve function addresses.
	std::string lib_filename (inchi_lib_path + LIB_INCHI_FILENAME);
	inchi_lib_handle = DO_INCHI_LOAD_LIBRARY;
	if (inchi_lib_handle == 0) {
		std::string errmsg = std::string("Unable to load InChI library: ") + lib_inchi_sys_errmsg() + ".";
		// Give it a second chance, falling back to default library load path.
		lib_filename = LIB_INCHI_FILENAME;
		inchi_lib_handle = DO_INCHI_LOAD_LIBRARY;
		if (inchi_lib_handle == 0)
			throw std::runtime_error (errmsg);
	}

	// The casting of the return value from LIB_INCHI_RESOLVE_ADDRESS is necessary on Windows.
	lib_INCHIGEN_Create				= (INCHIGEN_Create_funcptr)	LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "INCHIGEN_Create");
	lib_INCHIGEN_Setup				= (INCHIGEN_Setup_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "INCHIGEN_Setup");
	lib_INCHIGEN_DoNormalization	= (INCHIGEN_DoNormalization_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "INCHIGEN_DoNormalization");
	lib_INCHIGEN_DoCanonicalization	= (INCHIGEN_DoCanonicalization_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "INCHIGEN_DoCanonicalization");
	lib_INCHIGEN_DoSerialization	= (INCHIGEN_DoSerialization_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "INCHIGEN_DoSerialization");
	lib_INCHIGEN_Reset				= (INCHIGEN_Reset_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "INCHIGEN_Reset");
	lib_INCHIGEN_Destroy			= (INCHIGEN_Destroy_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "INCHIGEN_Destroy");
	//
	lib_GetINCHIfromINCHI			= (GetINCHIfromINCHI_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "GetINCHIfromINCHI");
	lib_GetINCHIKeyFromINCHI		= (GetINCHIKeyFromINCHI_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "GetINCHIKeyFromINCHI");
	//
	lib_GetStructFromINCHI          = (GetStructFromINCHI_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "GetStructFromINCHI");
	lib_FreeStructFromINCHI         = (FreeStructFromINCHI_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "FreeStructFromINCHI");
	lib_GetINCHI                    = (GetINCHI_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "GetINCHI");
	lib_FreeINCHI                   = (FreeINCHI_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "FreeINCHI");
	lib_Get_inchi_Input_FromAuxInfo = (Get_inchi_Input_FromAuxInfo_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "Get_inchi_Input_FromAuxInfo");
	lib_Free_inchi_Input            = (Free_inchi_Input_funcptr) LIB_INCHI_RESOLVE_ADDRESS(inchi_lib_handle, "Free_inchi_Input");

	if (
		lib_INCHIGEN_Create == 0 ||
		lib_INCHIGEN_Setup == 0 ||
		lib_INCHIGEN_DoNormalization == 0 ||
		lib_INCHIGEN_DoCanonicalization == 0 ||
		lib_INCHIGEN_DoSerialization == 0 ||
		lib_INCHIGEN_Reset == 0 ||
		lib_INCHIGEN_Destroy == 0 ||
		//
		lib_GetINCHIfromINCHI == 0 ||
		lib_GetINCHIKeyFromINCHI == 0 ||
		//
		lib_GetStructFromINCHI == 0 ||
		lib_FreeStructFromINCHI == 0 ||
		lib_GetINCHI == 0 ||
		lib_FreeINCHI == 0 ||
		lib_Get_inchi_Input_FromAuxInfo == 0 ||
		lib_Free_inchi_Input == 0
	) {
		DO_INCHI_UNLOAD_LIBRARY(inchi_lib_handle);
		inchi_lib_handle = 0;

		lib_INCHIGEN_Create = lazyload_INCHIGEN_Create;
		lib_INCHIGEN_Setup = lazyload_INCHIGEN_Setup;
		lib_INCHIGEN_DoNormalization = lazyload_INCHIGEN_DoNormalization;
		lib_INCHIGEN_DoCanonicalization = lazyload_INCHIGEN_DoCanonicalization;
		lib_INCHIGEN_DoSerialization = lazyload_INCHIGEN_DoSerialization;
		lib_INCHIGEN_Reset = lazyload_INCHIGEN_Reset;
		lib_INCHIGEN_Destroy = lazyload_INCHIGEN_Destroy;
		//
		lib_GetINCHIfromINCHI = lazyload_GetINCHIfromINCHI;
		lib_GetINCHIKeyFromINCHI = lazyload_GetINCHIKeyFromINCHI;
		//
		lib_GetStructFromINCHI = lazyload_GetStructFromINCHI;
		lib_FreeStructFromINCHI = lazyload_FreeStructFromINCHI;
		lib_GetINCHI = lazyload_GetINCHI;
		lib_FreeINCHI = lazyload_FreeINCHI;
		lib_Get_inchi_Input_FromAuxInfo = lazyload_Get_inchi_Input_FromAuxInfo;
		lib_Free_inchi_Input = lazyload_Free_inchi_Input;

		throw std::runtime_error ("The InChI library was successfully loaded, but it does not contain all the required functions.");
	}
}

/**
  Lazy-load-implementing functions follow.
**/
INCHIGEN_HANDLE lazyload_INCHIGEN_Create()
{
	load_inchi_library();
	return lib_INCHIGEN_Create();
}
INCHIGEN_Create_funcptr lib_INCHIGEN_Create = lazyload_INCHIGEN_Create;

int lazyload_INCHIGEN_Setup( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Input * pInp)
{
	load_inchi_library();
	return lib_INCHIGEN_Setup(HGen, pGenData, pInp);
}
INCHIGEN_Setup_funcptr lib_INCHIGEN_Setup = lazyload_INCHIGEN_Setup;

int lazyload_INCHIGEN_DoNormalization( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData )
{
	load_inchi_library();
	return lib_INCHIGEN_DoNormalization(HGen, pGenData);
}
INCHIGEN_DoNormalization_funcptr lib_INCHIGEN_DoNormalization = lazyload_INCHIGEN_DoNormalization;

int lazyload_INCHIGEN_DoCanonicalization( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData )
{
	load_inchi_library();
	return lib_INCHIGEN_DoCanonicalization(HGen, pGenData);
}
INCHIGEN_DoCanonicalization_funcptr lib_INCHIGEN_DoCanonicalization = lazyload_INCHIGEN_DoCanonicalization;

int lazyload_INCHIGEN_DoSerialization(INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults )
{
	load_inchi_library();
	return lib_INCHIGEN_DoSerialization(HGen, pGenData, pResults);
}
INCHIGEN_DoSerialization_funcptr lib_INCHIGEN_DoSerialization = lazyload_INCHIGEN_DoSerialization;

void lazyload_INCHIGEN_Reset( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults)
{
	load_inchi_library();
	return lib_INCHIGEN_Reset(HGen, pGenData, pResults);
}
INCHIGEN_Reset_funcptr lib_INCHIGEN_Reset = lazyload_INCHIGEN_Reset;

void lazyload_INCHIGEN_Destroy( INCHIGEN_HANDLE HGen )
{
	load_inchi_library();
	return lib_INCHIGEN_Destroy(HGen);
}
INCHIGEN_Destroy_funcptr lib_INCHIGEN_Destroy = lazyload_INCHIGEN_Destroy;

int lazyload_GetINCHIfromINCHI(inchi_InputINCHI *inpInChI, inchi_Output *out)
{
	load_inchi_library();
	return lib_GetINCHIfromINCHI(inpInChI, out);
}
GetINCHIfromINCHI_funcptr lib_GetINCHIfromINCHI = lazyload_GetINCHIfromINCHI;

int lazyload_GetINCHIKeyFromINCHI(const char* szINCHISource, const int xtra1, const int xtra2, char* szINCHIKey, char* szXtra1, char* szXtra2)
{
	load_inchi_library();
	return lib_GetINCHIKeyFromINCHI(szINCHISource, xtra1, xtra2, szINCHIKey, szXtra1, szXtra2);
}
GetINCHIKeyFromINCHI_funcptr lib_GetINCHIKeyFromINCHI = lazyload_GetINCHIKeyFromINCHI;

int lazyload_GetStructFromINCHI(inchi_InputINCHI* inpInChI, inchi_OutputStruct* outStruct)
{
	load_inchi_library();
	return lib_GetStructFromINCHI(inpInChI, outStruct);
}
GetStructFromINCHI_funcptr lib_GetStructFromINCHI = lazyload_GetStructFromINCHI;

void lazyload_FreeStructFromINCHI( inchi_OutputStruct* out )
{
	load_inchi_library();
	return lib_FreeStructFromINCHI( out );
}
FreeStructFromINCHI_funcptr lib_FreeStructFromINCHI = lazyload_FreeStructFromINCHI;

int lazyload_GetINCHI(inchi_Input *inp, inchi_Output *out)
{
	load_inchi_library();
	return lib_GetINCHI(inp, out);
}
GetINCHI_funcptr lib_GetINCHI = lazyload_GetINCHI;

void lazyload_FreeINCHI(inchi_Output *out)
{
	load_inchi_library();
	return lib_FreeINCHI(out);
}
FreeINCHI_funcptr lib_FreeINCHI = lazyload_FreeINCHI;

int lazyload_Get_inchi_Input_FromAuxInfo( char *szInchiAuxInfo, int bDoNotAddH, int bDiffUnkUndfStereo, InchiInpData *pInchiInp )
{
	load_inchi_library();
	return lib_Get_inchi_Input_FromAuxInfo( szInchiAuxInfo, bDoNotAddH, bDiffUnkUndfStereo, pInchiInp );
}
Get_inchi_Input_FromAuxInfo_funcptr lib_Get_inchi_Input_FromAuxInfo = lazyload_Get_inchi_Input_FromAuxInfo;

void lazyload_Free_inchi_Input( inchi_Input *pInp )
{
	load_inchi_library();
	return lib_Free_inchi_Input( pInp );
}
Free_inchi_Input_funcptr lib_Free_inchi_Input = lazyload_Free_inchi_Input;

#else
// Point to functions directly when we are not dynamically loading InChI library.
INCHIGEN_Create_funcptr lib_INCHIGEN_Create = INCHIGEN_Create;
INCHIGEN_Setup_funcptr lib_INCHIGEN_Setup = INCHIGEN_Setup;
INCHIGEN_DoNormalization_funcptr lib_INCHIGEN_DoNormalization = INCHIGEN_DoNormalization;
INCHIGEN_DoCanonicalization_funcptr lib_INCHIGEN_DoCanonicalization = INCHIGEN_DoCanonicalization;
INCHIGEN_DoSerialization_funcptr lib_INCHIGEN_DoSerialization = INCHIGEN_DoSerialization;
INCHIGEN_Reset_funcptr lib_INCHIGEN_Reset = INCHIGEN_Reset;
INCHIGEN_Destroy_funcptr lib_INCHIGEN_Destroy = INCHIGEN_Destroy;
GetINCHIfromINCHI_funcptr lib_GetINCHIfromINCHI = GetINCHIfromINCHI;
GetINCHIKeyFromINCHI_funcptr lib_GetINCHIKeyFromINCHI = GetINCHIKeyFromINCHI;
GetStructFromINCHI_funcptr lib_GetStructFromINCHI = GetStructFromINCHI;
FreeStructFromINCHI_funcptr lib_FreeStructFromINCHI = FreeStructFromINCHI;
GetINCHI_funcptr lib_GetINCHI = GetINCHI;
FreeINCHI_funcptr lib_FreeINCHI = FreeINCHI;
Get_inchi_Input_FromAuxInfo_funcptr lib_Get_inchi_Input_FromAuxInfo = Get_inchi_Input_FromAuxInfo;
Free_inchi_Input_funcptr lib_Free_inchi_Input = Free_inchi_Input;
#endif

/**
  inchi_Input helper functions.
**/

namespace {

	inchi_Atom& get_inchi_atom(inchi_Input& inchi_input, int atom_idx)
	{
		check_range<std::runtime_error>(atom_idx, 0, inchi_input.num_atoms - 1, "inchi_atom_reset: atom_idx");
		return inchi_input.atom[atom_idx];
	}
}

void inchi_set_atom(inchi_Input& inchi_input, int atom_idx, const std::string& atom_symbol, double x, double y, double z, int charge, int radical, int isotopic_mass)
{
	inchi_Atom& iatom = get_inchi_atom(inchi_input, atom_idx);

	iatom.x = x;
	iatom.y = y;
	iatom.z = z;

	iatom.charge = charge;
	iatom.radical = radical;
	iatom.isotopic_mass = isotopic_mass;

	// Let InChI add implicit hydrogens as it pleases.
	iatom.num_iso_H[0] = -1;
	iatom.num_iso_H[1] = 0;
	iatom.num_iso_H[2] = 0;
	iatom.num_iso_H[3] = 0;

	iatom.num_bonds = 0;

	// Copy string-length + 1 bytes to include the 0-byte at the end of the string.
	safe_strcpy(iatom.elname, ATOM_EL_LEN, atom_symbol.c_str(), atom_symbol.length() + 1);
}

void inchi_add_bond(inchi_Input& inchi_input, int from_atom_idx, int to_atom_idx, int bond_cardinality, BondStereo bond_stereo)
{
	inchi_Atom& from_iatom = get_inchi_atom(inchi_input, from_atom_idx);
	inchi_Atom& to_iatom   = get_inchi_atom(inchi_input, to_atom_idx);

	inchi_BondStereo2D inchi_bondstereo = INCHI_BOND_STEREO_NONE;
	if (bond_stereo != RINCHI_BOND_STEREO_NONE) {
		switch (bond_stereo) {
			case RINCHI_BOND_STEREO_UP:
				inchi_bondstereo = INCHI_BOND_STEREO_SINGLE_1UP;
				break;
			case RINCHI_BOND_STEREO_DOWN:
				inchi_bondstereo = INCHI_BOND_STEREO_SINGLE_1DOWN;
				break;
			case RINCHI_BOND_STEREO_EITHER:
				inchi_bondstereo = INCHI_BOND_STEREO_SINGLE_1EITHER;
				break;
			case RINCHI_BOND_STEREO_DOUBLE_EITHER:
				inchi_bondstereo = INCHI_BOND_STEREO_DOUBLE_EITHER;
				break;
			default:
				throw std::runtime_error("Internal bug: Unsupported rinchi::BondStereo value. Please contact the RInChI developers.");
		}
	}

	int bond_idx = from_iatom.num_bonds;
	from_iatom.num_bonds++;
	from_iatom.neighbor[bond_idx]    = to_atom_idx;
	from_iatom.bond_type[bond_idx]   = bond_cardinality;
	from_iatom.bond_stereo[bond_idx] = inchi_bondstereo;

	bond_idx = to_iatom.num_bonds;
	to_iatom.num_bonds++;
	to_iatom.neighbor[bond_idx]    = from_atom_idx;
	to_iatom.bond_type[bond_idx]   = bond_cardinality;
	// We take advantage of the fact that the opposite direction of a (single)
	// InChI stereo bond has a negative int-value. In case this should change
	// in future: In good automated regression tests we trust.
	if (inchi_bondstereo != INCHI_BOND_STEREO_DOUBLE_EITHER)
		to_iatom.bond_stereo[bond_idx] = - inchi_bondstereo;
	else
		to_iatom.bond_stereo[bond_idx] = inchi_bondstereo;
}

} // End of namespace.
