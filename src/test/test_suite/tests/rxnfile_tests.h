#ifndef IUPAC_RINCHI_RXNFILETESTS_HEADER_GUARD
#define IUPAC_RINCHI_RXNFILETESTS_HEADER_GUARD

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

class RxnfileTests: public rinchi::unit_test::TestCase {
	public:
		void standard_files();
		void equilibrium_override();

		void chiral_flag_preservation();
		// MdlRxnfileWriter should optionally write out agents.
		void agent_support_in_writer();

		RxnfileTests()
		{
			REGISTER_TEST(RxnfileTests, standard_files);
			REGISTER_TEST(RxnfileTests, equilibrium_override);

			REGISTER_TEST(RxnfileTests, chiral_flag_preservation);
			REGISTER_TEST(RxnfileTests, agent_support_in_writer);

			goto_subdir("rxnfiles");
		}

};

} // end of namespace

#endif
