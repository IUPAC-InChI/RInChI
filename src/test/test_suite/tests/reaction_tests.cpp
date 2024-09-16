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

#include "rinchi_consts.h"
#include "rinchi_utils.h"
#include "reaction_tests.h"

#include "rinchi_reaction.h"
#include "mdl_molfile_reader.h"
#include "mdl_rdfile_reader.h"
#include "rinchi_reader.h"

namespace rinchi_tests {

void ReactionTests::sorting()
{
	rinchi::MdlMolfileReader mr;
	{
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_reactant());
		mr.read_molecule("F2.mol", *rxn.add_reactant());
		mr.read_molecule("H3NO.mol", *rxn.add_reactant());

		mr.read_molecule("CH4S.mol", *rxn.add_product());
		mr.read_molecule("F2.mol", *rxn.add_product());
		mr.read_molecule("H3NO.mol", *rxn.add_product());

		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("F2.mol", *rxn.add_agent());
		mr.read_molecule("H3NO.mol", *rxn.add_agent());

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), "RInChI=1.00.1S/"
			"CH4S/c1-2/h2H,1H3!F2/c1-2!H3NO/c1-2/h2H,1H2" + rinchi::DELIM_GROUP +
			"CH4S/c1-2/h2H,1H3!F2/c1-2!H3NO/c1-2/h2H,1H2" + rinchi::DELIM_GROUP +
			"CH4S/c1-2/h2H,1H3!F2/c1-2!H3NO/c1-2/h2H,1H2"
		"/d+", "Identical components.");
	}

	{
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_reactant());
		mr.read_molecule("F2.mol", *rxn.add_reactant());
		mr.read_molecule("H3NO.mol", *rxn.add_reactant());

		mr.read_molecule("F2.mol", *rxn.add_product());
		mr.read_molecule("CH4S.mol", *rxn.add_product());
		mr.read_molecule("H3NO.mol", *rxn.add_product());

		mr.read_molecule("H3NO.mol", *rxn.add_agent());
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("F2.mol", *rxn.add_agent());

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), "RInChI=1.00.1S/"
			"CH4S/c1-2/h2H,1H3!F2/c1-2!H3NO/c1-2/h2H,1H2" + rinchi::DELIM_GROUP +
			"CH4S/c1-2/h2H,1H3!F2/c1-2!H3NO/c1-2/h2H,1H2" + rinchi::DELIM_GROUP +
			"CH4S/c1-2/h2H,1H3!F2/c1-2!H3NO/c1-2/h2H,1H2"
		"/d+", "Identical components - 2.");
	}

}

void ReactionTests::parsing()
{
	rinchi::MdlMolfileReader mr;
	{
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_reactant());
		mr.read_molecule("F2.mol",   *rxn.add_reactant());

		mr.read_molecule("H3NO.mol", *rxn.add_product());
		mr.read_molecule("F2.mol",   *rxn.add_product());

		mr.read_molecule("H3NO.mol", *rxn.add_agent());
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("NoStructure.mol", *rxn.add_agent());

		std::string rinchi_string  = rxn.rinchi_string();
		std::string rinchi_auxinfo = rxn.rinchi_auxinfo();

		rinchi::unit_test::check_is_equal(rinchi_string, "RInChI=1.00.1S/"
			"CH4S/c1-2/h2H,1H3!F2/c1-2" + rinchi::DELIM_GROUP +
			"F2/c1-2!H3NO/c1-2/h2H,1H2" + rinchi::DELIM_GROUP +
			"CH4S/c1-2/h2H,1H3!H3NO/c1-2/h2H,1H2"
		"/d+/u0-0-1", "Parsing - RInChI string.");
		rinchi::unit_test::check_is_equal(rinchi_auxinfo, "RAuxInfo=1.00.1/"
			"0/N:1,2/rA:2nCS/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;!0/N:1,2/E:(1,2)/rA:2nFF/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;" + rinchi::DELIM_GROUP +
			"0/N:1,2/E:(1,2)/rA:2nFF/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;!0/N:1,2/rA:2nNO/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;" + rinchi::DELIM_GROUP +
			"0/N:1,2/rA:2nCS/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;!0/N:1,2/rA:2nNO/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;"
		, "Parsing - RAuxInfo.");

		rinchi::Reaction rxn2;

		rinchi::RInChIReader rinchi_reader;
		rinchi_reader.split_into_reaction(rinchi_string, rinchi_auxinfo, rxn2);

		// The 'rxn2' reaction will have its reaction components in
		// InChI key order. The original 'rxn' reaction does not 
		// necessarily have sorted components.
		// In this case 'rxn' reactants are sorted, but products and agents are not.
		rinchi::unit_test::check_is_equal(rxn.reactants().at(0)->inchi_string(), rxn2.reactants().at(0)->inchi_string(), "Reactant1 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.reactants().at(0)->inchi_auxinfo(), rxn2.reactants().at(0)->inchi_auxinfo(), "Reactant1 - AuxInfo.");
		rinchi::unit_test::check_is_equal(rxn.reactants().at(1)->inchi_string(), rxn2.reactants().at(1)->inchi_string(), "Reactant2 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.reactants().at(1)->inchi_auxinfo(), rxn2.reactants().at(1)->inchi_auxinfo(), "Reactant2 - AuxInfo.");

		rinchi::unit_test::check_is_equal(rxn.products().at(1)->inchi_string(), rxn2.products().at(0)->inchi_string(), "Product1 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.products().at(1)->inchi_auxinfo(), rxn2.products().at(0)->inchi_auxinfo(), "Product1 - AuxInfo.");
		rinchi::unit_test::check_is_equal(rxn.products().at(0)->inchi_string(), rxn2.products().at(1)->inchi_string(), "Product2 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.products().at(0)->inchi_auxinfo(), rxn2.products().at(1)->inchi_auxinfo(), "Product2 - AuxInfo.");

		rinchi::unit_test::check_is_equal(rxn.agents().at(1)->inchi_string(), rxn2.agents().at(0)->inchi_string(), "Agent1 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(1)->inchi_auxinfo(), rxn2.agents().at(0)->inchi_auxinfo(), "Agent1 - AuxInfo.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(0)->inchi_string(), rxn2.agents().at(1)->inchi_string(), "Agent2 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(0)->inchi_auxinfo(), rxn2.agents().at(1)->inchi_auxinfo(), "Agent2 - AuxInfo.");
	        rinchi::unit_test::check_is_equal(rxn.agents().at(2)->inchi_string(), rxn2.agents().at(2)->inchi_string(), "Agent3 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(2)->inchi_auxinfo(), rxn2.agents().at(2)->inchi_auxinfo(), "Agent3 - AuxInfo.");
	}

	// Once again, this time with reverse direction.
	{
		rinchi::Reaction rxn;
		mr.read_molecule("H3NO.mol", *rxn.add_reactant());
		mr.read_molecule("F2.mol",   *rxn.add_reactant());

		mr.read_molecule("CH4S.mol", *rxn.add_product());
		mr.read_molecule("F2.mol",   *rxn.add_product());

		mr.read_molecule("H3NO.mol", *rxn.add_agent());
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("NoStructure.mol", *rxn.add_agent());

		std::string rinchi_string  = rxn.rinchi_string();
		std::string rinchi_auxinfo = rxn.rinchi_auxinfo();

		rinchi::unit_test::check_is_equal(rinchi_string, "RInChI=1.00.1S/"
			"CH4S/c1-2/h2H,1H3!F2/c1-2" + rinchi::DELIM_GROUP +
			"F2/c1-2!H3NO/c1-2/h2H,1H2" + rinchi::DELIM_GROUP +
			"CH4S/c1-2/h2H,1H3!H3NO/c1-2/h2H,1H2"
		"/d-/u0-0-1", "Parsing - RInChI string.");
		rinchi::unit_test::check_is_equal(rinchi_auxinfo, "RAuxInfo=1.00.1/"
			"0/N:1,2/rA:2nCS/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;!0/N:1,2/E:(1,2)/rA:2nFF/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;" + rinchi::DELIM_GROUP +
			"0/N:1,2/E:(1,2)/rA:2nFF/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;!0/N:1,2/rA:2nNO/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;" + rinchi::DELIM_GROUP +
			"0/N:1,2/rA:2nCS/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;!0/N:1,2/rA:2nNO/rB:s1;/rC:-4.2134,-3.4179,0;-3.4989,-3.8304,0;"
		, "Parsing - RAuxInfo.");

		rinchi::Reaction rxn2;

		rinchi::RInChIReader rinchi_reader;
		rinchi_reader.split_into_reaction(rinchi_string, rinchi_auxinfo, rxn2);

		rinchi::unit_test::check_is_equal(rxn.reactants().at(1)->inchi_string(), rxn2.reactants().at(0)->inchi_string(), "Reactant1 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.reactants().at(1)->inchi_auxinfo(), rxn2.reactants().at(0)->inchi_auxinfo(), "Reactant1 - AuxInfo.");
		rinchi::unit_test::check_is_equal(rxn.reactants().at(0)->inchi_string(), rxn2.reactants().at(1)->inchi_string(), "Reactant2 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.reactants().at(0)->inchi_auxinfo(), rxn2.reactants().at(1)->inchi_auxinfo(), "Reactant2 - AuxInfo.");

		rinchi::unit_test::check_is_equal(rxn.products().at(0)->inchi_string(), rxn2.products().at(0)->inchi_string(), "Product1 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.products().at(0)->inchi_auxinfo(), rxn2.products().at(0)->inchi_auxinfo(), "Product1 - AuxInfo.");
		rinchi::unit_test::check_is_equal(rxn.products().at(1)->inchi_string(), rxn2.products().at(1)->inchi_string(), "Product2 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.products().at(1)->inchi_auxinfo(), rxn2.products().at(1)->inchi_auxinfo(), "Product2 - AuxInfo.");

		rinchi::unit_test::check_is_equal(rxn.agents().at(1)->inchi_string(), rxn2.agents().at(0)->inchi_string(), "Agent1 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(1)->inchi_auxinfo(), rxn2.agents().at(0)->inchi_auxinfo(), "Agent1 - AuxInfo.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(0)->inchi_string(), rxn2.agents().at(1)->inchi_string(), "Agent2 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(0)->inchi_auxinfo(), rxn2.agents().at(1)->inchi_auxinfo(), "Agent2 - AuxInfo.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(2)->inchi_string(), rxn2.agents().at(2)->inchi_string(), "Agent3 - InChI.");
		rinchi::unit_test::check_is_equal(rxn.agents().at(2)->inchi_auxinfo(), rxn2.agents().at(2)->inchi_auxinfo(), "Agent3 - AuxInfo.");
	}
}

void ReactionTests::no_structure_handling_from_rxn()
{
	rinchi::MdlRDfileReader rd_reader;
	{
		std::ifstream rdfile ("NoStructures.rdf");
		rinchi::Reaction rxn;
		rd_reader.read_reaction(rdfile, rxn);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), "RInChI=1.00.1S/"
			"C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP +
			"C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" + rinchi::DELIM_GROUP +
			"Na.H2O/h;1H2/q+1;/p-1"
		"/d-/u2-1-2", "RInChI");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(), "RAuxInfo=1.00.1/"
			"0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;" + rinchi::DELIM_GROUP +
			"0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;" + rinchi::DELIM_GROUP +
			"1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;"
		, "RAuxInfo");
		rinchi::unit_test::check_is_equal(rxn.rinchi_long_key(), "Long-RInChIKey=SA-BUHFF-"
			"PQXKWPLDPFFDJP-WUCPZUCCSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N--"
			"JCYSVJNMXBWPHS-DMTCNVIQSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N--"
			"HEMHJVSKTPXQMS-UHFFFAOYSA-M-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N"
		, "Long key");
		rinchi::unit_test::check_is_equal(rxn.rinchi_short_key(),
			"Short-RInChIKey=SA-BUHFF-PQXKWPLDPF-JCYSVJNMXB-HEMHJVSKTP-NIBFB-NMAHA-MUHFF-BAB"
		, "Short key");
	}
}

void ReactionTests::no_structure_handling_from_rinchi()
{
	rinchi::RInChIReader rinchi_reader;
	{
		rinchi::Reaction rxn;
		rinchi_reader.split_into_reaction(
			"RInChI=1.00.1S/"
			"/!/!C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP +
			"/!C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" + rinchi::DELIM_GROUP +
			"/!/!Na.H2O/h;1H2/q+1;/p-1"
			"/d-",
			"RAuxInfo=1.00.1/"
			"/!/!0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;" + rinchi::DELIM_GROUP +
			"/!0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;" + rinchi::DELIM_GROUP +
			"/!/!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;",
			rxn);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), "RInChI=1.00.1S/"
			"C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP +
			"C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" + rinchi::DELIM_GROUP +
			"Na.H2O/h;1H2/q+1;/p-1"
		"/d-/u2-1-2", "RInChI");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(), "RAuxInfo=1.00.1/"
			"0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;" + rinchi::DELIM_GROUP +
			"0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;" + rinchi::DELIM_GROUP +
			"1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;"
		, "RAuxInfo");
		rinchi::unit_test::check_is_equal(rxn.rinchi_long_key(), "Long-RInChIKey=SA-BUHFF-"
			"PQXKWPLDPFFDJP-WUCPZUCCSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N--"
			"JCYSVJNMXBWPHS-DMTCNVIQSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N--"
			"HEMHJVSKTPXQMS-UHFFFAOYSA-M-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N"
		, "Long key");
		rinchi::unit_test::check_is_equal(rxn.rinchi_short_key(),
			"Short-RInChIKey=SA-BUHFF-PQXKWPLDPF-JCYSVJNMXB-HEMHJVSKTP-NIBFB-NMAHA-MUHFF-BAB"
		, "Short key");
	}
	{
		// Check splitting without RAuxInfo.
		rinchi::Reaction rxn;
		rinchi_reader.split_into_reaction(
			"RInChI=1.00.1S/"
			"/!/!C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP +
			"/!C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" + rinchi::DELIM_GROUP +
			"/!/!Na.H2O/h;1H2/q+1;/p-1"
			"/d-",
			"",
			rxn);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), "RInChI=1.00.1S/"
			"C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP +
			"C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" + rinchi::DELIM_GROUP +
			"Na.H2O/h;1H2/q+1;/p-1"
			"/d-/u2-1-2", "RInChI, no AuxInfo");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(),
			"RAuxInfo=1.00.1//" + rinchi::DELIM_GROUP + "/" + rinchi::DELIM_GROUP + "/"
			, "RAuxInfo - allow, no AuxInfo");
		rinchi::unit_test::check_is_equal(rxn.rinchi_long_key(), "Long-RInChIKey=SA-BUHFF-"
			"PQXKWPLDPFFDJP-WUCPZUCCSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N--"
			"JCYSVJNMXBWPHS-DMTCNVIQSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N--"
			"HEMHJVSKTPXQMS-UHFFFAOYSA-M-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N"
			, "Long key, no AuxInfo");
		rinchi::unit_test::check_is_equal(rxn.rinchi_short_key(),
			"Short-RInChIKey=SA-BUHFF-PQXKWPLDPF-JCYSVJNMXB-HEMHJVSKTP-NIBFB-NMAHA-MUHFF-BAB"
			, "Short key, no AuxInfo");
	}

}

void ReactionTests::web_key()
{
	rinchi::MdlMolfileReader mr;
	{
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_reactant());
		mr.read_molecule("F2.mol", *rxn.add_reactant());
		mr.read_molecule("H3NO.mol", *rxn.add_product());

		rinchi::unit_test::check_is_equal(rxn.rinchi_web_key(),
			"Web-RInChIKey=YPIAMIHIVMKRNHGRL-NUHFFFADPSCTJSA", "Web key, simple rxn");
	}
	{
		// Adding a chiral molecule affects the minor layer.
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_reactant());
		mr.read_molecule("F2.mol", *rxn.add_reactant());
		mr.read_molecule("H3NO.mol", *rxn.add_product());
		mr.read_molecule("WithStereo.mol", *rxn.add_product());

		rinchi::unit_test::check_is_equal(rxn.rinchi_web_key(),
			"Web-RInChIKey=SVWZFSCOXGIJJYBSH-NLFJDGMNFEUVOSA", "Web key, simple rxn with stereo");
	}
	{
		// The web key is invariant to the role of components. Each component counts
		// the same whether it is a reactant, a product or an agent.
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("F2.mol", *rxn.add_product());
		mr.read_molecule("H3NO.mol", *rxn.add_reactant());

		rinchi::unit_test::check_is_equal(rxn.rinchi_web_key(),
			"Web-RInChIKey=YPIAMIHIVMKRNHGRL-NUHFFFADPSCTJSA", "Web key, simple rxn, switched component roles");
	}
	{
		// Duplicate components are collapsed into a single entry and thus won't
		// have an effect on the web key.
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("F2.mol", *rxn.add_product());
		mr.read_molecule("H3NO.mol", *rxn.add_reactant());

		mr.read_molecule("F2.mol", *rxn.add_product());
		mr.read_molecule("CH4S.mol", *rxn.add_product());
		mr.read_molecule("H3NO.mol", *rxn.add_product());

		mr.read_molecule("H3NO.mol", *rxn.add_agent());
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("F2.mol", *rxn.add_agent());

		rinchi::unit_test::check_is_equal(rxn.rinchi_web_key(),
			"Web-RInChIKey=YPIAMIHIVMKRNHGRL-NUHFFFADPSCTJSA", "Web key, multiple occurrences of components");
	}
	{
		// No-Structures count as any other component.
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("F2.mol", *rxn.add_product());
		mr.read_molecule("H3NO.mol", *rxn.add_reactant());

		mr.read_molecule("NoStructure.mol", *rxn.add_reactant());

		rinchi::unit_test::check_is_equal(rxn.rinchi_web_key(),
			"Web-RInChIKey=JRSKCAIHQDTLICXUK-NUHFFFADPSCTJSA", "Web key, single No-Structure");
	}
	{
		// Multiple No-Structures is the same as one No-Structure.
		rinchi::Reaction rxn;
		mr.read_molecule("CH4S.mol", *rxn.add_agent());
		mr.read_molecule("F2.mol", *rxn.add_product());
		mr.read_molecule("H3NO.mol", *rxn.add_reactant());

		mr.read_molecule("NoStructure.mol", *rxn.add_reactant());
		mr.read_molecule("NoStructure.mol", *rxn.add_reactant());
		mr.read_molecule("NoStructure.mol", *rxn.add_product());
		mr.read_molecule("NoStructure.mol", *rxn.add_product());
		mr.read_molecule("NoStructure.mol", *rxn.add_agent());
		mr.read_molecule("NoStructure.mol", *rxn.add_agent());

		rinchi::unit_test::check_is_equal(rxn.rinchi_web_key(),
			"Web-RInChIKey=JRSKCAIHQDTLICXUK-NUHFFFADPSCTJSA", "Web key, multiple No-Structures");
	}
}

} // end of namespace
