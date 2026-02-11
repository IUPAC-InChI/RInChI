#ifndef IUPAC_RINCHI_INCHI_GENERATOR_HEADER_GUARD
#define IUPAC_RINCHI_INCHI_GENERATOR_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <string>
#include <stdexcept>
#include <inchi_api.h>

namespace rinchi {

class InChIGeneratorError: public std::runtime_error {
public:
	explicit InChIGeneratorError(const std::string& what) : std::runtime_error(what) { }
};

class InChIGenerator {
public:
	InChIGenerator () {}

	void calculate_inchi(inchi_Input& inchi_input, std::string& o_inchi_string, std::string& o_aux_info);
	void validate_inchi(const std::string& inchi_string);

	std::string inchi_key(inchi_Input& inchi_input);
	std::string inchi_string2key(const std::string& inchi_string);
};

// Generates structure from InChI string and optionally AuxInfo.
// If AuxInfo is blank, a 0D structure will be returned.
class InChIToStructureConverter {
private:
	// Following three used when AuxInfo present.
	InchiInpData m_input_data;
	inchi_Input m_input_inchi;
	inchi_Output m_output;
	// Following two used to generate 0D structure.
	inchi_InputINCHI m_input_inchi_struct;
	inchi_OutputStruct m_output_struct;
	//
	void free_inchi_structs();
	void reset();
public:
	InChIToStructureConverter();
	~InChIToStructureConverter();

	std::string to_molfile(const std::string& inchi_string, const std::string& auxinfo);
};

} // End of namespace.

#endif
