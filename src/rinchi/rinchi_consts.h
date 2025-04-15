#ifndef IUPAC_RINCHI_CONSTS_HEADER_GUARD
#define IUPAC_RINCHI_CONSTS_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
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
