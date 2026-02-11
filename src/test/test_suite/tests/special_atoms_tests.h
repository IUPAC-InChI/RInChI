#ifndef IUPAC_RINCHI_SPECIALATOMSTESTS_HEADER_GUARD
#define IUPAC_RINCHI_SPECIALATOMSTESTS_HEADER_GUARD

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

class SpecialAtomsTests: public rinchi::unit_test::TestCase {
	public:
		void err_files();
		void ok_files();
		void single_atom_mol();

		SpecialAtomsTests()
		{
			REGISTER_TEST(SpecialAtomsTests, err_files);
			REGISTER_TEST(SpecialAtomsTests, ok_files);
			REGISTER_TEST(SpecialAtomsTests, single_atom_mol);

			goto_subdir("special_atoms");
		}

};

} // end of namespace

#endif
