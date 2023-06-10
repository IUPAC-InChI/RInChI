#ifndef IUPAC_RINCHI_RINCHIREADER_HEADER_GUARD
#define IUPAC_RINCHI_RINCHIREADER_HEADER_GUARD

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
