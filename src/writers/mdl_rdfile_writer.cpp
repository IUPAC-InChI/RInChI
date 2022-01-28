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
#include <iomanip>

#include <rinchi_utils.h>

#include <mdl_rdfile.h>
#include <mdl_rdfile_writer.h>

#include <mdl_rxnfile_writer.h>
#include <inchi_generator.h>

#include <ctime>

namespace {

	std::string rdfile_timestamp()
	{
		// Get current time.
		time_t t = time(0);
		std::tm current_time;
	#ifdef ON_WINDOWS
		int err = localtime_s(&current_time, &t);
		if (err)
			throw std::runtime_error("Invalid argument to localtime_s.");
	#else
		current_time = *localtime(&t);
	#endif

		const int MAX_DATE_FORMAT_STRING_LENGTH = 200;
		char tmdescr[MAX_DATE_FORMAT_STRING_LENGTH] = {0};
		if ( strftime(tmdescr, sizeof(tmdescr) - 1, "%m/%d/%Y %H:%M:%S", &current_time) > 0 )
			return tmdescr;
		else
			return "N/A";
	}

}

namespace rinchi {

void MdlRDfileWriter::write_reaction(const Reaction& reaction, std::ostream& output_stream)
{
	try {
		// Header.
		output_stream << MDL_RDFILE_IDENTIFIER << '\n';
		output_stream << MDL_RDTAG_DATM << ' ' << rdfile_timestamp() << '\n';

		// Reaction as embedded RXN file.
		output_stream << MDL_RDTAG_RFMT << '\n';

		MdlRxnfileWriter rxn_writer;
		rxn_writer.write_reaction(reaction, output_stream);

		// Output agents as molfiles, each one prefixed with two lines: An MDL_RDTAG_DTYPE data line and an MDL_RDLINE_MFMT line.
		InChIToStructureConverter sc;
		for (ReactionComponentList::const_iterator rc = reaction.agents().begin(); rc != reaction.agents().end(); rc++) {
			int agent_number = (int) (rc - reaction.agents().begin() + 1);
			// TODO: Output more reasonable data line ?
			output_stream << MDL_RDTAG_DTYPE << " RXN:AGENTS(1):MOLECULES(" << agent_number << "):MOLSTRUCTURE" << '\n';
			output_stream << MDL_RDLINE_MFMT << '\n';
			output_stream << "Agent" << agent_number << sc.to_molfile((*rc)->inchi_string(), (*rc)->inchi_auxinfo());
		}
	}
	catch (std::exception& e) {
		throw MdlRDfileWriterError (e.what());
	}
}

} // end of namespace
