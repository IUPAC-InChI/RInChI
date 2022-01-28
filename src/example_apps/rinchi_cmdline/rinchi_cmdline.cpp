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

#include <iostream>
#include <fstream>

#include "rinchi_utils.h"
#include "rinchi_reaction.h"

#include "rinchi_consts.h"
#include "rinchi_reader.h"
#include "mdl_rxnfile.h"
#include "mdl_rxnfile_reader.h"
#include "mdl_rdfile_reader.h"
#include "mdl_rxnfile_writer.h"
#include "mdl_rdfile_writer.h"

bool has_parameter(const std::string& p, int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
		if (rinchi::lowercase(p) == rinchi::lowercase(std::string(argv[i])))
			return true;

	return false;
}

int main(int argc, char* argv[])
{
	try {
		// Adding an option that allows 2D coordinate reconstruction from RInChI strings
		// without RAuxInfo requires linking to a 3rd party ChemInformatics toolkit.
		if (argc < 2) {
			std::cerr << "Usage: rinchi_cmdline <filename> <options>" << "\n"
				<< "       <filename> may be an RXN or RD file, or a file containing an RInChI" << "\n"
				<< "                  string optionally followed by a linefeed and RAuxInfo data." << "\n"
				<< "       <options>" << "\n"
				<< "            /force-equilibrium" << "\n"
				<< "                Forces the reaction to be interpreted as an equilibrium reaction." << "\n"
				<< "                Only relevant for RD or RXN file input." << "\n"
				<< "            /rd or /rxn" << "\n"
				<< "                Force output file format of reconstructed reaction file to be RD" << "\n"
				<< "                or RXN." << "\n"
				<< "                Only relevant for RInChI string input." << "\n"
				<< "                If the output file format is not forced, this utility will choose" << "\n"
				<< "                to write RD files if the reaction has agents, and RXN files if not." << "\n"
				<< std::endl;
			return 1;
		}

		bool option_force_equilbrium = has_parameter("/force-equilibrium", argc, argv);
		bool option_output_rd        = has_parameter("/rd", argc, argv);
		bool option_output_rxn       = has_parameter("/rxn", argc, argv);

		if (option_output_rd && option_output_rxn)
			throw std::runtime_error ("Only one of /rd and /rxn may be used at a time.");

		const char* file_name;
		std::string file_format;

		file_name = argv[1];
		{
			std::ifstream test_file(file_name);
			if (!test_file) 
				throw std::runtime_error (std::string("File '") + file_name + "' does not exist.");

			// Detect input file format.
			std::string first_line;
			rinchi::rinchi_getline(test_file, first_line);
			if (first_line == rinchi::MDL_TAG_RXN_BEGIN)
				file_format = "RXN";
			else if (first_line.substr(0, rinchi::RINCHI_STD_HEADER.length()) == rinchi::RINCHI_STD_HEADER)
				file_format = "RINCHI";
			else
				file_format = "RD";
		}

		rinchi::Reaction rxn;

		if (file_format == "RXN") {
			rinchi::MdlRxnfileReader rxn_rdr;
			rxn_rdr.read_reaction(file_name, rxn, option_force_equilbrium);
		}
		else if (file_format == "RD") {
			rinchi::MdlRDfileReader rd_rdr;
			rd_rdr.read_reaction(file_name, rxn, option_force_equilbrium);
		}
		else if (file_format == "RINCHI") {
			std::string rinchi_string;
			std::string rauxinfo;

			std::ifstream rinchi_file(file_name);
			rinchi::rinchi_getline(rinchi_file, rinchi_string);
			rinchi::rinchi_getline(rinchi_file, rauxinfo);

			rinchi::RInChIReader rinchi_rdr;
			rinchi_rdr.split_into_reaction(rinchi_string, rauxinfo, rxn);

			bool output_rd = (rxn.agents().size() > 0 || option_output_rd) && !option_output_rxn;
			if (output_rd) {
				rinchi::MdlRDfileWriter rd_writer;
				rd_writer.write_reaction(rxn, std::cout);
			}
			else {
				rinchi::MdlRxnfileWriter rxn_writer;
				rxn_writer.write_reaction(rxn, std::cout);
			}
		}
		else
			throw std::runtime_error ("Internal bug: Unsupported file format '" + file_format + "'.");

		if (file_format != "RINCHI") {
			std::cout << rxn.rinchi_string() << std::endl;
			std::cout << rxn.rinchi_auxinfo() << std::endl;
			std::cout << rxn.rinchi_long_key() << std::endl;
			std::cout << rxn.rinchi_short_key() << std::endl;
			std::cout << rxn.rinchi_web_key() << std::endl;
		}

		return 0;
	} catch (std::exception& e) {
		std::cerr << "ERROR: " << rinchi::demangled_class_name(typeid(e).name()) << ": " << e.what() << std::endl;
		return 2;
	}
}
