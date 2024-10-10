#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../../../LICENCE.txt.
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
