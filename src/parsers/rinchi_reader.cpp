#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <memory>
#include <sstream>

#ifdef RINCHI_READER_DEBUG_TO_CONSOLE
	#include <iostream>
#endif

#include "rinchi_reader.h"

#include "rinchi_consts.h"
#include "rinchi_utils.h"
#include "rinchi_reaction.h"

#include "inchi_generator.h"

namespace rinchi {

	class RInChIReaderHelper {
    private:
        // Validate the direct-loaded InChI string and AuxInfo (if any).
        static void validate_reaction_component_inchi_strings(ReactionComponent* c)
        {
            InChIGenerator().validate_inchi(c->m_inchi_string);
            if (!c->m_inchi_auxinfo.empty()) {
                // Validate AuxInfo by attempting a structure rebuild.
                try {
                    InChIToStructureConverter().to_molfile(c->m_inchi_string, c->m_inchi_auxinfo);
                } catch (std::exception&) {
                    throw RInChIReaderError("Invalid AuxInfo '" + c->m_inchi_auxinfo + "' for a reaction component.");
                }
            }
        }

	public:
		static void create_components_from_inchigroup(const std::string& inchigroup, ReactionComponentList& components)
		{
			if (inchigroup.empty())
				return;

			size_t delim_pos = 0;
			size_t token_start = 0;
			std::string inchi_string;
			while (true) {
				delim_pos = inchigroup.find(DELIM_COMP, token_start);
				if (delim_pos != std::string::npos) {
					inchi_string = inchigroup.substr(token_start, delim_pos - token_start);
					token_start = delim_pos + DELIM_COMP.length();
				}
				else {
					inchi_string = inchigroup.substr(token_start);
				}

                trim_right(inchi_string, " ");
                trim_left(inchi_string, " ");
                if (inchi_string.find_first_of(" \n\t") != std::string::npos)
                    throw RInChIReaderError("Invalid trailing text in component InChI '" + inchi_string + "'.");

				inchi_string = INCHI_STD_HEADER + inchi_string;
				// Validate and cleanup InChI input.
				InChIGenerator().validate_inchi(inchi_string);

				std::unique_ptr<ReactionComponent> cmp (new ReactionComponent());
				components.push_back(cmp.get());
				ReactionComponent* tmp_cmp = cmp.release();
				tmp_cmp->m_inchi_string = inchi_string;

				if (delim_pos == std::string::npos)
					break;
			}
		}

		static void add_auxinfo_to_components(const std::string& auxinfogroup, ReactionComponentList& components, const std::string& group_name)
		{
			if (auxinfogroup.empty())
				return;

			int cmp_idx = 0;
			size_t delim_pos = 0;
			size_t token_start = 0;
			std::string auxinfo;
			while (true) {
				delim_pos = auxinfogroup.find(DELIM_COMP, token_start);
				if (delim_pos != std::string::npos) {
					auxinfo = auxinfogroup.substr(token_start, delim_pos - token_start);
					token_start = delim_pos + DELIM_COMP.length();
				}
				else {
					auxinfo = auxinfogroup.substr(token_start);
				}

				if (cmp_idx >= (int) components.size())
					throw RInChIReaderError ("RAuxInfo contains too many elements in the " + group_name + ".");
				ReactionComponent* cmp = components.at(cmp_idx);

				auxinfo = INCHI_AUXINFO_HEADER + auxinfo;
				// Validate AuxInfo by attempting a structure rebuild.
				try {
					InChIToStructureConverter().to_molfile(cmp->inchi_string(), auxinfo);
				} catch (std::exception&) {
					throw RInChIReaderError("Invalid AuxInfo '" + auxinfo + "' for a reaction component.");
				}

				cmp->m_inchi_auxinfo = auxinfo;
				cmp_idx++;

				if (delim_pos == std::string::npos)
					break;
			}
		}

        static void read_components(const std::string& inchi_lines, ReactionComponentList& components)
        {
            if (inchi_lines.empty())
                return;

            std::stringstream inchi_lines_stream (inchi_lines);
            std::string line;
            int line_no = 1;

            // A single trailing blank line is accepted.
            bool blank_line_detected = false;
            ReactionComponent* c = nullptr;
            while (inchi_lines_stream) {
                rinchi_getline(inchi_lines_stream, line);
                if (blank_line_detected && inchi_lines_stream)
                    throw RInChIReaderError("Line " + int2str(line_no) + ": Unexpected trailing data; expected an EOF after previous blank line.");
                // ' line.rfind("ABC", 0) == 0 ' is equivalent to ' line.starts_with("ABC") '.
                if (line.rfind("InChI=", 0) == 0) {
                    if (c == nullptr || !c->inchi_string().empty()) {
                        // Check previously added component, if any.
                        if (c != nullptr)
                            validate_reaction_component_inchi_strings(c);
                        // Add new component to reaction.
                        c = new ReactionComponent();
                        components.push_back(c);
                    }
                    c->m_inchi_string = line;
                }
                else if (line.rfind("AuxInfo=", 0) == 0) {
                    if (c->inchi_string().empty())
                        throw new RInChIReaderError ("Line " + int2str(line_no) + ": AuxInfo without preceeding InChI string.");
                    c->m_inchi_auxinfo = line;
                }
                else if (line.empty()) {
                    blank_line_detected = true;
                }
                else
                    throw RInChIReaderError("Line " + int2str(line_no) + ": Unexpected line data; expected an InChI or AuxInfo string.");

                ++line_no;
            }
            // Check last-added reaction component.
            if (c != nullptr)
                validate_reaction_component_inchi_strings(c);
        }

        static void add_inchis_to_reaction(const std::string& reactant_inchis, const std::string& product_inchis, const std::string& agent_inchis, Reaction& rxn)
        {
            read_components(reactant_inchis, rxn.m_reactants);
            read_components(product_inchis,  rxn.m_products);
            read_components(agent_inchis,    rxn.m_agents);

            rxn.m_is_cache_valid = false;
        }

    };
}

namespace rinchi {

void RInChIReader::split_into_reaction(const std::string& rinchi_string, const std::string& rinchi_auxinfo, Reaction& rxn, bool* is_reverse_direction)
{
	check<RInChIReaderError>(rxn.empty(), "Cannot put components of an RInChI string into a non-empty Reaction.");
	check<RInChIReaderError>(
		rinchi_string.length() >= RINCHI_STD_HEADER.length() &&
		rinchi_string.substr(0, RINCHI_STD_HEADER.length()) == RINCHI_STD_HEADER
		, "Invalid or incompatible RInChI header.");

	/**
	  Split RInChI string into groups. **/
	std::string first_group;
	std::string second_group;
	std::string third_group;

	std::string* last_group = NULL;

	size_t found_delim_pos = rinchi_string.find(DELIM_GROUP);
	// First group may be all there is.
	if (found_delim_pos == std::string::npos) {
		first_group = rinchi_string.substr(RINCHI_STD_HEADER.length());
		last_group = &first_group;
	}
	else {
		first_group = rinchi_string.substr(RINCHI_STD_HEADER.length(), found_delim_pos - RINCHI_STD_HEADER.length());
		size_t token_start = found_delim_pos + DELIM_GROUP.length();
		// Look for third optional group.
		found_delim_pos = rinchi_string.find(DELIM_GROUP, token_start);
		if (found_delim_pos != std::string::npos) {
			second_group = rinchi_string.substr(token_start, found_delim_pos - token_start);
			third_group = rinchi_string.substr(found_delim_pos + DELIM_GROUP.length());
			last_group = &third_group;
		}
		else {
			// Second group only - no third group.
			second_group = rinchi_string.substr(token_start);
			last_group = &second_group;
		}
	}

	// Dig out direction flag and No-Structure counts, if defined.
	std::string no_struct_data;
	std::string direction_data;
	for (int i = 0; i < 2; i++) {
		size_t layer_pos = last_group->find_last_of(DELIM_LAYER);
		if (layer_pos != std::string::npos && layer_pos < last_group->length() - 1) {
			std::string last_tag = last_group->substr(layer_pos, 2);
			if (last_tag == NOSTRUCT_TAG) {
				if (!no_struct_data.empty())
					throw RInChIReaderError ("Duplicate No-Structure tag in RInChI input string.");
				no_struct_data = last_group->substr(layer_pos);
				last_group->erase(layer_pos);
			}
			else if (last_tag == DIRECTION_TAG) {
				if (!direction_data.empty())
					throw RInChIReaderError ("Duplicate direction tag in RInChI input string.");
				direction_data = last_group->substr(layer_pos);
				last_group->erase(layer_pos);
			}
			else
				break;
		}
	}

	char direction_flag = DIRECTION_FORWARD;
	if (!direction_data.empty())
		direction_flag = direction_data.at(2);

#ifdef RINCHI_READER_DEBUG_TO_CONSOLE
	std::cout << "First group:" << std::endl;
	std::cout << first_group << std::endl;
	std::cout << "Second group:" << std::endl;
	std::cout << second_group << std::endl;
	std::cout << "Third group:" << std::endl;
	std::cout << third_group << std::endl;
	if (direction_flag != 0)
		std::cout << "Direction flag is '" << direction_flag << "'" << std::endl;
	else
		std::cout << "No direction flag." << std::endl;
	if (no_struct_data.empty())
		std::cout << "No No-Structures." << std::endl;
	else
		std::cout << "No-Structure counts: " << no_struct_data << std::endl;
#endif

	/**
	  Split groups into component InChI strings. **/
	if (direction_flag == DIRECTION_FORWARD || direction_flag == DIRECTION_EQUILIBRIUM) {
		RInChIReaderHelper::create_components_from_inchigroup(first_group, rxn.m_reactants);
		RInChIReaderHelper::create_components_from_inchigroup(second_group, rxn.m_products);
	}
	else if (direction_flag == DIRECTION_REVERSE) {
		RInChIReaderHelper::create_components_from_inchigroup(first_group, rxn.m_products);
		RInChIReaderHelper::create_components_from_inchigroup(second_group, rxn.m_reactants);
	}
	RInChIReaderHelper::create_components_from_inchigroup(third_group, rxn.m_agents);

	// Set reaction direction.
	if (direction_flag == DIRECTION_EQUILIBRIUM)
		rxn.m_directionality = rdEquilibrium;
	else
		rxn.m_directionality = rdDirectional;

	if (is_reverse_direction != NULL)
		*is_reverse_direction = (direction_flag == DIRECTION_REVERSE);
	rxn.m_reverse_output = (direction_flag == DIRECTION_REVERSE);

	/**
	  Split RAuxInfo string, if any, into groups. **/
	first_group.clear();
	second_group.clear();
	third_group.clear();
	if (!rinchi_auxinfo.empty()) {
		// Find first group. It may be alone.
		size_t found_delim_pos = rinchi_auxinfo.find(DELIM_GROUP);
		if (found_delim_pos == std::string::npos) {
			first_group = rinchi_auxinfo.substr(RINCHI_AUXINFO_HEADER.length());
		}
		else {
			first_group = rinchi_auxinfo.substr(
				RINCHI_AUXINFO_HEADER.length(), found_delim_pos - RINCHI_AUXINFO_HEADER.length());
			// Find second and optionally third group.
			size_t token_start = found_delim_pos + DELIM_GROUP.length();
			found_delim_pos = rinchi_auxinfo.find(DELIM_GROUP, token_start);
			if (found_delim_pos != std::string::npos) {
				second_group = rinchi_auxinfo.substr(token_start, found_delim_pos - token_start);
				// Remainder is the third optional group.
				third_group = rinchi_auxinfo.substr(found_delim_pos + DELIM_GROUP.length());
			}
			else {
				// No third group.
				second_group = rinchi_auxinfo.substr(token_start);
			}
		}

		if (direction_flag == DIRECTION_FORWARD || direction_flag == DIRECTION_EQUILIBRIUM) {
			RInChIReaderHelper::add_auxinfo_to_components(first_group, rxn.m_reactants, "first group (reactants)");
			RInChIReaderHelper::add_auxinfo_to_components(second_group, rxn.m_products, "second group (products)");
		}
		else if (direction_flag == DIRECTION_REVERSE) {
			RInChIReaderHelper::add_auxinfo_to_components(first_group, rxn.m_products, "first group (products)");
			RInChIReaderHelper::add_auxinfo_to_components(second_group, rxn.m_reactants, "second group (reactants)");
		}
		RInChIReaderHelper::add_auxinfo_to_components(third_group, rxn.m_agents, "third group (agents)");
	}
	// If no RAuxInfo, set blank AuxInfo on all components.
	else {
		for (ReactionComponentList::const_iterator rc = rxn.reactants().begin(); rc != rxn.reactants().end(); rc++)
			(*rc)->m_inchi_auxinfo = "";
		for (ReactionComponentList::const_iterator rc = rxn.products().begin(); rc != rxn.products().end(); rc++)
			(*rc)->m_inchi_auxinfo = "";
		for (ReactionComponentList::const_iterator rc = rxn.agents().begin(); rc != rxn.agents().end(); rc++)
			(*rc)->m_inchi_auxinfo = "";
	}

	if (!no_struct_data.empty()) {
		// Remove tag.
		no_struct_data.erase(0, 2);
		size_t first_delim = no_struct_data.find_first_of(NOSTRUCT_DELIM);
		size_t last_delim  = no_struct_data.find_last_of(NOSTRUCT_DELIM);
		if (first_delim == std::string::npos || last_delim == std::string::npos)
			throw RInChIReaderError ("Invalid No-Structure count format in '" + no_struct_data + "'.");

		rxn.m_nostruct_counts[0] = str2int(no_struct_data.substr(0, first_delim));
		rxn.m_nostruct_counts[1] = str2int(no_struct_data.substr(first_delim + 1, last_delim - first_delim - 1));
		rxn.m_nostruct_counts[2] = str2int(no_struct_data.substr(last_delim + 1));

		if (rxn.m_reverse_output)
			std::swap(rxn.m_nostruct_counts[0], rxn.m_nostruct_counts[1]);

		// Add No-Structures to reaction component lists.
		ReactionComponentList* rc_lists [RINCHI_NUM_GROUPS];

		rc_lists[0] = &rxn.m_reactants;
		rc_lists[1] = &rxn.m_products;
		rc_lists[2] = &rxn.m_agents;

		std::string nostruct_auxinfo = NOSTRUCT_AUXINFO;
		if (rinchi_auxinfo.empty())
			nostruct_auxinfo.clear();

		for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
			for (int k = 0; k < rxn.m_nostruct_counts[i]; k++) {
				std::unique_ptr<ReactionComponent> cmp (new ReactionComponent());
				rc_lists[i]->push_back(cmp.get());
				ReactionComponent* tmp_cmp = cmp.release();
				tmp_cmp->m_inchi_string  = NOSTRUCT_INCHI;
				tmp_cmp->m_inchi_auxinfo = nostruct_auxinfo;
			}
		}
	}
	else
		for (int i = 0; i < RINCHI_NUM_GROUPS; i++)
			rxn.m_nostruct_counts[i] = 0;

	rxn.m_is_cache_valid = false;

#ifdef RINCHI_READER_DEBUG_TO_CONSOLE
	std::cout << "RInChI reader: Reactants (" << rxn.reactants().size() << "): " << std::endl;
for (ReactionComponentList::const_iterator rc = rxn.reactants().begin(); rc != rxn.reactants().end(); rc++)
	std::cout << (*rc)->inchi_string() << std::endl << (*rc)->inchi_auxinfo() << std::endl;
std::cout << "RInChI reader: Products (" << rxn.products().size() << "): " << std::endl;
for (ReactionComponentList::const_iterator rc = rxn.products().begin(); rc != rxn.products().end(); rc++)
	std::cout << (*rc)->inchi_string() << std::endl << (*rc)->inchi_auxinfo() << std::endl;
std::cout << "RInChI reader: Agents (" << rxn.agents().size() << "): " << std::endl;
for (ReactionComponentList::const_iterator rc = rxn.agents().begin(); rc != rxn.agents().end(); rc++)
	std::cout << (*rc)->inchi_string() << std::endl << (*rc)->inchi_auxinfo() << std::endl;
std::cout << "RInChI reader: No-Structure counts" << std::endl;
for (int i = 0; i < RINCHI_NUM_GROUPS; i++)
	std::cout << rxn.m_nostruct_counts[i] << "  ";
std::cout << std::endl;
#endif
/*
#ifdef RINCHI_READER_DEBUG_TO_CONSOLE
	InChIToStructureConverter sc;
	std::cout << "****************************************************" << std::endl;
	std::cout << "RInChI reader: Reactants (" << rxn.reactants().size() << "): " << std::endl;
for (ReactionComponentList::const_iterator rc = rxn.reactants().begin(); rc != rxn.reactants().end(); rc++) {
	std::cout << "Reactant" << (int) (rc - rxn.reactants().begin() + 1) << sc.to_molfile((*rc)->inchi_string(), (*rc)->inchi_auxinfo()) << std::endl;
	std::cout << "===========================" << std::endl;
}
std::cout << "RInChI reader: Products (" << rxn.products().size() << "): " << std::endl;
for (ReactionComponentList::const_iterator rc = rxn.products().begin(); rc != rxn.products().end(); rc++) {
	std::cout << "Product" << (int) (rc - rxn.products().begin() + 1) << sc.to_molfile((*rc)->inchi_string(), (*rc)->inchi_auxinfo()) << std::endl;
	std::cout << "===========================" << std::endl;
}
std::cout << "RInChI reader: Agents (" << rxn.agents().size() << "): " << std::endl;
for (ReactionComponentList::const_iterator rc = rxn.agents().begin(); rc != rxn.agents().end(); rc++) {
	std::cout << "Agent" << (int) (rc - rxn.agents().begin() + 1) << sc.to_molfile((*rc)->inchi_string(), (*rc)->inchi_auxinfo()) << std::endl;
	std::cout << "===========================" << std::endl;
}
#endif
*/
}

namespace {


}

void RInChIReader::add_inchis_to_reaction(const std::string& reactant_inchis, const std::string& product_inchis, const std::string& agent_inchis, Reaction& rxn)
{
    RInChIReaderHelper::add_inchis_to_reaction(reactant_inchis, product_inchis, agent_inchis, rxn);
}

} // end of namespace
