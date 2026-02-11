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
#include <vector>
#include <iomanip>

#include <rinchi_utils.h>
#include <rinchi_consts.h>

#include <mdl_rxnfile.h>
#include <mdl_rxnfile_writer.h>
#include <inchi_generator.h>

using namespace std;

namespace rinchi {

void MdlRxnfileWriter::write_reaction(const Reaction& reaction, std::ostream& output_stream, bool do_write_agents)
{
	try {
		output_stream << MDL_TAG_RXN_BEGIN << '\n';
		// Lines two and three: Reaction name (blank) and producer identity..
		output_stream << '\n' << "      " << RINCHI_IDENTIFIER << '\n';
		// Line four: Comments.
		if (reaction.directionality() == rdEquilibrium)
			output_stream << "NOTE: Reaction is an equilibrium reaction.";
		output_stream << '\n';
		// Count line: #Reactants #Products.
		output_stream << setw(3) << reaction.reactants().size() << setw(3) << reaction.products().size();
        if ( do_write_agents && (!reaction.agents().empty()) )
            output_stream << setw(3) << reaction.agents().size();
        output_stream << '\n';
		// Reactants and products as molfiles, each one prefixed with an MDL_TAG_RXN_COMPONENT_START line.
		InChIToStructureConverter sc;
		for (ReactionComponentList::const_iterator rc = reaction.reactants().begin(); rc != reaction.reactants().end(); rc++) {
			output_stream << MDL_TAG_RXN_COMPONENT_START << '\n';
			output_stream << "Reactant" << (int) (rc - reaction.reactants().begin() + 1) << sc.to_molfile((*rc)->inchi_string(), (*rc)->inchi_auxinfo());
		}
		for (ReactionComponentList::const_iterator rc = reaction.products().begin(); rc != reaction.products().end(); rc++) {
			output_stream << MDL_TAG_RXN_COMPONENT_START << '\n';
			output_stream << "Product" << (int) (rc - reaction.products().begin() + 1) << sc.to_molfile((*rc)->inchi_string(), (*rc)->inchi_auxinfo());
		}
        // ... and agents, if any.
        if (do_write_agents) {
            for (ReactionComponentList::const_iterator rc = reaction.agents().begin(); rc != reaction.agents().end(); rc++) {
                output_stream << MDL_TAG_RXN_COMPONENT_START << '\n';
                output_stream << "Agent" << (int) (rc - reaction.agents().begin() + 1) << sc.to_molfile((*rc)->inchi_string(), (*rc)->inchi_auxinfo());
            }
        }
	}
	catch (exception& e) {
		throw MdlRxnfileWriterError (e.what());
	}
}

} // end of namespace
