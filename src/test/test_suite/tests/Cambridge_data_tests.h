#ifndef IUPAC_RINCHI_CAMBRIDGEDATA_RXNFILETESTS_HEADER_GUARD
#define IUPAC_RINCHI_CAMBRIDGEDATA_RXNFILETESTS_HEADER_GUARD

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

class CambridgeRxnfileTests: public rinchi::unit_test::TestCase {
	public:
		void rinchi_0_02_regression();

		CambridgeRxnfileTests()
		{
			REGISTER_TEST(CambridgeRxnfileTests, rinchi_0_02_regression);

			goto_subdir("Cambridge_rxnfiles");
		}

};

} // end of namespace

#endif
