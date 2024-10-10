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
#include "rdfile_tests.h"

#include "rinchi_reaction.h"
#include "mdl_rdfile_reader.h"

namespace rinchi_tests {

void RDfileTests::standard_files()
{
	boost::filesystem::path current_dir(".");
	boost::filesystem::directory_iterator end_itr;

	int test_count = 0;
	for (boost::filesystem::directory_iterator testfile(current_dir); testfile != end_itr; testfile++) {
		if (!boost::filesystem::is_regular_file(testfile->path()))
			continue;
		std::string test_filename = testfile->path().string();
		if (test_filename.substr(test_filename.length() - 4, 4) != ".rdf")
			continue;

		test_count++;

		std::string expected_rinchi_string;
		std::string test_filename_rinchis = test_filename + ".rinchi_strings.txt";
		std::ifstream expected_file ( test_filename_rinchis.c_str() );
		if (!expected_file)
			throw rinchi::unit_test::TestFailure (test_filename_rinchis + " does not exist.");
		rinchi::rinchi_getline(expected_file, expected_rinchi_string);

		rinchi::Reaction rxn;
		rinchi::MdlRDfileReader rd_reader;
		rd_reader.read_reaction(test_filename, rxn);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), expected_rinchi_string, test_filename + ": RInChI string");
		// TODO: Check for warnings; e.g. "Example 03 metab UDM.rdf" lists a reactant metabolite that will be perceived as a non-agent,
		//       but it does not match any reactant or product and therefore ends up as an agent.
	}
	// TODO: Add test where RD reader uses alternate keywords. Check that warnings are produced when non-agent does not match reactant or product.
	std::cout << " [" << test_count << " files tested]";
}

namespace {

	void test_equi_read(const std::string& test_filename, const std::string& expected_long_key, const std::string& expected_short_key)
	{
		std::string expected_rinchi_string;

		std::string test_filename_rinchis = test_filename + ".rinchi_strings.txt";
		std::ifstream expected_file ( test_filename_rinchis.c_str() );
		if (!expected_file)
			throw rinchi::unit_test::TestFailure (test_filename_rinchis + " does not exist.");
		rinchi::rinchi_getline(expected_file, expected_rinchi_string);

		boost::replace_all(expected_rinchi_string, rinchi::DIRECTION_TAG + rinchi::DIRECTION_FORWARD, rinchi::DIRECTION_TAG + rinchi::DIRECTION_EQUILIBRIUM);
		boost::replace_all(expected_rinchi_string, rinchi::DIRECTION_TAG + rinchi::DIRECTION_REVERSE, rinchi::DIRECTION_TAG + rinchi::DIRECTION_EQUILIBRIUM);

		rinchi::Reaction rxn;
		rinchi::MdlRDfileReader rd_reader;
		//                                          +----- Force equilibrium.
		rd_reader.read_reaction(test_filename, rxn, true);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), expected_rinchi_string, test_filename + ": RInChI string");

		rinchi::unit_test::check_is_equal(rxn.rinchi_long_key(), expected_long_key, test_filename + ": RInChI long key");
		rinchi::unit_test::check_is_equal(rxn.rinchi_short_key(), expected_short_key, test_filename + ": RInChI short key");
	}
}

void RDfileTests::equilibrium_override()
{
	// d-
	test_equi_read("Example 01 CCR.rdf",
		"Long-RInChIKey=SA-EUHFF-OCEYRUMTOAEWEA-UYRXBGFRSA-N--HRQNWWCYHJTAPI-UHFFFAOYSA-N--WEVYAHXRMPXWCK-UHFFFAOYSA-N-WGHUNMFFLAMBJD-UHFFFAOYSA-M",
		"Short-RInChIKey=SA-EUHFF-OCEYRUMTOA-HRQNWWCYHJ-ANFMRDWEKN-NDGAC-NUHFF-MUHFF-ZZZ");
	// d+
	test_equi_read("Example 03 metab UDM.rdf",
		"Long-RInChIKey=SA-EUHFF-OPNPQXLQERQBBV-UHFFFAOYSA-N--JIOUJVPLJOQUFD-UHFFFAOYSA-N--QVLTXCYWHPZMCA-UHFFFAOYSA-K-SAZUGELZHZOXHB-UHFFFAOYSA-N-XLYOFNOQVPJJNP-UHFFFAOYSA-N-MPMYQQHEHYDOCL-UHFFFAOYSA-N",
		"Short-RInChIKey=SA-EUHFF-OPNPQXLQER-JIOUJVPLJO-ZZLLQJHRMH-NUHFF-NUHFF-KUHFF-ZZZ");
}

} // end of namespace
