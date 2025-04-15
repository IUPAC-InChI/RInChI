#ifndef IUPAC_RINCHI_INCHI_API_INTF_HEADER_GUARD
#define IUPAC_RINCHI_INCHI_API_INTF_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <inchi_api.h>
#include <string>

namespace rinchi {

#ifdef RINCHI_LOAD_INCHI_DYNAMICALLY
/**
  If the InChI lib is not in your loader path you must set this explicitly.
  Note that setting LD_LIBRARY_PATH run-time on a UNIX-like platform will have no effect
  since LD_LIBRARY_PATH is only inspected by the loader on process startup.
**/
const int INCHI_LIB_PATH_MAX = 512;

// Path to the InChI shared library file; pure path, no file name at end.
extern std::string inchi_lib_path;
// C style char array set by functions that cannot reliably handle C++ strings (e.g. DLL init function).
// This likely includes a file name which is not the file name of the InChI library. Will only be used
// if 'inchi_lib_path' is blank.
extern char inchi_lib_path_c_str[INCHI_LIB_PATH_MAX];
#endif

/** InChI options **/
extern const char* INCHI_INPUT_OPTION_CHIRAL_FLAG_ON;
extern const char* INCHI_INPUT_OPTION_CHIRAL_FLAG_OFF;

extern const char* INCHI_OUTPUT_OPTION_SDF;
extern const char* INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_ON;
extern const char* INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_OFF;

/** InChI generator interface **/

// INCHIGEN_HANDLE INCHIGEN_Create(void);
typedef INCHIGEN_HANDLE (*INCHIGEN_Create_funcptr)();
extern INCHIGEN_Create_funcptr	lib_INCHIGEN_Create;

// int  INCHIGEN_Setup( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Input * pInp);
typedef int (*INCHIGEN_Setup_funcptr)( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Input * pInp);
extern INCHIGEN_Setup_funcptr	lib_INCHIGEN_Setup;

// int  INCHIGEN_DoNormalization( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData );
typedef int (*INCHIGEN_DoNormalization_funcptr)( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData );
extern INCHIGEN_DoNormalization_funcptr	lib_INCHIGEN_DoNormalization;

// int  INCHIGEN_DoCanonicalization( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData );
typedef int (*INCHIGEN_DoCanonicalization_funcptr)( INCHIGEN_HANDLE HGen, INCHIGEN_DATA *pGenData );
extern INCHIGEN_DoCanonicalization_funcptr	lib_INCHIGEN_DoCanonicalization;

// int  INCHIGEN_DoSerialization(INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults );
typedef int (*INCHIGEN_DoSerialization_funcptr)(INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults );
extern INCHIGEN_DoSerialization_funcptr	lib_INCHIGEN_DoSerialization;

// void INCHIGEN_Reset( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults);
typedef void (*INCHIGEN_Reset_funcptr)( INCHIGEN_HANDLE HGen, INCHIGEN_DATA * pGenData, inchi_Output * pResults);
extern INCHIGEN_Reset_funcptr	lib_INCHIGEN_Reset;

// void INCHIGEN_Destroy( INCHIGEN_HANDLE HGen );
typedef void (*INCHIGEN_Destroy_funcptr)( INCHIGEN_HANDLE HGen );
extern INCHIGEN_Destroy_funcptr	lib_INCHIGEN_Destroy;

/** InChI to InChI, InChI to key interface **/
//int INCHI_DECL GetINCHIfromINCHI(inchi_InputINCHI *inpInChI, inchi_Output *out)
typedef int (*GetINCHIfromINCHI_funcptr)(inchi_InputINCHI *inpInChI, inchi_Output *out);
extern GetINCHIfromINCHI_funcptr lib_GetINCHIfromINCHI;

// int INCHI_DECL GetINCHIKeyFromINCHI(const char* szINCHISource, const int xtra1, const int xtra2, char* szINCHIKey, char* szXtra1, char* szXtra2);
typedef int (*GetINCHIKeyFromINCHI_funcptr)(const char* szINCHISource, const int xtra1, const int xtra2, char* szINCHIKey, char* szXtra1, char* szXtra2);
extern GetINCHIKeyFromINCHI_funcptr lib_GetINCHIKeyFromINCHI;

/** InChI => structure interface **/
// int INCHI_DECL GetStructFromINCHI(inchi_InputINCHI* inpInChI, inchi_OutputStruct* outStruct);
typedef int (*GetStructFromINCHI_funcptr)(inchi_InputINCHI* inpInChI, inchi_OutputStruct* outStruct);
extern GetStructFromINCHI_funcptr lib_GetStructFromINCHI;

// void INCHI_DECL FreeStructFromINCHI( inchi_OutputStruct* out );
typedef void (*FreeStructFromINCHI_funcptr)( inchi_OutputStruct* out );
extern FreeStructFromINCHI_funcptr lib_FreeStructFromINCHI;

// int INCHI_DECL GetINCHI(inchi_Input *inp, inchi_Output *out);
typedef int (*GetINCHI_funcptr)(inchi_Input *inp, inchi_Output *out);
extern GetINCHI_funcptr lib_GetINCHI;

// void INCHI_DECL FreeINCHI(inchi_Output *out);
typedef void (*FreeINCHI_funcptr)(inchi_Output *out);
extern FreeINCHI_funcptr lib_FreeINCHI;

// int INCHI_DECL Get_inchi_Input_FromAuxInfo(
//	char *szInchiAuxInfo, int bDoNotAddH, int bDiffUnkUndfStereo, InchiInpData *pInchiInp )
typedef int (*Get_inchi_Input_FromAuxInfo_funcptr)(char *szInchiAuxInfo, int bDoNotAddH, int bDiffUnkUndfStereo, InchiInpData *pInchiInp );
extern Get_inchi_Input_FromAuxInfo_funcptr lib_Get_inchi_Input_FromAuxInfo;

// void INCHI_DECL Free_inchi_Input( inchi_Input *pInp )
typedef void (*Free_inchi_Input_funcptr)( inchi_Input *pInp );
extern Free_inchi_Input_funcptr lib_Free_inchi_Input;

/**
  inchi_Input helper functions.
**/

// Enum listing supported bond stereo definitions.
typedef enum { RINCHI_BOND_STEREO_NONE, RINCHI_BOND_STEREO_UP, RINCHI_BOND_STEREO_DOWN, RINCHI_BOND_STEREO_EITHER, RINCHI_BOND_STEREO_DOUBLE_EITHER } BondStereo;

void inchi_set_atom(inchi_Input& inchi_input, int atom_idx, const std::string& atom_symbol, double x, double y, double z, int charge, int radical, int isotopic_mass);
void inchi_add_bond(inchi_Input& inchi_input, int from_atom_idx, int to_atom_idx, int bond_cardinality, BondStereo bond_stereo);

} // End of namespace.

#endif
