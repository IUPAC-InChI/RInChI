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
#include "rxnfile_tests.h"

#include "rinchi_reaction.h"
#include "rinchi_reader.h"
#include "mdl_rxnfile_reader.h"
#include "mdl_rxnfile_writer.h"

namespace rinchi_tests {

void RxnfileTests::standard_files()
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

		std::string test_filename_rinchis = test_filename + ".rinchi_strings.txt";

		std::string expected_rinchi_string;
		std::string expected_rinchi_auxinfo;
		std::string expected_rinchi_longkey;
		std::string expected_rinchi_shortkey_naive;
		std::string expected_rinchi_shortkey_layered;

		std::ifstream expected_file ( test_filename_rinchis.c_str() );
		if (!expected_file)
			throw rinchi::unit_test::TestFailure (test_filename_rinchis + " does not exist.");
		rinchi::rinchi_getline(expected_file, expected_rinchi_string);
		rinchi::rinchi_getline(expected_file, expected_rinchi_auxinfo);
		rinchi::rinchi_getline(expected_file, expected_rinchi_longkey);
		rinchi::rinchi_getline(expected_file, expected_rinchi_shortkey_naive);
		rinchi::rinchi_getline(expected_file, expected_rinchi_shortkey_layered);

		rinchi::Reaction rxn;
		rinchi::MdlRxnfileReader rxn_reader;
		rxn_reader.read_reaction(test_filename, rxn);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), expected_rinchi_string, test_filename + ": RInChI string");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(), expected_rinchi_auxinfo, test_filename + ": RAuxInfo");
		rinchi::unit_test::check_is_equal(rxn.rinchi_long_key(), expected_rinchi_longkey, test_filename + ": RInChI longkey");
		rinchi::unit_test::check_is_equal(rxn.rinchi_short_key(), expected_rinchi_shortkey_naive, test_filename + ": RInChI shortkey");
	}
	std::cout << " [" << test_count << " files tested]";
}

namespace {

	void test_equi_override(const std::string& test_filename)
	{
		std::string test_filename_rinchis = test_filename + ".rinchi_strings.txt";

		std::string expected_rinchi_string;
		std::string expected_rinchi_auxinfo;
		std::string expected_rinchi_longkey;
		std::string expected_rinchi_shortkey_naive;
		std::string expected_rinchi_shortkey_layered;

		std::ifstream expected_file ( test_filename_rinchis.c_str() );
		if (!expected_file)
			throw rinchi::unit_test::TestFailure (test_filename_rinchis + " does not exist.");
		rinchi::rinchi_getline(expected_file, expected_rinchi_string);
		rinchi::rinchi_getline(expected_file, expected_rinchi_auxinfo);
		rinchi::rinchi_getline(expected_file, expected_rinchi_longkey);
		rinchi::rinchi_getline(expected_file, expected_rinchi_shortkey_naive);
		rinchi::rinchi_getline(expected_file, expected_rinchi_shortkey_layered);

		boost::replace_all(expected_rinchi_string, rinchi::DIRECTION_TAG + rinchi::DIRECTION_FORWARD, rinchi::DIRECTION_TAG + rinchi::DIRECTION_EQUILIBRIUM);
		boost::replace_all(expected_rinchi_string, rinchi::DIRECTION_TAG + rinchi::DIRECTION_REVERSE, rinchi::DIRECTION_TAG + rinchi::DIRECTION_EQUILIBRIUM);
		boost::replace_all(expected_rinchi_longkey, "=SA-BUHFF-", "=SA-EUHFF-");
		boost::replace_all(expected_rinchi_longkey, "=SA-FUHFF-", "=SA-EUHFF-");
		boost::replace_all(expected_rinchi_shortkey_naive, "=SA-BUHFF-", "=SA-EUHFF-");
		boost::replace_all(expected_rinchi_shortkey_naive, "=SA-FUHFF-", "=SA-EUHFF-");
		boost::replace_all(expected_rinchi_shortkey_layered, "=SA-BUHFF-", "=SA-EUHFF-");
		boost::replace_all(expected_rinchi_shortkey_layered, "=SA-FUHFF-", "=SA-EUHFF-");

		rinchi::Reaction rxn;
		rinchi::MdlRxnfileReader rxn_reader;
		//                                           +---- Force equilibrium.
		rxn_reader.read_reaction(test_filename, rxn, true);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), expected_rinchi_string, test_filename + ": RInChI string");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(), expected_rinchi_auxinfo, test_filename + ": RAuxInfo");
		rinchi::unit_test::check_is_equal(rxn.rinchi_long_key(), expected_rinchi_longkey, test_filename + ": RInChI longkey");
		rinchi::unit_test::check_is_equal(rxn.rinchi_short_key(), expected_rinchi_shortkey_naive, test_filename + ": RInChI shortkey");
	}

}

void RxnfileTests::equilibrium_override()
{
	test_equi_override("R005a.rxn");
}

void RxnfileTests::chiral_flag_preservation()
{
	rinchi::MdlRxnfileReader rxn_reader;
	rinchi::MdlRxnfileWriter rxn_writer;
	rinchi::RInChIReader rinchi_reader;
	{
		// Check the non-chiral version first.
		rinchi::Reaction rxn;
		rxn_reader.read_reaction("R005a.rxn", rxn);

		std::string rinchi_string = rxn.rinchi_string();
		std::string rauxinfo = rxn.rinchi_auxinfo();

		rinchi::Reaction rxn_rebuilt;
		rinchi_reader.split_into_reaction(rxn.rinchi_string(), rxn.rinchi_auxinfo(), rxn_rebuilt);

		std::stringstream rxn_stream;
		rxn_writer.write_reaction(rxn_rebuilt, rxn_stream);
		rxn_stream.seekg(0, std::iostream::beg);

		// Check header lines for all components in rebuilt RXN file.
		std::string line;
		for (int i = 0; i < 10; ++i) std::getline(rxn_stream, line);
		rinchi::unit_test::check_is_equal(line, "  6  5  0  0  0  0  0  0  0  0  1 V2000", "Component 1 should not be chiral");
		for (int i = 0; i < 17; ++i) std::getline(rxn_stream, line);
		rinchi::unit_test::check_is_equal(line, "  2  1  0  0  0  0  0  0  0  0  1 V2000", "Component 2 should not be chiral");
		for (int i = 0; i < 9; ++i) std::getline(rxn_stream, line);
		rinchi::unit_test::check_is_equal(line, "  5  5  0  0  0  0  0  0  0  0  1 V2000", "Component 3 should not be chiral");
	}
	{
		// Chiral version next.
		rinchi::Reaction rxn;
		rxn_reader.read_reaction("../rxnfiles_chiral/R005a_chiral.rxn", rxn);

		std::string rinchi_string = rxn.rinchi_string();
		std::string rauxinfo = rxn.rinchi_auxinfo();

		{
			// Rebuild reaction WITH RAuxInfo.
			rinchi::Reaction rxn_rebuilt;
			rinchi_reader.split_into_reaction(rxn.rinchi_string(), rxn.rinchi_auxinfo(), rxn_rebuilt);

			std::stringstream rxn_stream;
			rxn_writer.write_reaction(rxn_rebuilt, rxn_stream);
			rxn_stream.seekg(0, std::iostream::beg);

			// Check header lines for all components in rebuilt RXN file.
			std::string line;
			for (int i = 0; i < 10; ++i) std::getline(rxn_stream, line);
			//                                                     +-- Chiral flag.
			rinchi::unit_test::check_is_equal(line, "  6  5  0  0  1  0  0  0  0  0  1 V2000", "Component 1 should be chiral");
			// ... and also check a single line of coordinates.
			std::getline(rxn_stream, line);
			rinchi::unit_test::check_is_equal(line.substr(0, 32), "    1.6292   -6.9765    0.0000 C", "Component 1 should have coordinates");
			for (int i = 0; i < 16; ++i) std::getline(rxn_stream, line);
			rinchi::unit_test::check_is_equal(line, "  2  1  0  0  0  0  0  0  0  0  1 V2000", "Component 2 should NOT be chiral");
			for (int i = 0; i < 9; ++i) std::getline(rxn_stream, line);
			rinchi::unit_test::check_is_equal(line, "  5  5  0  0  1  0  0  0  0  0  1 V2000", "Component 3 should be chiral");
		}

		{
			// Rebuild reaction withOUT RAuxInfo. All components should lose their chirality and coordinates should end up as 0D.
			rinchi::Reaction rxn_rebuilt;
			rinchi_reader.split_into_reaction(rxn.rinchi_string(), "", rxn_rebuilt);

			std::stringstream rxn_stream;
			rxn_writer.write_reaction(rxn_rebuilt, rxn_stream);
			rxn_stream.seekg(0, std::iostream::beg);

			// Check header lines for all components in rebuilt RXN file.
			std::string line;
			for (int i = 0; i < 10; ++i) std::getline(rxn_stream, line);
			// Two hydrogens added by RInChI - therefore different atom+bond counts.
			rinchi::unit_test::check_is_equal(line, "  8  7  0  0  0  0  0  0  0  0  1 V2000", "Component 1 should not be chiral");
			// ... and also check a single line of coordinates.
			std::getline(rxn_stream, line);
			rinchi::unit_test::check_is_equal(line.substr(0, 32), "    0.0000    0.0000    0.0000 C", "Component 1 should not have coordinates");
			for (int i = 0; i < 20; ++i) std::getline(rxn_stream, line);
			// Metal disconnected by RInChI - therefore different bond count.
			rinchi::unit_test::check_is_equal(line, "  2  0  0  0  0  0  0  0  0  0  2 V2000", "Component 2 should not be chiral");
			// The above mol had the metal disconnected so there is one less bond line. But InChI added a charge to
			// the Na and that adds back a line to the molfile. So we still have to read 9 lines forward.
			for (int i = 0; i < 9; ++i) std::getline(rxn_stream, line);
			// Two hydrogens added by RInChI - therefore different atom+bond counts.
			rinchi::unit_test::check_is_equal(line, "  7  7  0  0  0  0  0  0  0  0  1 V2000", "Component 3 should not be chiral");
		}
	}
}

} // end of namespace
