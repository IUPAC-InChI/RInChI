#ifndef IUPAC_RINCHI_RINCHIREADERTESTS_HEADER_GUARD
#define IUPAC_RINCHI_RINCHIREADERTESTS_HEADER_GUARD

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

class RInChIReaderTests: public rinchi::unit_test::TestCase {
	public:
		void error_cases();
		void trivial_cases();

		void reactants_only();
		void products_only();
		void agents_only();
		void no_structures();

		// Test direct load of component InChI strings via text input.
		void load_inchis_from_text();

		RInChIReaderTests()
		{
			REGISTER_TEST(RInChIReaderTests, error_cases);
			REGISTER_TEST(RInChIReaderTests, trivial_cases);

			REGISTER_TEST(RInChIReaderTests, reactants_only);
			REGISTER_TEST(RInChIReaderTests, products_only);
			REGISTER_TEST(RInChIReaderTests, agents_only);
			REGISTER_TEST(RInChIReaderTests, no_structures);

			REGISTER_TEST(RInChIReaderTests, load_inchis_from_text);
		}

};

} // end of namespace

#endif
