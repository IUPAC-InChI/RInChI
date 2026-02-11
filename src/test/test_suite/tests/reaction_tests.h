#ifndef IUPAC_RINCHI_REACTIONTESTS_HEADER_GUARD
#define IUPAC_RINCHI_REACTIONTESTS_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../../../LICENCE.txt.
#pragma endregion
#endif

#include <unit_test.h>

namespace rinchi_tests {

class ReactionTests: public rinchi::unit_test::TestCase {
	public:
		void sorting();
		void parsing();
		void no_structure_handling_from_rxn();
		void no_structure_handling_from_rinchi();
		void web_key();

		ReactionTests()
		{
			REGISTER_TEST(ReactionTests, sorting);
			REGISTER_TEST(ReactionTests, parsing);
			REGISTER_TEST(ReactionTests, no_structure_handling_from_rxn);
			REGISTER_TEST(ReactionTests, no_structure_handling_from_rinchi);
			REGISTER_TEST(ReactionTests, web_key);

			goto_subdir("ReactionTest");
		}

};

} // end of namespace

#endif
