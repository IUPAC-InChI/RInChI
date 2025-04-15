#ifndef IUPAC_RINCHI_RINCHIREADER_HEADER_GUARD
#define IUPAC_RINCHI_RINCHIREADER_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <rinchi_reaction.h>

namespace rinchi {

/**
  The RInChIReader parses an RInChI string and populates a corresponding
  Reaction. The reaction will end up with components that contain the 
  individual InChIs of the input RInChI string appropriately distributed
  over reactants, products, and agents.
**/
class RInChIReaderError: public RInChIError {
	public:
		explicit RInChIReaderError(const std::string& what) : RInChIError(what) { }
};

class RInChIReader {
public:
	RInChIReader() {}

        /**
                Splits an RInChI string into its separate InChI strings and puts them
                in a Reaction instance.
                If AuxInfo is provided it will be transferred into the reaction's components
                as well.
                If 'is_reverse_direction' is non-NULL the status of the direction flag
                (if not equilibrium) will be passed back, + => false, - => true.
                NOTE: The Reaction instance that you pass in must be empty!
	**/
	void split_into_reaction(const std::string& rinchi_string, const std::string& rinchi_auxinfo, Reaction& rxn, bool* is_reverse_direction = NULL);

        /**
		Adds InChI strings and optionally AuxInfo of components to a Reaction instance.
		Input InChI strings are linefeed-delimited. Each InChI string may be optionally
		followed by its AuxInfo. For instance, three reactants may be represented as:

		InChI=...    <-- Reactant #1
		InChI=...    <-- Reactant #2
		InChI=...    <-- Reactant #3

		or

		InChI=...    <-- Reactant #1
		AuxInfo=...  <-- Reactant #1, AuxInfo
		InChI=...    <-- Reactant #2
		AuxInfo=...  <-- Reactant #2, AuxInfo
		InChI=...    <-- Reactant #3
		AuxInfo=...  <-- Reactant #3, AuxInfo

		or

		InChI=...    <-- Reactant #1
		InChI=...    <-- Reactant #2
		AuxInfo=...  <-- Reactant #2, AuxInfo
		InChI=...    <-- Reactant #3

		In the last example, only reactant #2 will have AuxInfo attached. This mode of mixing
		components with and without AuxInfo is NOT recommended.
        **/
	void add_inchis_to_reaction(const std::string& reactant_inchis, const std::string& product_inchis, const std::string& agent_inchis, Reaction& rxn);

};

} // end of namespace

#endif
