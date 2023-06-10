#ifndef IUPAC_RINCHI_RINCHI_LIB_HEADER_GUARD
#define IUPAC_RINCHI_RINCHI_LIB_HEADER_GUARD

#ifdef MSVC
#pragma region InChI-Trust Licence
/*
 * Reaction International Chemical Identifier (RInChI)
 * Version 1
 * Software version 1.00
 * 2022-01-14
 * 
 * The RInChI library and programs are free software developed under the
 * auspices of the International Union of Pure and Applied Chemistry (IUPAC).
 * 
 * IUPAC/InChI-Trust Licence No.1.0 for the 
 * Reaction International Chemical Identifier (RInChI) Software version 1.0
 * Copyright (C) IUPAC and InChI Trust Limited
 * 
 * This library is free software; you can redistribute it and/or modify it 
 * under the terms of the IUPAC/InChI Trust InChI Licence No.1.0, 
 * or any later version.
 * 
 * Please note that this library is distributed WITHOUT ANY WARRANTIES 
 * whatsoever, whether expressed or implied.  See the IUPAC/InChI Trust
 * Licence for the International Chemical Identifier (InChI) Software
 * ("IUPAC/InChI-Trust InChI Licence No. 1.0" in "LICENCE.TXT")
 * for more details.
 * 
 * You should have received a copy of the IUPAC/InChI Trust InChI 
 * Licence No. 1.0 with this library; if not, please write to:
 * 
 *     The InChI Trust
 *     8 Cavendish Avenue
 *     Cambridge CB1 7US
 *     UK
 *
 * or email to: alan@inchi-trust.org.
 *
 */
#pragma endregion
#endif

#define RETURN_CODE_SUCCESS 0
#define RETURN_CODE_ERROR 1

extern const char* RINCHI_INPUT_FORMAT_RDFILE;
extern const char* RINCHI_INPUT_FORMAT_RXNFILE;
extern const char* RINCHI_INPUT_FORMAT_AUTO;

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif



/**
 * @brief rinchilib_latest_err_msg() returns the latest error message produced.
 * @return Pointer to C-string with latest error message.
 */
const char* rinchilib_latest_err_msg();

/**
 * @brief rinchilib_rinchi_from_file_text() generates RInChI and RAuxInfo from an RD or RXN file text block.
 * @param input_format - "RD" or "RXN" to force explicit parsing; "" or "AUTO" to attempt format auto-detection.
 * @param in_file_text - Text block with RD or RXN file contents.
 * @param in_force_equilibrium - Force interpretation of reaction as equilibrium reaction.
 * @param out_rinchi_string - Returned pointer to C-string with generated RInChI.
 * @param out_rinchi_auxinfo - Returned pointer to C-string with generated RAuxInfo.
 * @return Standard error code.
 */
int rinchilib_rinchi_from_file_text(
	const char* input_format, const char* in_file_text, bool in_force_equilibrium, const char** out_rinchi_string, const char** out_rinchi_auxinfo
);

/**
 * @brief rinchilib_rinchikey_from_file_text() generates an RInChI key from an RD or RXN file text block.
 * @param input_format - "RD" or "RXN" to force explicit parsing; "" or "AUTO" to attempt format auto-detection.
 * @param in_file_text - Text block with RD or RXN file contents.
 * @param in_force_equilibrium - Force interpretation of reaction as equilibrium reaction.
 * @param key_type - 1-letter string controlling the type of key generated: L(ong), S(hort) or W(eb).
 * @param out_rinchi_key - Returned pointer to C-string with generated key.
 * @return Standard error code.
 */
int rinchilib_rinchikey_from_file_text(
	const char* input_format, const char* in_file_text, const char* key_type, bool in_force_equilibrium, const char** out_rinchi_key
);

/**
 * @brief rinchilib_file_text_from_rinchi() reconstructs an RD or RXN file from RInChI and (optionally) RAuxInfo.
 * @param rinchi_string - RInChI string.
 * @param rinchi_auxinfo - RAuxInfo. May be blank but not NULL.
 * @param output_format - "RD" or "RXN".
 * @param out_file_text - Returned pointer to C-string with produced file text block.
 * @return Standard error code.
 */
int rinchilib_file_text_from_rinchi(
	const char* rinchi_string, const char* rinchi_auxinfo, const char* output_format, const char** out_file_text
);

/**
 * @brief rinchilib_inchis_from_rinchi() splits RInChI and (optionally) RAuxInfo into InChI+AuxInfo pairs of individual component moeities.
 * @param rinchi_string - RInChI string.
 * @param rinchi_auxinfo- RAuxInfo. May be blank but not NULL.
 * @param out_inchis_text- Returned pointer to C-string with InChIs and RAuxInfo-s.
 * @return Standard error code.
 */
int rinchilib_inchis_from_rinchi(
	const char* rinchi_string, const char* rinchi_auxinfo, const char** out_inchis_text
);

/**
 * @brief rinchilib_rinchi_from_inchis() loads pre-calculated InChI strings, and optionally AuxInfo strings, from linefeed-delimited text input and outputs RInChI and RAuxInfo strings.
 * @param reactant_inchis - Text lines with InChI lines or InChI+AuxInfo line pairs. NULL input is equivalent to a blank input string.
 * @param product_inchis - Text lines with InChI lines or InChI+AuxInfo line pairs. NULL input is equivalent to a blank input string.
 * @param agent_inchis - Text lines with InChI lines or InChI+AuxInfo line pairs. NULL input is equivalent to a blank input string.
 * @param out_rinchi_string - Returned pointer to C-string with generated RInChI.
 * @param out_rinchi_auxinfo - Returned pointer to C-string with generated RAuxInfo.
 * @return Standard error code.
 */
int rinchilib_rinchi_from_inchis(
        const char* reactant_inchis, const char* product_inchis, const char* agent_inchis, const char** out_rinchi_string, const char** out_rinchi_auxinfo
);

/**
 * @brief rinchilib_rinchikey_from_rinchi() generates an RInChI key from an RInChI string.
 * @param rinchi_string - RInChI string.
 * @param key_type - 1-letter string controlling the type of key generated: L(ong), S(hort) or W(eb).
 * @param out_rinchi_key - Returned pointer to C-string with generated key.
 * @return Standard error code.
 */
int rinchilib_rinchikey_from_rinchi(
	const char* rinchi_string, const char* key_type, const char** out_rinchi_key
);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif
