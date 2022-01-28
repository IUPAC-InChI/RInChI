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
#include <vector>

#include "rinchi_utils.h"

#include "mdl_rxnfile.h"
#include "mdl_rxnfile_reader.h"
#include "mdl_molfile_reader.h"

using namespace std;

namespace rinchi {

void MdlRxnfileReader::read_reaction_from_stream(std::istream& input_stream, Reaction& result, bool force_equilibrium)
{
	try {
		get_next_line(input_stream);
		if (m_current_line != MDL_TAG_RXN_BEGIN)
			throw MdlRxnfileReaderError("RXN files must begin with a '" + MDL_TAG_RXN_BEGIN + "' line.");

		// Skip name line.
		get_next_line(input_stream);
		// Skip source-and-timestamp line.
		get_next_line(input_stream);
		// Skip comment line.
		get_next_line(input_stream);

		// Read component count line.
		get_next_line(input_stream);
		check(m_current_line.length() == 6, "Invalid component count line - must be 6 characters long.");
		int reactant_count = str2int(m_current_line.substr(0, 3));
		int product_count  = str2int(m_current_line.substr(3, 3));

		MdlMolfileReader mr;

		for (int i = 0; i < reactant_count; i++) {
			get_next_line(input_stream);
			if (m_current_line != MDL_TAG_RXN_COMPONENT_START)
				throw MdlRxnfileReaderError("Reaction components must be delimited by a '" + MDL_TAG_RXN_COMPONENT_START + "' line.");
			ReactionComponent* rc = result.add_reactant();
			mr.read_molecule(input_stream, *rc);
			m_line_number += mr.lines_read();
		}

		for (int i = 0; i < product_count; i++) {
			get_next_line(input_stream);
			if (m_current_line != MDL_TAG_RXN_COMPONENT_START)
				throw MdlRxnfileReaderError("Reaction components must be delimited by a '" + MDL_TAG_RXN_COMPONENT_START + "' line.");
			ReactionComponent* rc = result.add_product();
			mr.read_molecule(input_stream, *rc);
			m_line_number += mr.lines_read();
		}

		if (force_equilibrium)
			result.set_directionality(rdEquilibrium);
	}
	catch (exception& e) {
		throw_error(e);
	}
}

void MdlRxnfileReader::read_reaction(std::istream& input_stream, Reaction& result, bool force_equilibrium, int lines_already_read)
{
	m_input_name = "std::istream";
	m_line_number = lines_already_read;

	if (!input_stream)
		throw_error("Input RXN file stream is not open");

	read_reaction_from_stream(input_stream, result, force_equilibrium);
}

void MdlRxnfileReader::read_reaction(const std::string& file_name, Reaction& result, bool force_equilibrium, int lines_already_read)
{
	m_input_name  = file_name;
	m_line_number = lines_already_read;

	ifstream input_stream ( file_name.c_str() );
	if (!input_stream)
		throw_error("Can't open input RXN file '" + file_name + "'");

	read_reaction_from_stream(input_stream, result, force_equilibrium);
}


} // end of namespace
