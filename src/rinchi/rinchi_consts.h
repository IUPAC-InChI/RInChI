#ifndef IUPAC_RINCHI_CONSTS_HEADER_GUARD
#define IUPAC_RINCHI_CONSTS_HEADER_GUARD

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

#include <string>

namespace rinchi {

/** InChI and RInChI String constants. **/
extern const std::string RINCHI_VERSION;
extern const std::string RINCHI_IDENTIFIER;

extern const std::string INCHI_STD_HEADER;
extern const std::string RINCHI_STD_HEADER;

extern const std::string INCHI_AUXINFO_HEADER;
extern const std::string RINCHI_AUXINFO_HEADER;

// RInChI String delimiters.
extern const std::string DELIM_LAYER;
extern const std::string DELIM_COMP;
extern const std::string DELIM_GROUP;

extern const std::string DIRECTION_TAG;
	extern const char DIRECTION_FORWARD;
	extern const char DIRECTION_REVERSE;
	extern const char DIRECTION_EQUILIBRIUM;

extern const std::string NOSTRUCT_INCHI;
extern const std::string NOSTRUCT_AUXINFO;
// No-structure InChI key as used in RInChI long key.
extern const std::string NOSTRUCT_RINCHI_LONGKEY;

extern const std::string NOSTRUCT_TAG;
	extern const char NOSTRUCT_DELIM;

/** RInChI Key constants. **/
extern const std::string RINCHI_LONG_KEY_HEADER;
extern const std::string RINCHI_SHORT_KEY_HEADER;
extern const std::string RINCHI_WEB_KEY_HEADER;

extern const std::string RINCHI_KEY_VERSION_ID_HEADER;

// RInChI Key delimiters.
extern const std::string KEY_DELIM_BLOCK;
extern const std::string KEY_DELIM_COMP;
extern const std::string KEY_DELIM_GROUP;

} // End of namespace.

#endif
