#ifndef IUPAC_RINCHI_MDL_RXNFILE_HEADER_GUARD
#define IUPAC_RINCHI_MDL_RXNFILE_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

/** Constants defining tags in MDL rxn files. **/

namespace rinchi {

	const std::string MDL_TAG_RXN_BEGIN("$RXN");
	const std::string MDL_TAG_RXN_COMPONENT_START("$MOL");

} // end of namespace

#endif
