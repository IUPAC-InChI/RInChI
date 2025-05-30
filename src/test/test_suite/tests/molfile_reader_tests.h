#ifndef IUPAC_RINCHI_MOLFILEREADERTESTS_HEADER_GUARD
#define IUPAC_RINCHI_MOLFILEREADERTESTS_HEADER_GUARD

#ifdef MSVC
#pragma region InChI-Trust Licence
/*
 * Reaction International Chemical Identifier (RInChI)
 * Version 1
 * Software version 1.00
 * 2022-01-14
 * 
 * The RInChI library and programs are free software developed under the
 * auspices of the International Union of Pure and Applied Chemistry (IUPAC).
 * 
 * IUPAC/InChI-Trust Licence No.1.0 for the 
 * Reaction International Chemical Identifier (RInChI) Software version 1.0
 * Copyright (C) IUPAC and InChI Trust Limited
 * 
 * This library is free software; you can redistribute it and/or modify it 
 * under the terms of the IUPAC/InChI Trust InChI Licence No.1.0, 
 * or any later version.
 * 
 * Please note that this library is distributed WITHOUT ANY WARRANTIES 
 * whatsoever, whether expressed or implied.  See the IUPAC/InChI Trust
 * Licence for the International Chemical Identifier (InChI) Software
 * ("IUPAC/InChI-Trust InChI Licence No. 1.0" in "LICENCE.TXT")
 * for more details.
 * 
 * You should have received a copy of the IUPAC/InChI Trust InChI 
 * Licence No. 1.0 with this library; if not, please write to:
 * 
 *     The InChI Trust
 *     8 Cavendish Avenue
 *     Cambridge CB1 7US
 *     UK
 *
 * or email to: alan@inchi-trust.org.
 *
 */
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
