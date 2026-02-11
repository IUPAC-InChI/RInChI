#ifndef IUPAC_RINCHI_MDL_RDFILE_HEADER_GUARD
#define IUPAC_RINCHI_MDL_RDFILE_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

/** Constants defining tags in MDL RD files. **/

namespace rinchi {

	const std::string MDL_RDFILE_IDENTIFIER("$RDFILE 1");
	const std::string MDL_RDTAG_DATM("$DATM");

	const std::string MDL_RDTAG_RDFILE("$RDFILE");
	const std::string MDL_RDTAG_RFMT("$RFMT");

	const std::string MDL_RDTAG_DTYPE("$DTYPE");
	const std::string MDL_RDLINE_MFMT("$DATUM $MFMT");

} // end of namespace

#endif
