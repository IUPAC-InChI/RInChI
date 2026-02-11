#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
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
