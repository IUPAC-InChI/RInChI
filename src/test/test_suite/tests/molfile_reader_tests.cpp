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
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "rinchi_utils.h"
#include "molfile_reader_tests.h"

#include "rinchi_reaction.h"
#include "mdl_molfile_reader.h"

namespace rinchi_tests {

void MolfileReaderTests::inchi_1_06_regression()
{
	std::ifstream sdfile ("INCHI-1_testset/InChI_TestSet.sdf");
	std::ifstream expected_data ("INCHI-1_testset/its-std-01.inc");

	std::string expected_inchi_string;
	std::string expected_inchi_key;

	if (!sdfile)
		throw rinchi::unit_test::TestFailure("Can't open InChI standard input test set 'InChI_TestSet.sdf'.");
	if (!expected_data)
		throw rinchi::unit_test::TestFailure("Can't open InChI standard test results 'its-std-01.inc'.");

	int current_line_no = 1;
	int mol_no = 1;
	while (sdfile) {
		std::stringstream molfile;
		std::string s;
		while (sdfile && s != "$$$$") {
			rinchi::rinchi_getline(sdfile, s);
			current_line_no++;
			// Our molfile reader ignores trailing data after "M  END" so no need
			// to filter fields and end-of-record marker away.
			molfile << s << "\n";
		}

		// Allow for one or two blank trailing lines in SD file. Otherwise, if we
		// have 4 chars or less in the molfile we must be at the end of the SD file.
		if (molfile.str().length() >= 5) {
			rinchi::rinchi_getline(expected_data, expected_inchi_string);
			rinchi::rinchi_getline(expected_data, expected_inchi_key);

			rinchi::MdlMolfileReader molreader;
			rinchi::ReactionComponent rxncomp;
			molreader.read_molecule(molfile, rxncomp);
            // TODO: Figure out why our implementation, calling into the InChI lib,
            //       can process the 5H isotope defined in the B2H6 H-bridge example,
            //       (test record number 995) while the 1.06 command line inchi-1.exe
            //       and winchi-1.exe both fail with the message "Unacceptable isotope
            //       of hydrogen".
            if (mol_no == 995) {
                rinchi::unit_test::check_is_equal(
                    rxncomp.inchi_string(), "InChI=1S/B2H6/c1-3-2-4-1/h1-2H2/i3+4", "B2H6 special case.");
                rinchi::unit_test::check_is_equal(
                    "InChIKey=" + rxncomp.inchi_key(), "InChIKey=KLDBIFITUCWVCC-UDGUOMFESA-N", "B2H6 special case.");
            }
            else {
                rinchi::unit_test::check_is_equal(
                    rxncomp.inchi_string(), expected_inchi_string,
                    "InChI string: Mol #" + rinchi::int2str(mol_no) + "; record ending at line " + rinchi::int2str(current_line_no) + ".");
                rinchi::unit_test::check_is_equal(
                    "InChIKey=" + rxncomp.inchi_key(), expected_inchi_key,
                    "InChI key: Mol #" + rinchi::int2str(mol_no) + "; record ending at line " + rinchi::int2str(current_line_no) + ".");
            }
            mol_no++;
		}
	}
	std::cout << " [" << (mol_no - 1) << " mols tested]";
}

void MolfileReaderTests::old_style_molfiles()
{
	std::ifstream molfile_stream ("./Non_versioned_molfiles/example1.mol");
	rinchi::MdlMolfileReader mr;
	rinchi::Reaction rxn;
	rinchi::ReactionComponent* rc = rxn.add_reactant();
	mr.read_molecule(molfile_stream, *rc);

	rinchi::unit_test::check_is_equal(rc->inchi_string(), "InChI=1S/C8H8O2/c1-10-8-4-2-7(6-9)3-5-8/h2-6H,1H3", "InChI - example1");
	rinchi::unit_test::check_is_equal(rc->inchi_auxinfo(), "AuxInfo=1/0/N:1,4,5,6,7,3,9,10,2,8/E:(2,3)(4,5)/rA:10nCOCCCCCOCC/rB:;d2;;;d4;s5;s1;s3s4d5;s6d7s8;/rC:1.4358,0,0;1.4858,4.9995,0;.9068,4.6076,0;0,3.0351,0;1.8152,3.0351,0;0,1.9828,0;1.8152,1.9828,0;.8714,.4089,0;.9068,3.5566,0;.9068,1.4597,0;", "AuxInfo - example1");
}

void MolfileReaderTests::V3000_reader()
{
	boost::filesystem::path current_dir("./V3000_molfiles/");
	boost::filesystem::directory_iterator end_itr;

	int test_count = 0;
	for (boost::filesystem::directory_iterator testfile(current_dir); testfile != end_itr; testfile++) {
		std::string filename_V3000 = testfile->path().string();
		if (filename_V3000.find("V3000.mol") == std::string::npos)
			continue;

		test_count++;

		std::string filename_V2000 = filename_V3000;
		boost::replace_last(filename_V2000, "V3000.mol", "V2000.mol");

		rinchi::MdlMolfileReader molreader;
		rinchi::ReactionComponent rxncomp_V2000;
		rinchi::ReactionComponent rxncomp_V3000;
		molreader.read_molecule(filename_V2000, rxncomp_V2000);
		molreader.read_molecule(filename_V3000, rxncomp_V3000);

		rinchi::unit_test::check_is_equal(
			rxncomp_V2000.inchi_string(), rxncomp_V3000.inchi_string(),
			"InChI of '" + filename_V3000 + "'.");
	}
	std::cout << " [" << test_count << " files tested]";
}

} // end of namespace
