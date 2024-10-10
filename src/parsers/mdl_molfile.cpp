#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include "mdl_molfile.h"

namespace rinchi {

	const std::string MDL_TAG_V2000("V2000");
	const std::string MDL_TAG_V3000("V3000");

	const std::string MDL_TAG_MOLFILE_END("M  END");

	// V2000 constants for reading InChI-relevant data.

	const std::string MDL_TAG_CHARGE("M  CHG");
	const std::string MDL_TAG_RADICAL("M  RAD");
	const std::string MDL_TAG_ISOTOPE("M  ISO");

	const std::string MDL_TAG_ATOM_ALIAS ("A  ");
	const std::string MDL_TAG_ATOM_VALUE ("V  ");
	const std::string MDL_TAG_GROUP_ABBREVIATION ("G  ");

	const int MDL_MAX_V2000_SGROUP_TEXT_LINE_LENGTH = 69;
	const int MDL_MAX_MOLECULE_NAME_LENGTH = 80;

	// V3000 constants.

	const int MDL_MAX_V3000_LINE_LENGTH = 80;
	const int MDL_TAG_V30_LINE_LENGTH = 7;
	const std::string MDL_TAG_V30_LINE("M  V30 ");
	const std::string MDL_TAG_V30_BLOCK_CTAB_BEGIN("BEGIN CTAB");
	const std::string MDL_TAG_V30_COUNTS("COUNTS");
	const std::string MDL_TAG_V30_BLOCK_CTAB_END("END CTAB");
	const std::string MDL_TAG_V30_STEREO_CFG_KEY("CFG=");

	const std::string MDL_TAG_V30_BLOCK_ATOM_BEGIN("BEGIN ATOM");
	const std::string MDL_TAG_V30_BLOCK_ATOM_END("END ATOM");
	const std::string MDL_TAG_V30_ATOM_KEY_CHARGE("CHG=");
	const std::string MDL_TAG_V30_ATOM_KEY_VALENCE("VAL=");
	const std::string MDL_TAG_V30_ATOM_KEY_ISOTOPE("MASS=");
	const std::string MDL_TAG_V30_ATOM_KEY_RADICAL("RAD=");

	const std::string MDL_TAG_V30_BLOCK_BOND_BEGIN("BEGIN BOND");
	const std::string MDL_TAG_V30_BLOCK_BOND_END("END BOND");

	const std::string MDL_TAG_V30_BLOCK_COLLECTION_BEGIN("BEGIN COLLECTION");
	const std::string MDL_TAG_V30_HILITE_COLLECTION_NAME("MDLV30/HILITE");
	const std::string MDL_TAG_V30_COLLECTION_ATOMS("ATOMS=");
	const std::string MDL_TAG_V30_BLOCK_COLLECTION_END("END COLLECTION");

} // end of namespace
