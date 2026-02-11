#ifndef IUPAC_RINCHI_USPTO_PATENTDATA_TESTS_HEADER_GUARD
#define IUPAC_RINCHI_USPTO_PATENTDATA_TESTS_HEADER_GUARD

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

class USPTOPatentDataTests: public rinchi::unit_test::TestCase {
	public:
		void rxnfiles_subset();

		USPTOPatentDataTests()
		{
			REGISTER_TEST(USPTOPatentDataTests, rxnfiles_subset);

			goto_subdir("USPTO_patent_data");
		}

};

} // end of namespace

#endif
