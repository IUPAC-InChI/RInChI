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
#include "rinchi_reader_tests.h"

#include "rinchi_reaction.h"
#include "rinchi_reader.h"

namespace rinchi_tests {

namespace {

	void check_rinchi_parsing_error(
		const std::string& case_no,
		const std::string& rinchi_string,
		const std::string& rauxinfo,
		const std::string& errmsg_expected
	)
	{
		rinchi::RInChIReader rdr;
		rinchi::Reaction rxn;
		try {
			rdr.split_into_reaction(rinchi_string, rauxinfo, rxn);
			throw rinchi::unit_test::TestFailure("Case " + case_no + " did not fail as expected.");
		} catch (std::exception& e) {
			rinchi::unit_test::check_errmsg_has_substr(e.what(), errmsg_expected);
		}
	}

	// When blank RAuxInfo is passed in, the RInChI reaction will still produce RAuxInfo output
	// since every reaction component must have a corresponding entry in RAuxInfo even when
	// blank.
	void check_rinchi_parsing(
		const std::string& case_no,
		const std::string& rinchi_string,
		const std::string& rauxinfo, const std::string& rauxinfo_expected
	)
	{
		rinchi::RInChIReader rdr;
		rinchi::Reaction rxn;
		rdr.split_into_reaction(rinchi_string, rauxinfo, rxn);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), rinchi_string, case_no + " RInChI string");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(), rauxinfo_expected, case_no + " RAuxInfo");
	}

} // end of anon. namespace.

void RInChIReaderTests::error_cases()
{
	check_rinchi_parsing_error(
		"01",
		"RInChI=1.00.1S/H3NO/c1-2/h2H,1H2/ hummus", "", "Structure: 1 Syntax error (-2) in MOBILE_H_ISO_LAYER_FORK (9)"
	);
	// RAuxInfo is not allowed as trailing data to the InChI.
	check_rinchi_parsing_error(
		"02",
		"RInChI=1.00.1S/<>H3NO/c1-2/h2H,1H2/d- RAuxInfo=1.00.1/<>/", "", "Structure: 1 Syntax error (-2) in MOBILE_H_ISO_LAYER_FORK (9)"
	);
	check_rinchi_parsing_error(
		"03",
		"RInChI=1.00.1S/<>H3NO/c1-2/h2H,1H2/d-\nRAuxInfo=1.00.1/<>/", "", "Structure: 1 Syntax error (-2) in MOBILE_H_ISO_LAYER_FORK (9)"
	);

	// Invalid RAuxInfo.
	check_rinchi_parsing_error(
		"04",
		"RInChI=1.00.1S/<>H3NO/c1-2/h2H,1H2/d-", "RAuxInfo=1.00.1/<><>/", "RAuxInfo contains too many elements"
	);
	check_rinchi_parsing_error(
		"05",
		"RInChI=1.00.1S/<>H3NO/c1-2/h2H,1H2/d-", "RAuxInfo=1.00.1/<>invaliddata/", "Invalid AuxInfo 'AuxInfo=1/invaliddata/' for a reaction component."
	);
}

void RInChIReaderTests::trivial_cases()
{
	// R005a.rxn.
	check_rinchi_parsing(
		"01",
		"RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP + "C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-/u0-1-0",
		"", "RAuxInfo=1.00.1//" + rinchi::DELIM_GROUP + "/!/"
	);
	// R005a.rxn with d+ direction.
	check_rinchi_parsing(
		"02",
		"RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP + "C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d+",
		"", "RAuxInfo=1.00.1//" + rinchi::DELIM_GROUP + "/!/"
	);
	// R005a.rxn with RAuxInfo.
	check_rinchi_parsing(
		"03",
		"RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP + "C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d+",
		"RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;" + rinchi::DELIM_GROUP + "0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;",
		"RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;" + rinchi::DELIM_GROUP + "0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;"
	);

	// An RInChI with two agents - Example 01 CCR.
	check_rinchi_parsing(
		"04",
		"RInChI=1.00.1S/C18H13NO5S2/c1-12-2-4-14(5-3-12)26(21,22)19-13-6-9-18(10-7-13)23-15-8-11-25-16(15)17(20)24-18/h2-11H,1H3/b19-13-" + rinchi::DELIM_GROUP + "C18H15NO5S2/c1-12-2-8-15(9-3-12)26(22,23)19-13-4-6-14(7-5-13)24-16-10-11-25-17(16)18(20)21/h2-11,19H,1H3,(H,20,21)" + rinchi::DELIM_GROUP + "C2H3N/c1-2-3/h1H3!C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/q+1;/p-1/d-",
		"", "RAuxInfo=1.00.1//" + rinchi::DELIM_GROUP + "/" + rinchi::DELIM_GROUP + "/!/"
	);
}

void RInChIReaderTests::reactants_only()
{
	// An RInChI with one reactant only.
	check_rinchi_parsing(
		"01",
		"RInChI=1.00.1S/<>C5H12O/c1-4(2)5(3)6/h4-6H,1-3H3/d-",
		"", "RAuxInfo=1.00.1/<>/"
	);
	// An RInChI with two reactants only.
	check_rinchi_parsing(
		"02",
		"RInChI=1.00.1S/<>C5H12O/c1-4(2)5(3)6/h4-6H,1-3H3!C8H8O2/c9-8(10)6-7-4-2-1-3-5-7/h1-5H,6H2,(H,9,10)/d-",
		"", "RAuxInfo=1.00.1/<>/!/"
	);
}

void RInChIReaderTests::products_only()
{
	// An RInChI with one product only.
	check_rinchi_parsing(
		"01",
		"RInChI=1.00.1S/<>C8H8O2/c9-8(10)6-7-4-2-1-3-5-7/h1-5H,6H2,(H,9,10)/d+",
		"", "RAuxInfo=1.00.1/<>/"
	);
	// An RInChI with one product only and RAuxInfo.
	check_rinchi_parsing(
		"02",
		"RInChI=1.00.1S/<>C8H8O2/c9-8(10)6-7-4-2-1-3-5-7/h1-5H,6H2,(H,9,10)/d+",
		"RAuxInfo=1.00.1/<>1/N:3,2,4,1,5,7,6,8,9,10/E:(2,3)(4,5)(9,10)/rA:10nCCCCCCCCOO/rB:d1;s2;d3;s4;s1d5;s6;s7;s8;d8;/rC:14.2607,3.2411,0;13.5462,2.8286,0;13.5462,2.0036,0;14.2607,1.5911,0;14.9752,2.0036,0;14.9752,2.8286,0;15.6897,3.2411,0;16.4041,2.8286,0;17.1186,3.2411,0;16.4041,2.0036,0;",
		"RAuxInfo=1.00.1/<>1/N:3,2,4,1,5,7,6,8,9,10/E:(2,3)(4,5)(9,10)/rA:10nCCCCCCCCOO/rB:d1;s2;d3;s4;s1d5;s6;s7;s8;d8;/rC:14.2607,3.2411,0;13.5462,2.8286,0;13.5462,2.0036,0;14.2607,1.5911,0;14.9752,2.0036,0;14.9752,2.8286,0;15.6897,3.2411,0;16.4041,2.8286,0;17.1186,3.2411,0;16.4041,2.0036,0;"
	);
	// An RInChI with two products only.
	check_rinchi_parsing(
		"03",
		"RInChI=1.00.1S/<>C5H12O/c1-4(2)5(3)6/h4-6H,1-3H3!C8H8O2/c9-8(10)6-7-4-2-1-3-5-7/h1-5H,6H2,(H,9,10)/d+",
		"", "RAuxInfo=1.00.1/<>/!/"
	);
}

void RInChIReaderTests::agents_only()
{
	/**
	The Cambridge 0.02 implementation could produce agent-only RInChI. An example is:

		RInChI=0.02.1S///////C10H12BrN3/c1-6(12)10-7(11)5-9-8(13-10)3-4-14(9)2/h3-6H,12H2,1-2H3//C3H8O/c1-3(2)4/h3-4H,1-2H3//H2O/h1H2/d+

	which in InChI 1.00 format is:
	
		RInChI=1.00.1S/<><>C10H12BrN3/c1-6(12)10-7(11)5-9-8(13-10)3-4-14(9)2/h3-6H,12H2,1-2H3!C3H8O/c1-3(2)4/h3-4H,1-2H3!H2O/h1H2/d+

	**/
	check_rinchi_parsing(
		"01",
		"RInChI=1.00.1S/<><>C10H12BrN3/c1-6(12)10-7(11)5-9-8(13-10)3-4-14(9)2/h3-6H,12H2,1-2H3!C3H8O/c1-3(2)4/h3-4H,1-2H3!H2O/h1H2/d+",
		"", "RAuxInfo=1.00.1/<><>/!/!/"
	);
}

void RInChIReaderTests::no_structures()
{
	// An RInChI with d- direction and a No-Structure reactant.
	check_rinchi_parsing(
		"01",
		"RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" + rinchi::DELIM_GROUP + "C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-/u0-1-0",
		"", "RAuxInfo=1.00.1//" + rinchi::DELIM_GROUP + "/!/"
	);

	/** More in-depth No-Structure parsing is covered in ReactionTests test suite. **/
}

} // end of namespace
