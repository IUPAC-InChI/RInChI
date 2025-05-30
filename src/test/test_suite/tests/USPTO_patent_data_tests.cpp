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

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "rinchi_consts.h"
#include "rinchi_utils.h"

#include "rinchi_reaction.h"
#include "mdl_rxnfile_reader.h"

#include "USPTO_patent_data_tests.h"

namespace rinchi_tests {

void USPTOPatentDataTests::rxnfiles_subset()
{
	boost::filesystem::path current_dir(".");
	boost::filesystem::directory_iterator end_itr;

	int test_count = 0;
	for (boost::filesystem::directory_iterator testfile(current_dir); testfile != end_itr; testfile++) {
		if (!boost::filesystem::is_regular_file(testfile->path()))
			continue;
		std::string test_filename = testfile->path().string();
		if (test_filename.substr(test_filename.length() - 4, 4) != ".rxn")
			continue;

		test_count++;

		std::string test_filename_rinchis = test_filename.substr(0, test_filename.length() - 4) + ".txt";
        // This single RXN file produces different RAuxInfo on Windows versus on Linux. This is an InChI
        // issue that we cannot fix here; only work around.
        if (test_filename == "./1976-US03964903-9091.rxn") {
            test_filename_rinchis = test_filename.substr(0, test_filename.length() - 4) + "__LINUX.txt";
        }

		std::string expected_rinchi_string;
		std::string expected_rinchi_auxinfo;
		std::ifstream expected_file ( test_filename_rinchis.c_str() );
		if (!expected_file)
			throw rinchi::unit_test::TestFailure (test_filename_rinchis + " does not exist.");
		rinchi::rinchi_getline(expected_file, expected_rinchi_string);
		rinchi::rinchi_getline(expected_file, expected_rinchi_auxinfo);

        rinchi::Reaction rxn;
		rinchi::MdlRxnfileReader rxn_reader;
		rxn_reader.read_reaction(test_filename, rxn);
		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), expected_rinchi_string, test_filename + ": RInChI string");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(), expected_rinchi_auxinfo, test_filename + ": RAuxInfo");
	}
	std::cout << " [" << test_count << " files tested]";
}

} // end of namespace
