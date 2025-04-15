#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../../LICENCE.txt.
#pragma endregion
#endif

#include <iostream>

#include "rinchi_reaction.h"
#include "mdl_molfile_reader.h"

/**
  In some cases you may not have your reaction components ready in RXN
  or RD format. This example program shows how to use the RInChI C++ API
  to build an RInChI reaction object from individual molfiles and generate
  a corresponding RInChI string and keys from that.
  
  Below, we create an RInChI instance and loads the reaction with
  
    * One reactant.
	* One product.
	* Two agents.

  The RInChI reaction object can then produce an RInChI string of
  
    RInChI=0.03.1S/C18H13NO5S2/c1-12-2-4-14(5-3-12)26(21,22)19-13-6-9-18(10-7-13)23-15-8-11-25-16(15)17(20)24-18/
	h2-11H,1H3/b19-13-<>C18H15NO5S2/c1-12-2-8-15(9-3-12)26(22,23)19-13-4-6-14(7-5-13)24-16-10-11-25-17(16)18(20)21/
	h2-11,19H,1H3,(H,20,21)<>C2H3N/c1-2-3/h1H3!C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/
	q+1;/p-1/d-
  
  Breaking down that RInChI string, separating on "<>" and "!" delimiters:

	Reactant 01:
	C18H13NO5S2/c1-12-2-4-14(5-3-12)26(21,22)19-13-6-9-18(10-7-13)23-15-8-11-25-16(15)17(20)24-18/h2-11H,1H3/b19-13-
  
    Product 01:
    C18H15NO5S2/c1-12-2-8-15(9-3-12)26(22,23)19-13-4-6-14(7-5-13)24-16-10-11-25-17(16)18(20)21/h2-11,19H,1H3,(H,20,21)

	Agent 01:
    C2H3N/c1-2-3/h1H3

	Agent 02:
	C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/q+1;/p-1

	Direction flag:
	/d-

  Once you have the RInChI string generated, the keys will uniquely
  identify that RInChI string.
**/

int main(int argc, char* argv[])
{
	try {
		rinchi::Reaction rxn;
		rinchi::ReactionComponent* rc;
		rinchi::MdlMolfileReader mol_reader;

		// Load reactant(s).
		rc = rxn.add_reactant();
		mol_reader.read_molecule("../data/reactant_01.mol", *rc);
		// Load product(s).
		rc = rxn.add_product();
		mol_reader.read_molecule("../data/product_01.mol", *rc);
		// Load agent(s).
		rc = rxn.add_agent();
		mol_reader.read_molecule("../data/agent_01.mol", *rc);

		rc = rxn.add_agent();
		mol_reader.read_molecule("../data/agent_02.mol", *rc);

		std::cout << "RInChI string    = " << rxn.rinchi_string() << std::endl;
		std::cout << "RInChI long key  = " << rxn.rinchi_long_key() << std::endl;
		std::cout << "RInChI short key = " << rxn.rinchi_short_key() << std::endl;
		std::cout << "RInChI web key   = " << rxn.rinchi_web_key() << std::endl;
		return 0;
	} catch (std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
