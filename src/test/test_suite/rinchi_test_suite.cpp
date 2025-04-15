#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../../LICENCE.txt.
#pragma endregion
#endif

/**
  Test suite runner. Make sure that this executable starts in the directory
  <RINCHI_ROOT_DIR>/src/test/ so it can locate test data correctly.
**/

#include <iostream>

#include "molfile_reader_tests.h"
#include "reaction_tests.h"
#include "rxnfile_tests.h"
#include "rdfile_tests.h"
#include "special_atoms_tests.h"
#include "rinchi_reader_tests.h"

#include "Cambridge_data_tests.h"
#include "USPTO_patent_data_tests.h"

int main(int argc, char* argv[])
{
	// argc and argv unused.
	(void) argc;
	(void) argv;

    int exit_code = 0;

	try {
	// Molfile reader tests are very expensive to run when in debug mode on Windows.
	// Therefore SKIP_MOLFILE_READER_TESTS will be defined in the Windows debug build.
	#ifndef SKIP_MOLFILE_READER_TESTS
		rinchi_tests::MolfileReaderTests().run_tests();
	#endif
		rinchi_tests::ReactionTests().run_tests();
		rinchi_tests::RxnfileTests().run_tests();
		rinchi_tests::RDfileTests().run_tests();
		rinchi_tests::SpecialAtomsTests().run_tests();
		rinchi_tests::RInChIReaderTests().run_tests();
		rinchi_tests::CambridgeRxnfileTests().run_tests();
        rinchi_tests::USPTOPatentDataTests().run_tests();

		rinchi::unit_test::GlobalTestStats().show_stats();

        if (!rinchi::unit_test::GlobalTestStats().err_msg_list.empty())
            exit_code = 2;

    } catch (std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		exit_code = 1;
	}

    return exit_code;
}
