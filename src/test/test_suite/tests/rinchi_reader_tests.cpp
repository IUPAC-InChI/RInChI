#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../../../LICENCE.txt.
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
			rinchi::unit_test::check_errmsg_has_substr(e.what(), errmsg_expected, "Case " + case_no);
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
		"RInChI=1.00.1S/H3NO/c1-2/h2H,1H2/ hummus", "", "Invalid trailing text in component InChI 'H3NO/c1-2/h2H,1H2/ hummus'."
	);
	// RAuxInfo is not allowed as trailing data to the InChI.
	check_rinchi_parsing_error(
		"02",
		"RInChI=1.00.1S/<>H3NO/c1-2/h2H,1H2/d- RAuxInfo=1.00.1/<>/", "", "Invalid trailing text in component InChI 'H3NO/c1-2/h2H,1H2/d- RAuxInfo=1.00.1/'."
	);
	check_rinchi_parsing_error(
		"03",
		"RInChI=1.00.1S/<>H3NO/c1-2/h2H,1H2/d-\nRAuxInfo=1.00.1/<>/", "", "Invalid trailing text in component InChI 'H3NO/c1-2/h2H,1H2/d-\nRAuxInfo=1.00.1/'."
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

namespace {

	void check_rinchi_text_load(
		const std::string& case_no,
		const std::string& reactant_inchis, const std::string& product_inchis, const std::string& agent_inchis,
		const std::string& rinchi_expected, const std::string& rauxinfo_expected
	)
	{
		rinchi::RInChIReader rdr;
		rinchi::Reaction rxn;
		rdr.add_inchis_to_reaction(reactant_inchis, product_inchis, agent_inchis, rxn);

		rinchi::unit_test::check_is_equal(rxn.rinchi_string(), rinchi_expected, case_no + " RInChI string");
		rinchi::unit_test::check_is_equal(rxn.rinchi_auxinfo(), rauxinfo_expected, case_no + " RAuxInfo");
	}

    void check_rinchi_text_load_error(
		const std::string& case_no,
		const std::string& reactant_inchis, const std::string& product_inchis, const std::string& agent_inchis,
		const std::string& errmsg_expected
	)
	{
		rinchi::RInChIReader rdr;
		rinchi::Reaction rxn;
        try {
            rdr.add_inchis_to_reaction(reactant_inchis, product_inchis, agent_inchis, rxn);
            rinchi::unit_test::check_is_equal(true, false, case_no + ": No error raised as expected.");
        } catch (std::exception& e) {
            rinchi::unit_test::check_errmsg_has_substr(e.what(), errmsg_expected);
        }
	}

} // end of anon. namespace.

void RInChIReaderTests::load_inchis_from_text()
{
    // R005a.rxn with RAuxInfo.
    check_rinchi_text_load(
        "01",
        "InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;" "\n"
        "InChI=1S/Na.H2O/h;1H2/q+1;/p-1" "\n"
        "AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;"
        ,
        "InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;"
        ,
        ""
        ,
        "RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-",
        "RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;<>0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;"
    );
    // Same as case 01, but with a trailing blank line after reactants and products.
    check_rinchi_text_load(
        "02",
        "InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;" "\n"
        "InChI=1S/Na.H2O/h;1H2/q+1;/p-1" "\n"
        "AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;" "\n"
        ,
        "InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;"
        ,
        ""
        ,
        "RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-",
        "RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;<>0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;"
    );
    // Same as case 01, but with no AuxInfo.
    check_rinchi_text_load(
        "03",
        "InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" "\n"
        "InChI=1S/Na.H2O/h;1H2/q+1;/p-1" "\n"
        ,
        "InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" "\n"
        ,
        ""
        ,
        "RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-",
        "RAuxInfo=1.00.1//<>/!/"
    );

    // Same as case 01, but with AuxInfo on only second reactant.
    check_rinchi_text_load(
        "04",
        "InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" "\n"
        "InChI=1S/Na.H2O/h;1H2/q+1;/p-1" "\n"
        "AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;" "\n"
        ,
        "InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" "\n"
        ,
        ""
        ,
        "RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-",
        "RAuxInfo=1.00.1//<>/!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;"
    );

    // === Error scenarios ===

    // Same as case 02, but with data after a blank line in reactants. This should trigger an "EOF expected" error.
    check_rinchi_text_load_error(
        "11",
        "InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;" "\n"
        "\n"
        "InChI=1S/Na.H2O/h;1H2/q+1;/p-1" "\n"
        "AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;" "\n"
        ,
        "InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;"
        ,
        ""
        ,
        "Line 4: Unexpected trailing data; expected an EOF after previous blank line."
    );
    // Same as case 02, but with the product's AuxInfo line truncated, rendering it invalid.
    check_rinchi_text_load_error(
        "12",
        "InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;" "\n"
        "InChI=1S/Na.H2O/h;1H2/q+1;/p-1" "\n"
        "AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;" "\n"
        ,
        "InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1" "\n"
        "AuxInfo=1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0"
        ,
        ""
        ,
        "Invalid AuxInfo 'AuxInfo=1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0' for a reaction component."
    );
}

} // end of namespace
