#ifndef IUPAC_RINCHI_MOLFILEREADERTESTS_HEADER_GUARD
#define IUPAC_RINCHI_MOLFILEREADERTESTS_HEADER_GUARD

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

/**
  The inchi_1_06_regression test case checks that this code is able to
  read MDL molfiles and convert them to InChI input data structures
  in a way that produces the same InChIs as the standard InChI 1.06
  executable. This is fundamental for RInChI to work correctly.
**/

class MolfileReaderTests: public rinchi::unit_test::TestCase {
	public:
		// Regression test against InChI 1.06's standard test data.
		// The standard test data set contains V2000 molfiles only.
		void inchi_1_06_regression();

		// Very-old-style molfiles without V2000/V3000 version headers.
		void old_style_molfiles();

		// Check that a subset of InChI 1.06's standard test data
		// converted to V3000 format can be read by the V3000 reader.
		void V3000_reader();

		MolfileReaderTests()
		{
			REGISTER_TEST(MolfileReaderTests, inchi_1_06_regression);
			REGISTER_TEST(MolfileReaderTests, old_style_molfiles);
			REGISTER_TEST(MolfileReaderTests, V3000_reader);
		}

};

} // end of namespace

#endif
