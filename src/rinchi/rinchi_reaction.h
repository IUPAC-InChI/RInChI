#ifndef IUPAC_RINCHI_REACTION_HEADER_GUARD
#define IUPAC_RINCHI_REACTION_HEADER_GUARD

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

#include <string>
#include <vector>
#include <stdexcept>

#include "inchi_api.h"

namespace rinchi {

class RInChIError: public std::runtime_error {
	public:
		explicit RInChIError(const std::string& what) : std::runtime_error(what) { }
};

/**
  Does reaction have a direction from reactants to products or is it
  an equilibrium reaction ? **/
typedef enum {rdDirectional, rdEquilibrium} ReactionDirectionality;

class ReactionComponent {
	// The RInChI string reader needs direct access to Reaction internals to load
	// data into the Reaction.
	friend class RInChIReader;
	friend class RInChIReaderHelper;
private:
	inchi_Input m_inchi_input;
	std::string m_inchi_string;
	std::string m_inchi_auxinfo;
	std::string m_inchi_key;
	// Clear the 'inchi_input' data structure.
	void clear_inchi_input();
public:
	ReactionComponent();
	~ReactionComponent();

	// Initializes the 'inchi_input' data structure.
	void initialize(int atom_count, bool is_chiral);
	inchi_Input& inchi_input();
	bool is_no_structure() const;

	// If you change 'inchi_input' after having called any of the functions
	// below, call reset_inchi_string() to ensure that InChI string, auxinfo
	// and key are re-calculated on next call to inchi_string(), inchi_auxinfo()
	// or inchi_key().
	void reset_inchi_string();
	const std::string& inchi_string();
	const std::string& inchi_auxinfo();
	const std::string& inchi_key();
};

typedef std::vector<ReactionComponent*> ReactionComponentList;

#define RINCHI_NUM_GROUPS 3

class Reaction {
	friend class RInChIReader;
	friend class RInChIReaderHelper;
private:
	ReactionComponentList m_reactants;
	ReactionComponentList m_products;
	ReactionComponentList m_agents;
	ReactionDirectionality m_directionality;

	// Flag that InChI-s have been sorted and groups calculated.
	bool m_is_cache_valid;
	/* When the RInChI string is calculated, this flag will be set to true if
	   the InChI group string of 'm_products' is lexically less than (lexically 
	   before) the InChI group string of 'm_reactants'. */
	bool m_reverse_output;
	/**
	 * I would have liked to use std::array for the cache arrays, but it is too hard to
	 * define a useful C-array declaration for the platforms that do not support std::array.
	 * See e.g. discussion of the failure to typedef a fixed-size C-array at
	 * http://stackoverflow.com/questions/4523497/typedef-fixed-length-array
	**/
	// {0, 1, 2} when forward output; {1, 0, 2} when reverse output.
	int m_output_order [RINCHI_NUM_GROUPS];
	// Copies of m_reactants, m_products, m_agents that are sorted by InChIs; 'rcs' = 'ReactionComponentS'.
	ReactionComponentList m_ordered_rcs [RINCHI_NUM_GROUPS];
	// Concatenated InChI's of ordered component lists.
	std::string m_rinchi_groups [RINCHI_NUM_GROUPS];
	std::string m_rauxinfo_groups [RINCHI_NUM_GROUPS];
	int m_nostruct_counts [RINCHI_NUM_GROUPS];

	// Number of RInChI groups to output, e.g. if no agents present only reactants and products
	// will be output (returns 2).
	// For RInChI and RAuxInfo output No-Structures are irrelevant for the group count, but they
	// are relevant for the output of e.g. the RInChI long key.
	int num_output_groups(bool consider_nostruct_counts);
	// Calculate above member variables (below 'm_has_rinchi') if 'm_has_rinchi' is false.
	void update_cache();
public:
	Reaction();
	~Reaction();

	bool empty() const;

	const ReactionComponentList& reactants() const { return m_reactants; }
	const ReactionComponentList& products() const { return m_products; }
	const ReactionComponentList& agents() const { return m_agents; }

	ReactionComponent* add_reactant();
	ReactionComponent* add_product();
	ReactionComponent* add_agent();
	void delete_agent(ReactionComponent* agent);

	ReactionComponent* reactant(int idx);
	ReactionComponent* product(int idx);
	ReactionComponent* agent(int idx);

	ReactionDirectionality directionality() const;
	void set_directionality(ReactionDirectionality dir);

	int nostructure_count(int group_idx);

	// Force re-calculation on next request of RInChI string, auxinfo or key.
	// void reset_rinchis(); // TODO: Implement.

	const std::string rinchi_string();
	const std::string rinchi_auxinfo();

	const std::string rinchi_long_key();
	const std::string rinchi_short_key();
	const std::string rinchi_web_key();
};

#ifdef IN_RINCHI_TEST_SUITE
std::string latest_hash_cleartext();
#endif

} // End of namespace.

#endif
