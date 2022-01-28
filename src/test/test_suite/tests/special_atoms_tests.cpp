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

#include "special_atoms_tests.h"

#include "mdl_molfile_reader.h"
#include "mdl_rxnfile_reader.h"
#include "mdl_rdfile_reader.h"

namespace rinchi_tests {

namespace {

	void test_err_file(const std::string& test_filename)
	{
		rinchi::Reaction rxn;
		try {
			if (test_filename.substr(test_filename.length() - 4, 4) == ".rxn") {
				rinchi::MdlRxnfileReader rxn_reader;
				rxn_reader.read_reaction(test_filename, rxn);
			}
			else {
				rinchi::MdlRDfileReader rd_reader;
				rd_reader.read_reaction(test_filename, rxn);
			}
			// Invalid/Unsupported atom symbols in molfiles will not cause a failure while reading
			// the molfile but a later InChI generation failure. When reading RD files, the error
			// will happen during reading, since the RD reader is using InChI keys to compare
			// read structures. The RXN reader will not throw an error, and so we have to force
			// InChI calculation via rxn.rinchi_string().
			std::string dummy_string = rxn.rinchi_string();

			throw rinchi::unit_test::TestFailure(test_filename + " input should have thrown an error.");
		} catch (std::exception& e) {
			rinchi::unit_test::check_errmsg_has_substr(e.what(), "no InChI has been created");
		}
	}

}

void SpecialAtomsTests::err_files()
{
	test_err_file("err__R_reactant-A_product.rxn");
	test_err_file("err__R_reactant-X_product.rxn");
	test_err_file("err__star_reactant-product.rdf");
}

namespace {

	void test_ok_file(const std::string& test_filename, const std::string& expected_rinchi)
	{
		rinchi::Reaction rxn;
		if (test_filename.substr(test_filename.length() - 4, 4) == ".rxn") {
			rinchi::MdlRxnfileReader rxn_reader;
			rxn_reader.read_reaction(test_filename, rxn);
		}
		else {
			rinchi::MdlRDfileReader rd_reader;
			rd_reader.read_reaction(test_filename, rxn);
		}

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), expected_rinchi, test_filename + ": RInChI string");
	}

}

void SpecialAtomsTests::ok_files()
{
	test_ok_file("ok__nostruct-A.rxn", "RInChI=1.00.1S//d+/u1-1-0");
	test_ok_file("ok__nostruct-X.rdf", "RInChI=1.00.1S/<><>C4H8O/c1-2-4-5-3-1/h1-4H2!Mn.2O/d+/u1-1-0");
	test_ok_file("ok__R-A.rxn", "RInChI=1.00.1S//d+/u1-1-0");
	test_ok_file("ok__R-X.rdf", "RInChI=1.00.1S/<><>C2H6O/c1-2-3/h3H,2H2,1H3!Cu/d+/u1-1-0");
	test_ok_file("ok__R-X.rxn", "RInChI=1.00.1S//d+/u1-1-0");
	test_ok_file("ok__star_star-nostruct.rdf", "RInChI=1.00.1S/<><>Cu.O/d+/u2-1-0");
}

namespace {

	void test_molfile(const std::string& test_filename, const std::string& expected_rinchi)
	{
		rinchi::MdlMolfileReader mr;
		rinchi::Reaction rxn;
		rinchi::ReactionComponent* rc = rxn.add_reactant();
		mr.read_molecule(test_filename, *rc);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), expected_rinchi, "Single-atom: " + test_filename);
	}

}

// Check that normal molfiles with a single atom can still be read OK (the molfile
// reader treats single-atom molfiles specially to handle No-Structure equivalents).
void SpecialAtomsTests::single_atom_mol()
{
	test_molfile("single_atom.mol", "RInChI=1.00.1S/<>CH4/h1H4/d-");
	test_molfile("single_atom_V3000.mol", "RInChI=1.00.1S/<>CH4/h1H4/d-");
	// Single X atom in V3000 file is also equivalent to No-Structure.
	test_molfile("single_X_atom_V3000.mol", "RInChI=1.00.1S//d+/u1-0-0");
}

} // end of namespace
