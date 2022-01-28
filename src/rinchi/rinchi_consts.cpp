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

#include "rinchi_consts.h"

namespace rinchi {

const std::string RINCHI_VERSION = "1.00";
const std::string RINCHI_IDENTIFIER = "RInChI" + RINCHI_VERSION;

const std::string INCHI_STD_HEADER = "InChI=1S/";
const std::string RINCHI_STD_HEADER = "RInChI=" + RINCHI_VERSION + ".1S/";

const std::string INCHI_AUXINFO_HEADER = "AuxInfo=1/";
const std::string RINCHI_AUXINFO_HEADER = "RAuxInfo=" + RINCHI_VERSION + ".1/";

// Delimiters in RInChI strings.
const std::string DELIM_LAYER = "/";
const std::string DELIM_COMP = "!";
const std::string DELIM_GROUP = "<>";

const std::string DIRECTION_TAG = "/d";
	const char DIRECTION_FORWARD = '+';
	const char DIRECTION_REVERSE = '-';
	const char DIRECTION_EQUILIBRIUM = '=';

const std::string NOSTRUCT_INCHI = INCHI_STD_HEADER + DELIM_LAYER;
const std::string NOSTRUCT_AUXINFO = INCHI_AUXINFO_HEADER + DELIM_LAYER;
const std::string NOSTRUCT_RINCHI_LONGKEY = "MOSFIJXAXDLOML-UHFFFAOYSA-N";

const std::string NOSTRUCT_TAG = "/u";
	const char NOSTRUCT_DELIM = '-';

const std::string RINCHI_LONG_KEY_HEADER = "Long-RInChIKey=";
const std::string RINCHI_SHORT_KEY_HEADER = "Short-RInChIKey=";
const std::string RINCHI_WEB_KEY_HEADER = "Web-RInChIKey=";

const std::string RINCHI_KEY_VERSION_ID_HEADER = "SA";

// Delimiters in RInChI keys.
const std::string KEY_DELIM_BLOCK = "-";
const std::string KEY_DELIM_COMP = "-";
const std::string KEY_DELIM_GROUP = "--";


} // End of namespace.
