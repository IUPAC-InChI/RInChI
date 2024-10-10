#ifndef IUPAC_RINCHI_RDFILETESTS_HEADER_GUARD
#define IUPAC_RINCHI_RDFILETESTS_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../../../LICENCE.txt.
#pragma endregion
#endif

#include "unit_test.h"

namespace rinchi_tests {

class RDfileTests: public rinchi::unit_test::TestCase {
	public:
		void standard_files();
		void equilibrium_override();

		RDfileTests()
		{
			REGISTER_TEST(RDfileTests, standard_files);
			REGISTER_TEST(RDfileTests, equilibrium_override);

			goto_subdir("RDfiles");
		}

};

} // end of namespace

#endif
