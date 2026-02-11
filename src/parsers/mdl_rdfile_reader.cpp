#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <fstream>
#include <sstream>
#ifdef RINCHI_RDFILE_READER_DEBUG_TO_CONSOLE
	#include <iostream>
#endif

#include <mdl_rdfile.h>
#include <mdl_rdfile_reader.h>

#include <mdl_rxnfile_reader.h>
#include <mdl_molfile_reader.h>

using namespace std;

namespace rinchi {

namespace {

	// Prefix keywords with ":" if not already there.
	void prepare_keywords(StringVector& sv)
	{
		for (StringVector::iterator s = sv.begin(); s != sv.end(); s++) {
			if (!s->empty() && s->at(0) != ':')
				s->insert(s->begin(), ':');
		}
	}
}

MdlRDfileReader::MdlRDfileReader()
{
	// Set default set of non-agent keywords.
	m_non_agent_keywords.push_back("REACTANT");
	m_non_agent_keywords.push_back("PRODUCT");
	// Alternate version of "reactant".
	m_non_agent_keywords.push_back("EDUCT");
	// German alternates that are likely to appear.
	m_non_agent_keywords.push_back("REAKTANT");
	m_non_agent_keywords.push_back("PRODUKT");

	prepare_keywords(m_non_agent_keywords);
}

MdlRDfileReader::MdlRDfileReader(const StringVector& non_agent_keywords)
{
	m_non_agent_keywords.assign(non_agent_keywords.begin(), non_agent_keywords.end());
	prepare_keywords(m_non_agent_keywords);
}

void MdlRDfileReader::read_reaction_from_stream(std::istream& input_stream, Reaction& result, bool force_equilibrium)
{
	try {
		get_next_line(input_stream);
		/**
		  We do not require an RD file to start with MDL_RDTAG_RDFILE ("$RDFILE"). If an RD file
		  has multiple entries the $RDFILE line will only appear once as the first line of that
		  file. If you decide to split the file into one file per entry, only the file containing
		  the first entry will have the $RDFILE line.
		  Therefore we simply look for the first $RFMT line instead; this indicates start of a new
		  reaction entry.
		  NOTE: The $RFMT line must be within the first 1000 lines of the file, or the reader will
		        assume that the file is not an RD file.
		**/
		int skipped_line_count = -1;
		while (m_current_line.substr(0, MDL_RDTAG_RFMT.length()) != MDL_RDTAG_RFMT) {
			skipped_line_count++;
			if (skipped_line_count >= 1000)
				throw MdlRDfileReaderError("RD file section must contain an '" + MDL_RDTAG_RFMT + "' line.");
			get_next_line(input_stream);
		}

		// Boldly assuming that the $RXN tag directly follows the $RFMT line.
		MdlRxnfileReader rxn_reader;
		rxn_reader.read_reaction(input_stream, result, force_equilibrium, lines_read());
		m_line_number = rxn_reader.lines_read();

		#ifdef RINCHI_RDFILE_READER_DEBUG_TO_CONSOLE
		std::cout << "RD file: Reactants: " << std::endl;
		for (ReactionComponentList::const_iterator rc = result.reactants().begin(); rc != result.reactants().end(); rc++)
			std::cout << (*rc)->inchi_key() << std::endl;
		std::cout << "RD file: Products: " << std::endl;
		for (ReactionComponentList::const_iterator rc = result.products().begin(); rc != result.products().end(); rc++)
			std::cout << (*rc)->inchi_key() << std::endl;
		#endif

		// Read agents until we reach end of file, a new $RFMT tag, or the variation number (if any) changes.
		int variation_no = -1;
		std::string prev_line = m_current_line;
		while (input_stream) {
			get_next_line(input_stream);
			if (m_current_line.substr(0, MDL_RDTAG_RFMT.length()) == MDL_RDTAG_RFMT)
				break;
			if (m_current_line == MDL_RDLINE_MFMT) {
				// Check previous line and check if the structure looks like a
				// reactant or product (non-agent).
				std::string upper_prev_line = uppercase(prev_line);

				// Has variation number ?
				int next_var_no = -1;
				size_t first_paren_pos = upper_prev_line.find_first_of('(');
				size_t next_paren_pos  = upper_prev_line.find_first_of(')', first_paren_pos);
				if (first_paren_pos != std::string::npos && next_paren_pos != std::string::npos) {
					try {
						next_var_no = str2int(upper_prev_line, (int) first_paren_pos + 1, (int) next_paren_pos - (int) first_paren_pos - 1);
					} catch (std::exception) {
						// OK, ignore variation numbers we can't read.
					}
				}
				if (next_var_no != -1) {
					if (variation_no == -1)
						variation_no = next_var_no;
					else {
						// Stop reading agents when variation number changes.
						if (next_var_no != variation_no)
							break;
					}
				}

				// Is the molecule a reactant or product, a non-agent ?
				bool is_non_agent = false;
				for (StringVector::const_iterator s = m_non_agent_keywords.begin(); s != m_non_agent_keywords.end(); s++) {
					if (upper_prev_line.find(*s) != std::string::npos) {
						is_non_agent = true;
						break;
					}
				}

				#ifdef RINCHI_RDFILE_READER_DEBUG_TO_CONSOLE
				std::cout << std::endl << "RD file: Agent " << prev_line << std::endl;
				if (is_non_agent)
					std::cout << "  IS NON-AGENT!" << std::endl;
				#endif

				ReactionComponent* agent = result.add_agent();
				MdlMolfileReader mr;
				mr.read_molecule(input_stream, *agent, this->lines_read());
				m_line_number = mr.lines_read();

				// If molecule looks like a non-agent and it is already present as a reactant or a product, drop it again.
				bool drop_agent = false;
				if (is_non_agent) {
					const std::string& agent_key = agent->inchi_key();
					for (ReactionComponentList::const_iterator rc = result.reactants().begin(); rc != result.reactants().end(); rc++) {
						if ((*rc)->inchi_key() == agent_key) {
							drop_agent = true;
							break;
						}
					}
					if (!drop_agent) {
						for (ReactionComponentList::const_iterator rc = result.products().begin(); rc != result.products().end(); rc++) {
							if ((*rc)->inchi_key() == agent_key) {
								drop_agent = true;
								break;
							}
						}
					}
					#ifdef RINCHI_RDFILE_READER_DEBUG_TO_CONSOLE
					std::cout << "  " << agent->inchi_key() << std::endl;
					if (drop_agent && is_non_agent)
						std::cout << "  DROPPED!" << std::endl << std::endl;
					#endif
				}

				#ifdef RINCHI_RDFILE_READER_DEBUG_TO_CONSOLE
				// TODO: Turn this into a proper warning.
				if (is_non_agent && !drop_agent)
					std::cout << "  WARNING!!: Info line suggests that this molecule is a reactant or product, but it does not match any of the reactants or products loaded in the RXN section." << std::endl;
				#endif

				if (drop_agent) {
					result.delete_agent(agent);
					agent = NULL;
				}
			}
			prev_line = m_current_line;
		}
	}
	catch (exception& e) {
		throw_error(e);
	}

	if (force_equilibrium)
		result.set_directionality(rdEquilibrium);
}

void MdlRDfileReader::read_reaction(std::istream& input_stream, Reaction& result, bool force_equilibrium, int lines_already_read)
{
	m_input_name = "std::istream";
	m_line_number = lines_already_read;

	if (!input_stream)
		throw_error("Input RD file stream is not open");

	read_reaction_from_stream(input_stream, result, force_equilibrium);
}

void MdlRDfileReader::read_reaction(const std::string& file_name, Reaction& result, bool force_equilibrium, int lines_already_read)
{
	m_input_name  = file_name;
	m_line_number = lines_already_read;

	ifstream input_stream ( file_name.c_str() );
	if (!input_stream)
		throw_error("Can't open input RD file '" + file_name + "'");

	read_reaction_from_stream(input_stream, result, force_equilibrium);
}


} // end of namespace
