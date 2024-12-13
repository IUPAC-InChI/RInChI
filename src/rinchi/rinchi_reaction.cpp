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

#include <sstream>
#include <algorithm>
#include <memory>
#include <map>

#include "inchi_api_intf.h"

#include "rinchi_consts.h"
#include "rinchi_reaction.h"
#include "inchi_generator.h"
#include "rinchi_utils.h"
#include "rinchi_hashing.h"

namespace rinchi {

#ifdef IN_RINCHI_TEST_SUITE
std::string __latest_hash_cleartext;

std::string latest_hash_cleartext()
{
	return __latest_hash_cleartext;
}
#endif

/**
  ReactionComponent
**/

ReactionComponent::ReactionComponent()
{
	clear_inchi_input();
}

ReactionComponent::~ReactionComponent()
{
	if (m_inchi_input.atom != NULL)
		delete [] m_inchi_input.atom;
}

void ReactionComponent::clear_inchi_input()
{
	m_inchi_input.atom      = NULL;
	m_inchi_input.stereo0D  = NULL;
	m_inchi_input.szOptions = NULL;
	m_inchi_input.num_atoms    = 0;
	m_inchi_input.num_stereo0D = 0;
}

void ReactionComponent::initialize(int atom_count, bool is_chiral)
{
	if (m_inchi_input.atom != NULL)
		delete [] m_inchi_input.atom;

	clear_inchi_input();
	m_inchi_input.num_atoms = atom_count;
	m_inchi_input.atom = new inchi_Atom[atom_count];

	if (is_chiral)
		m_inchi_input.szOptions = (char *) INCHI_INPUT_OPTION_CHIRAL_FLAG_ON;
	else
		m_inchi_input.szOptions = (char *) INCHI_INPUT_OPTION_CHIRAL_FLAG_OFF;
}

inchi_Input& ReactionComponent::inchi_input()
{
	return m_inchi_input;
}

bool ReactionComponent::is_no_structure() const
{
	// If component was instantiated directly from RInChI, then
	// 'm_inchi_input' has not had atoms allocated.
	if (m_inchi_input.atom == NULL)
		return m_inchi_string == INCHI_STD_HEADER + "/";
	else
		return m_inchi_input.num_atoms == 0;
}

void ReactionComponent::reset_inchi_string()
{
	m_inchi_string.clear();
	m_inchi_auxinfo.clear();
	m_inchi_key.clear();
}

const std::string& ReactionComponent::inchi_string()
{
	if (!m_inchi_string.empty())
		return m_inchi_string;

	InChIGenerator().calculate_inchi(m_inchi_input, m_inchi_string, m_inchi_auxinfo);
	return m_inchi_string;
}

const std::string& ReactionComponent::inchi_auxinfo()
{
	if (!m_inchi_string.empty())
		return m_inchi_auxinfo;

	InChIGenerator().calculate_inchi(m_inchi_input, m_inchi_string, m_inchi_auxinfo);
	return m_inchi_auxinfo;
}

const std::string& ReactionComponent::inchi_key()
{
	if (!m_inchi_key.empty())
		return m_inchi_key;

	m_inchi_key = InChIGenerator().inchi_string2key(inchi_string());
	return m_inchi_key;
}

/**
  Reaction
**/

namespace {

	ReactionComponent* add_new_component_to_list(ReactionComponentList& list)
	{
		std::unique_ptr<ReactionComponent> cmp (new ReactionComponent());
		list.push_back(cmp.get());
		return cmp.release();
	}

	void clear_component_list(ReactionComponentList& list)
	{
		for (ReactionComponentList::iterator rc = list.begin(); rc != list.end(); rc++)
			delete *rc;
		list.clear();
	}

}

Reaction::Reaction()
	: m_directionality(rdDirectional), m_is_cache_valid(false), m_reverse_output(false)
{
	for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
		m_output_order[i]    = -1;
		m_nostruct_counts[i] = 0;
	}
}

Reaction::~Reaction()
{
	clear_component_list(m_reactants);
	clear_component_list(m_products);
	clear_component_list(m_agents);
}

bool Reaction::empty() const
{
	return (
		m_reactants.empty() && m_products.empty() && m_agents.empty() &&
		m_nostruct_counts[0] == 0 && m_nostruct_counts[1] == 0 && m_nostruct_counts[2] == 0
	);
}

ReactionComponent* Reaction::add_reactant()
{
	m_is_cache_valid = false;
	return add_new_component_to_list(m_reactants);
}

ReactionComponent* Reaction::add_product()
{
	m_is_cache_valid = false;
	return add_new_component_to_list(m_products);
}

ReactionComponent* Reaction::add_agent()
{
	m_is_cache_valid = false;
	return add_new_component_to_list(m_agents);
}

void Reaction::delete_agent(ReactionComponent* agent)
{
	ReactionComponentList::iterator agent_to_delete = std::find(m_agents.begin(), m_agents.end(), agent);
	if (agent_to_delete == m_agents.end())
		return;

	m_is_cache_valid = false;
	m_agents.erase(agent_to_delete);
	delete agent;
}

ReactionDirectionality Reaction::directionality() const
{
	return m_directionality;
}

void Reaction::set_directionality(ReactionDirectionality dir)
{
	m_directionality = dir;
}

int Reaction::nostructure_count(int group_idx)
{
	update_cache();
	return m_nostruct_counts[group_idx];
}

int Reaction::num_output_groups(bool consider_nostruct_counts)
{
	int result = 1;
	for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
		if (!m_rinchi_groups[m_output_order[i]].empty())
			result = i + 1;
		if (consider_nostruct_counts && m_nostruct_counts[m_output_order[i]] != 0)
			result = i + 1;
	}
	return result;
}

namespace {

	bool ReactionComponent_inchi_string_less_than(ReactionComponent* rc1, ReactionComponent* rc2)
	{
		return rc1->inchi_string() < rc2->inchi_string();
	}

	void output_direction_flag(ReactionDirectionality direction, bool reverse_output, std::ostream& output)
	{
		switch (direction)
		{
		case rdDirectional:
			if (reverse_output)
				output << DIRECTION_TAG << DIRECTION_REVERSE;
			else
				output << DIRECTION_TAG << DIRECTION_FORWARD;
			break;
		case rdEquilibrium:
			output <<  DIRECTION_TAG << DIRECTION_EQUILIBRIUM;
			break;
		default:
			throw RInChIError("Internal bug: Unexpected reaction direction value (" + int2str(direction) + ".");
		}
	}

}

void Reaction::update_cache()
{
	if (m_is_cache_valid)
		return;

	// Initialize output cache.
	m_reverse_output = false;
	{
		ReactionComponentList* rc_lists [RINCHI_NUM_GROUPS];

		rc_lists[0] = &m_reactants;
		rc_lists[1] = &m_products;
		rc_lists[2] = &m_agents;

		for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
			m_output_order[i] = i;
			m_ordered_rcs[i].clear();
			m_rinchi_groups[i].clear();
			m_rauxinfo_groups[i].clear();
			m_nostruct_counts[i] = 0;
			// 'm_ordered_rcs' will not contain any No-Structures.
			for (ReactionComponentList::const_iterator rc = rc_lists[i]->begin(); rc != rc_lists[i]->end(); rc++) {
				if ((*rc)->is_no_structure())
					m_nostruct_counts[i]++;
				else
					m_ordered_rcs[i].push_back(*rc);
			}
		}
	}

	/** From Guenthers paper: 
	    "For example, a RInChI of a reaction without a known product and no information about 
		 solvents/reagents would only show the first group."

		From Hans's (Infochem) tests it is clear that we should allow not only empty product
		lists but also empty reactant lists. But at least one of them must be present.

		Jan 2016-02-26: After all, it does make sense to allow an empty reaction, just like
		InChI can handle No-Structure-s, we should be allowed to have No-Reactions in RInChI.
		Therefore, the next check has been commented away.
	**/
	//                            +- Reactant nostruct count                         +- Product nostruct count
	// if (m_reactants.empty() && m_nostruct_counts[0] == 0 && m_products.empty() && m_nostruct_counts[1] == 0)
	//		throw RInChIError("Reaction has neither reactants nor products.");

	// Sort and concatenate InChIs into groups.
	for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
		std::sort(m_ordered_rcs[i].begin(), m_ordered_rcs[i].end(), ReactionComponent_inchi_string_less_than);

		std::ostringstream rinchi_stream;
		std::ostringstream rauxinfo_stream;
		for (ReactionComponentList::const_iterator rc = m_ordered_rcs[i].begin(); rc != m_ordered_rcs[i].end(); rc++) {
			if (rc != m_ordered_rcs[i].begin()) {
				rinchi_stream << DELIM_COMP;
				rauxinfo_stream << DELIM_COMP;
			}
			// InChI string minus the leading "InChI=1S/".
			rinchi_stream << (*rc)->inchi_string().substr(INCHI_STD_HEADER.length());
			// AuxInfo minus the leading "AuxInfo=".
			const std::string& auxinfo = (*rc)->inchi_auxinfo();
			if (auxinfo.empty())
				rauxinfo_stream << DELIM_LAYER;
			else
				rauxinfo_stream << auxinfo.substr(INCHI_AUXINFO_HEADER.length());
		}

		m_rinchi_groups[i] = rinchi_stream.str();
		m_rauxinfo_groups[i] = rauxinfo_stream.str();
	}

	// Is the output reversed ?        +- products          +- reactants
	m_reverse_output = m_rinchi_groups[1] < m_rinchi_groups[0];

	if (m_reverse_output)
		std::swap(m_output_order[0], m_output_order[1]);

	m_is_cache_valid = true;
}

const std::string Reaction::rinchi_string()
{
	update_cache();

	std::ostringstream result;
	result << RINCHI_STD_HEADER;

	bool has_nostructures = false;
	for (int i = 0; i < RINCHI_NUM_GROUPS; i++)
		has_nostructures |= (m_nostruct_counts[i] > 0);

	int group_count = num_output_groups(false);
	for (int i = 0; i < group_count; i++) {
		result << m_rinchi_groups[m_output_order[i]];
		if (i < group_count - 1)
			result << DELIM_GROUP;
	}

	output_direction_flag(m_directionality, m_reverse_output, result);

	if (has_nostructures) {
		result << NOSTRUCT_TAG;
		for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
			result << m_nostruct_counts[m_output_order[i]];
			if (i < RINCHI_NUM_GROUPS - 1)
				result << NOSTRUCT_DELIM;
		}
	}

	return result.str();;
}

const std::string Reaction::rinchi_auxinfo()
{
	update_cache();

	std::ostringstream result;
	result << RINCHI_AUXINFO_HEADER;

	int group_count = num_output_groups(false);
	for (int i = 0; i < group_count; i++) {
		result << m_rauxinfo_groups[m_output_order[i]];
		if (i < group_count - 1)
			result << DELIM_GROUP;
	}

	return result.str();
}

namespace {

	void output_inchikey_group(ReactionComponentList& component_list, int nostruct_count, std::ostream& output)
	{
		for (ReactionComponentList::const_iterator rc = component_list.begin(); rc != component_list.end(); rc++) {
			if (rc != component_list.begin())
				output << KEY_DELIM_COMP;
			const std::string& key = (*rc)->inchi_key();
			// Concatenate standard InChI keys of components.
			output << key;
		}

		for (int i = 1; i <= nostruct_count; i++) {
			if (i == 1) {
				if (component_list.size() != 0)
					output << KEY_DELIM_COMP;
			}
			else
				output << KEY_DELIM_COMP;

			output << NOSTRUCT_RINCHI_LONGKEY;
		}
	}

	/**
	  Encode a No-Structure count as a char. 0..25 => Z,A..Y. Counts >= 25 are encoded as Y.

	  NOTE: Must be recoded for systems using a non-ASCII based charset, e.g. EBCDIC. **/
	char nostruct_count2char(int nostruct_count)
	{
		if (nostruct_count == 0)
			return 'Z';

		if (nostruct_count > 24)
			return 'Y';
		else
			return 'A' + nostruct_count - 1;
	}

	void output_nostruct_count_chars(int (& nostruct_counts)[RINCHI_NUM_GROUPS], int (& output_order)[RINCHI_NUM_GROUPS], std::ostream& result)
	{
		for (int i = 0; i < RINCHI_NUM_GROUPS; i++)
			result << nostruct_count2char(nostruct_counts[output_order[i]]);
	}

	void output_direction_code(ReactionDirectionality direction, bool reverse_output, std::ostream& output)
	{
		switch (direction)
		{
		case rdDirectional:
			if (reverse_output)
				output << "B";
			else
				output << "F";
			break;
		case rdEquilibrium:
			output << "E";
			break;
		default:
			throw RInChIError("Internal bug: Unexpected reaction direction value (" + int2str(direction) + ".");
		}
	}

}

const std::string Reaction::rinchi_long_key()
{
	update_cache();

	std::ostringstream result;
	result << RINCHI_LONG_KEY_HEADER;
	/** Cite paper:
	  "the first block (group of letters) consists of three letters of which the 
	   first one represents the version identifier and the next two identify the
	   constituent InChIKeys."
	
	  In this version we only allow standard InChIs, so the RInChI key is always 1S.
	**/
	result << RINCHI_KEY_VERSION_ID_HEADER << KEY_DELIM_BLOCK;
	/** Cite paper:
	  "The second block, which is separated from the first block with a hypen, is 
	   a hashed representation of any additional reaction layers taken as a whole."

	  "... the first letter of block 2 is F, B, E or U representing
	   forward, backward, equilibrium, or unspecified reactions, respectively."

	  Since we don't handle unknown reaction directionality at present, only
	  F, B, or E will be output by this code.
	**/
	output_direction_code(m_directionality, m_reverse_output, result);
	// Output hash block for "additional reaction layers" - reserved for future use.
	result << HASH_12_EMPTY_STRING.substr(0, 4) << KEY_DELIM_BLOCK;

	std::string result_str = result.str();

	int group_count = num_output_groups(true);
	for (int i = 0; i < group_count; i++) {
		output_inchikey_group(m_ordered_rcs[m_output_order[i]], m_nostruct_counts[m_output_order[i]], result);
		if (i < group_count - 1)
			result << KEY_DELIM_GROUP;
	}

	// Strip trailing KEY_DELIM_BLOCK if no key groups were output.
	if (result_str == result.str())
		return result_str.substr(0, result_str.length() - KEY_DELIM_BLOCK.length());
	else
		return result.str();
}

namespace {

	/**
	  Encode a proton count as a char compliant with InChI key rules.

	  NOTE: Must be recoded for systems using a non-ASCII based charset, e.g. EBCDIC. **/
	char proton_count2char(int proton_count)
	{
		if (proton_count > 12 || proton_count < -12)
			return 'A';
		else
			return 'N' + proton_count;
	}

	/**
	  Holds the major and minor layers of a collection of InChI strings.
	  When an InChI string is appended, it is automatically split
	  into major and minor layers and the layers are appended to 'majors'
	  and 'minors'. Layers belonging to separate InChI strings are
	  delimited by a component delimiter DELIM_COMP.
	  Protons are aggregated into a single overall protonation count.
	**/
	struct InChILayers {
		std::string majors;
		std::string minors;
		int proton_count;

		InChILayers(const ReactionComponentList& rc_list)
		{
			proton_count = 0;
			append_components(rc_list);
		}

		InChILayers()
		{
			proton_count = 0;
		}

		void append(const std::string& inchi_string)
		{
			if (inchi_string.empty())
				return;

			size_t delim_pos = inchi_string.find(DELIM_LAYER);
			size_t token_start = delim_pos + DELIM_LAYER.length();

			if (delim_pos != 8) throw RInChIError("Invalid InChI string - no layers.");
			if (inchi_string.at(delim_pos - 1) != 'S') throw RInChIError("Only standard InChIs are supported.");
			if (inchi_string.at(delim_pos - 2) != '1') throw RInChIError("Only InChI version 1 supported.");

			bool is_first_layer = true;
			bool is_major_layer = true;
			std::string layer;
			std::string major_layers;
			std::string minor_layers;
			while (true) {
				delim_pos = inchi_string.find(DELIM_LAYER, token_start);
				if (delim_pos != std::string::npos) {
					layer = inchi_string.substr(token_start - 1, delim_pos - token_start + 1);
					token_start = delim_pos + DELIM_LAYER.length();
				}
				else {
					layer = inchi_string.substr(token_start - 1);
				}

				// First layer is the formula - it is always present.
				if (is_first_layer)
					major_layers += layer;
				// Check that layer is at least two chars. It may be a single '/' in the case
				// of the empty InChI string "InChI=1S//". Empty layers will be ignored.
				else if (layer.length() >= 2) {
					if (is_major_layer) {
						switch (layer.at(1)) {
							case 'c':
							case 'h':
							case 'q':
								major_layers += layer;
								break;
							case 'p':
								proton_count += str2int(layer.substr(2));
								break;
							default:
								minor_layers += layer;
								is_major_layer = false;
						}
					}
					else
						minor_layers += layer;
				}

				is_first_layer = false;
				if (delim_pos == std::string::npos)
					break;
			}

			// Remove leading slash from major and minor layer unless it is empty.
			if (!major_layers.empty())
				major_layers.erase(0, 1);
			if (major_layers.empty())
				major_layers = DELIM_LAYER;

			if (!minor_layers.empty())
				minor_layers.erase(0, 1);

			if (!majors.empty())
				majors += DELIM_COMP;
			majors += major_layers;

			if (!minors.empty())
				minors += DELIM_COMP;
			minors += minor_layers;
		}

		void append_components(const ReactionComponentList& rc_list)
		{
			for (ReactionComponentList::const_iterator rc = rc_list.begin(); rc != rc_list.end(); rc++) {
				if ((*rc)->is_no_structure())
					continue;
				append((*rc)->inchi_string());
			}
		}

		// Major hash = 10-char hash of major layers.
		std::string major_hash()
		{
			return hash10char(majors);
		}

		// Major hash extended = 17-char hash of major layers.
		std::string major_hash_ext()
		{
			return hash17char(majors);
		}

		// Minor hash = Encoded protonation count (1 char) + 4-char hash of minor layers.
		std::string minor_hash()
		{
			return std::string(1, proton_count2char(proton_count)) + hash04char(minors);
		}

		// Minor hash extended = Encoded protonation count (1 char) + 12-char hash of minor layers.
		std::string minor_hash_ext()
		{
			return std::string(1, proton_count2char(proton_count)) + hash12char(minors);
		}

		static std::string empty_major_hash()
		{
			return HASH_10_EMPTY_STRING;
		}

		static std::string empty_minor_hash()
		{
			return std::string(1, proton_count2char(0)) + HASH_04_EMPTY_STRING;
		}
	};
}

const std::string Reaction::rinchi_short_key()
{
	update_cache();
#ifdef IN_RINCHI_TEST_SUITE
	__latest_hash_cleartext = "";
#endif

	std::ostringstream result;
	result << RINCHI_SHORT_KEY_HEADER;

	result << RINCHI_KEY_VERSION_ID_HEADER << KEY_DELIM_BLOCK;
	output_direction_code(m_directionality, m_reverse_output, result);
	// Output hash block for "additional reaction layers" - reserved for future use.
	result << HASH_04_EMPTY_STRING;

	std::string all_major_hash = "";
	std::string all_minor_hash = "";
	// Output major/minor hash of component groups.
	for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
		InChILayers layers (m_ordered_rcs[m_output_order[i]]);
		all_major_hash += KEY_DELIM_BLOCK + layers.major_hash();
		all_minor_hash += KEY_DELIM_BLOCK + layers.minor_hash();
#ifdef IN_RINCHI_TEST_SUITE
		__latest_hash_cleartext += layers.majors + "\n" + layers.minors + "\n";
#endif
	}
	result << all_major_hash << all_minor_hash;

	result << KEY_DELIM_BLOCK;
	output_nostruct_count_chars(m_nostruct_counts, m_output_order, result);

	return result.str();
}

const std::string Reaction::rinchi_web_key()
{
	update_cache();
#ifdef IN_RINCHI_TEST_SUITE
	__latest_hash_cleartext = "";
#endif

	// Create a unique list of all component InChIs.
	typedef std::map<std::string, int> UniqueStringList;
	UniqueStringList unique_inchis;

	ReactionComponentList* all_comp_lists[RINCHI_NUM_GROUPS];
	all_comp_lists[0] = &m_reactants;
	all_comp_lists[1] = &m_products;
	all_comp_lists[2] = &m_agents;

	for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
		for (ReactionComponentList::const_iterator rc = all_comp_lists[i]->begin(); rc != all_comp_lists[i]->end(); rc++)
			unique_inchis.insert( make_pair((**rc).inchi_string(), 1) );
	}

	// Sort unique InChIs. A map will keep items sorted, but that is an implementation detail
	// that I do not dare to rely on. So push items to a standard vector and sort that.
	typedef std::vector<std::string> StringList;
	StringList sorted_inchis;

	for (UniqueStringList::const_iterator i = unique_inchis.begin(); i != unique_inchis.end(); i++)
		sorted_inchis.push_back(i->first);

	std::sort(sorted_inchis.begin(), sorted_inchis.end());

	// Output major/minor hash of combined unique InChI strings.
	std::ostringstream result;
	InChILayers layers;
	for (StringList::const_iterator s = sorted_inchis.begin(); s != sorted_inchis.end(); s++)
		layers.append(*s);

#ifdef IN_RINCHI_TEST_SUITE
	__latest_hash_cleartext += layers.majors + "\n" + layers.minors + "\n";
#endif
	// Put "SA" version identifier at end without hyphen for less confusing search engine indexing.
	// A web search engine may split a query into tokens, considering the hyphen a token delimiter.
	// Thus "-SA" would become an additional "SA" query term which could cause very confusing results.
	result << RINCHI_WEB_KEY_HEADER << layers.major_hash_ext() << KEY_DELIM_BLOCK << layers.minor_hash_ext() << "SA";
	return result.str();
}

} // End of namespace.
