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
