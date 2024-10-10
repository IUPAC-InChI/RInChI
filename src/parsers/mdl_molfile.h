#ifndef IUPAC_RINCHI_MDL_MOLFILE_HEADER_GUARD
#define IUPAC_RINCHI_MDL_MOLFILE_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <string>

/** Constants defining tags in MDL molfiles. **/

namespace rinchi {

	extern const std::string MDL_TAG_V2000;
	extern const std::string MDL_TAG_V3000;

	extern const std::string MDL_TAG_MOLFILE_END;

	// V2000 constants for reading InChI-relevant data.

	extern const std::string MDL_TAG_CHARGE;
	extern const std::string MDL_TAG_RADICAL;
	extern const std::string MDL_TAG_ISOTOPE;

	extern const std::string MDL_TAG_ATOM_ALIAS;
	extern const std::string MDL_TAG_ATOM_VALUE;
	extern const std::string MDL_TAG_GROUP_ABBREVIATION;

	extern const int MDL_MAX_V2000_SGROUP_TEXT_LINE_LENGTH;
	extern const int MDL_MAX_MOLECULE_NAME_LENGTH;

	// V3000 constants.

	extern const int MDL_MAX_V3000_LINE_LENGTH;
	extern const int MDL_TAG_V30_LINE_LENGTH;
	extern const std::string MDL_TAG_V30_LINE;
	extern const std::string MDL_TAG_V30_BLOCK_CTAB_BEGIN;
	extern const std::string MDL_TAG_V30_COUNTS;
	extern const std::string MDL_TAG_V30_BLOCK_CTAB_END;
	extern const std::string MDL_TAG_V30_STEREO_CFG_KEY;

	extern const std::string MDL_TAG_V30_BLOCK_ATOM_BEGIN;
	extern const std::string MDL_TAG_V30_BLOCK_ATOM_END;
	extern const std::string MDL_TAG_V30_ATOM_KEY_CHARGE;
	extern const std::string MDL_TAG_V30_ATOM_KEY_VALENCE;
	extern const std::string MDL_TAG_V30_ATOM_KEY_ISOTOPE;
	extern const std::string MDL_TAG_V30_ATOM_KEY_RADICAL;

	extern const std::string MDL_TAG_V30_BLOCK_BOND_BEGIN;
	extern const std::string MDL_TAG_V30_BLOCK_BOND_END;

	extern const std::string MDL_TAG_V30_BLOCK_COLLECTION_BEGIN;
	extern const std::string MDL_TAG_V30_HILITE_COLLECTION_NAME;
	extern const std::string MDL_TAG_V30_COLLECTION_ATOMS;
	extern const std::string MDL_TAG_V30_BLOCK_COLLECTION_END;

} // end of namespace

#endif
