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
#include <sstream>
#ifdef LIB_RINCHI_DEBUG
	#include <iostream>
#endif

#include "rinchi_platform.h"
#include "rinchi_lib.h"
#include "rinchi_utils.h"

#include "rinchi_consts.h"
#include "rinchi_reaction.h"

#include "mdl_rxnfile.h"
#include "mdl_rxnfile_reader.h"
#include "mdl_rdfile_reader.h"

#include "rinchi_reader.h"
#include "mdl_rxnfile_writer.h"
#include "mdl_rdfile_writer.h"

// Control external symbol export on Linux. NOTE: Requires that all files are compiled
// with '-fvisibility=hidden' to have an effect.
// On Windows we don't need to do this as symbols are explicitly exported only by the .DEF file.
#ifdef ON_LINUX
	#define API_EXPORT __attribute__((visibility("default")))

	// Init function attribute for shared libraries. We only want it active when compiling
	// a shared library, 'cause in a normal executable it runs before static initializers
	// which leads to "interesting" behavior.
	#ifdef __PIC__
		#define SHARED_LIB_INIT_FUNC_ATTR __attribute__ ((constructor))
	#else
		#define SHARED_LIB_INIT_FUNC_ATTR
	#endif

#else
	// Windows: Nothing special needed. Handled by .DEF file.
	#define API_EXPORT
#endif

const char* RINCHI_INPUT_FORMAT_RDFILE = "RD";
const char* RINCHI_INPUT_FORMAT_RXNFILE = "RXN";
const char* RINCHI_INPUT_FORMAT_AUTO = "AUTO";

// Holds latest error message in case a C++ exception is thrown in an exported function.
std::string cpp_error_message;
// Holds latest results.
std::string cpp_result1;
std::string cpp_result2;

#define BEGIN_EXCP_CODE try { cpp_error_message.clear();
#define END_EXCP_CODE return RETURN_CODE_SUCCESS; } catch (std::exception& e) { cpp_error_message = rinchi::demangled_class_name(typeid(e).name()) + ": " + e.what(); return RETURN_CODE_ERROR; }

namespace {

	void load_reaction(const char* input_format, const char* in_file_text, bool in_force_equilibrium, rinchi::Reaction& rxn) {
		std::stringstream file_text_stream (in_file_text);

		std::string file_format = input_format;
		if (file_format.empty() || file_format == RINCHI_INPUT_FORMAT_AUTO) {
			std::ios::pos_type saved_stream_pos = file_text_stream.tellg();
			std::string first_line;
			rinchi::rinchi_getline(file_text_stream, first_line);
			file_text_stream.seekg(saved_stream_pos);

			if (first_line == rinchi::MDL_TAG_RXN_BEGIN)
				file_format = RINCHI_INPUT_FORMAT_RXNFILE;
			else
				file_format = RINCHI_INPUT_FORMAT_RDFILE;
		}

		if (file_format == RINCHI_INPUT_FORMAT_RDFILE) {
			rinchi::MdlRDfileReader rd_reader;
			rd_reader.read_reaction(file_text_stream, rxn, in_force_equilibrium);
		}
		else if (file_format == RINCHI_INPUT_FORMAT_RXNFILE) {
			rinchi::MdlRxnfileReader rxn_reader;
			rxn_reader.read_reaction(file_text_stream, rxn, in_force_equilibrium);
		}
		else
			throw rinchi::RInChIError(std::string("Unsupported input file format '") + file_format + "'.");
	}

	void output_component_as_string(const std::string& prefix, rinchi::ReactionComponent& rc, std::ostream& output_stream)
	{
		output_stream << prefix << rc.inchi_string() << "\n" << prefix << rc.inchi_auxinfo() << "\n";
	}

	std::string calculate_key(rinchi::Reaction& rxn, const char* key_type)
	{
		char key_selector = *key_type;
		if (key_selector == 0)
			throw rinchi::RInChIError("Missing key selector: 'key_type' parameter must be 'L'(ong), 'S'(hort) or W(eb).");

		if (key_selector == 'L')
			return rxn.rinchi_long_key();
		else if (key_selector == 'S')
			return rxn.rinchi_short_key();
		else if (key_selector == 'W')
			return rxn.rinchi_web_key();
		else
			throw rinchi::RInChIError("Invalid key selector. 'key_type' parameter must be 'L'(ong), 'S'(hort) or W(eb).");
	}

}

extern "C" {

	API_EXPORT const char* rinchilib_latest_err_msg()
	{
		return cpp_error_message.c_str();
	}

	API_EXPORT int rinchilib_rinchi_from_file_text(
		const char* input_format, const char* in_file_text, bool in_force_equilibrium, const char** out_rinchi_string, const char** out_rinchi_auxinfo
	)
	{
		BEGIN_EXCP_CODE
		rinchi::Reaction rxn;
		load_reaction(input_format, in_file_text, in_force_equilibrium, rxn);
		cpp_result1 = rxn.rinchi_string();
		cpp_result2 = rxn.rinchi_auxinfo();

		*out_rinchi_string  = cpp_result1.c_str();
		*out_rinchi_auxinfo = cpp_result2.c_str();
		END_EXCP_CODE
	}

	API_EXPORT int rinchilib_rinchikey_from_file_text(
		const char* input_format, const char* in_file_text, const char* key_type, bool in_force_equilibrium, const char** out_rinchi_key
	)
	{
		BEGIN_EXCP_CODE
		rinchi::Reaction rxn;
		load_reaction(input_format, in_file_text, in_force_equilibrium, rxn);
		cpp_result1 = calculate_key(rxn, key_type);
		*out_rinchi_key = cpp_result1.c_str();
		END_EXCP_CODE
	}

	API_EXPORT int rinchilib_file_text_from_rinchi(
		const char* rinchi_string, const char* rinchi_auxinfo, const char* output_format, const char** out_file_text
	)
	{
		BEGIN_EXCP_CODE
		rinchi::Reaction rxn;
		rinchi::RInChIReader reader;
#ifdef LIB_RINCHI_DEBUG
		std::cout << "RInChI string:\n" << rinchi_string << "\nRAuxInfo:\n" << rinchi_auxinfo << std::endl;
#endif
		reader.split_into_reaction(rinchi_string, rinchi_auxinfo, rxn);

		std::ostringstream file_text_stream;
		if (strcmp(output_format, RINCHI_INPUT_FORMAT_RDFILE) == 0) {
			rinchi::MdlRDfileWriter rd_writer;
			rd_writer.write_reaction(rxn, file_text_stream);
		}
		else if (strcmp(output_format, RINCHI_INPUT_FORMAT_RXNFILE) == 0) {
			rinchi::MdlRxnfileWriter rxn_writer;
			rxn_writer.write_reaction(rxn, file_text_stream);
		}
		else
			throw rinchi::RInChIError(std::string("Unsupported output file format '") + output_format + "'.");

		cpp_result1 = file_text_stream.str();
		*out_file_text = cpp_result1.c_str();
		END_EXCP_CODE
	}

	API_EXPORT int rinchilib_inchis_from_rinchi(
		const char* rinchi_string, const char* rinchi_auxinfo, const char** out_inchis_text
	)
	{
		BEGIN_EXCP_CODE
		rinchi::Reaction rxn;
		rinchi::RInChIReader reader;
#ifdef LIB_RINCHI_DEBUG
		std::cout << "RInChI string:\n" << rinchi_string << "\nRAuxInfo:\n" << rinchi_auxinfo << std::endl;
#endif
		bool reverse_rxn_dir;
		reader.split_into_reaction(rinchi_string, rinchi_auxinfo, rxn, &reverse_rxn_dir);

		std::ostringstream inchi_stream;

		// Add direction flag.
		switch (rxn.directionality())
		{
		case rinchi::rdDirectional:
			if (reverse_rxn_dir)
				inchi_stream << "D:" << rinchi::DIRECTION_REVERSE << "\n";
			else
				inchi_stream << "D:" << rinchi::DIRECTION_FORWARD << "\n";
			break;
		case rinchi::rdEquilibrium:
			inchi_stream <<  "D:" << rinchi::DIRECTION_EQUILIBRIUM << "\n";
			break;
		default:
			throw rinchi::RInChIError("Internal bug: Unexpected reaction direction value (" + int2str(rxn.directionality()) + ".");
		}

		// Add comma-separated No-Structure counts. Always Reactants first, then Products, then Agents.
		inchi_stream << "N:";
		for (int i = 0; i < RINCHI_NUM_GROUPS; i++) {
			if (i != 0)
				inchi_stream << ",";
			inchi_stream << rxn.nostructure_count(i);
		}
		inchi_stream << "\n";

		// Add InChI and AuxInfo for all Reactants, Products, and Agents.
		for (rinchi::ReactionComponentList::const_iterator rc = rxn.reactants().begin(); rc != rxn.reactants().end(); rc++)
			output_component_as_string("R:", **rc, inchi_stream);
		for (rinchi::ReactionComponentList::const_iterator rc = rxn.products().begin(); rc != rxn.products().end(); rc++)
			output_component_as_string("P:", **rc, inchi_stream);
		for (rinchi::ReactionComponentList::const_iterator rc = rxn.agents().begin(); rc != rxn.agents().end(); rc++)
			output_component_as_string("A:", **rc, inchi_stream);

		cpp_result1 = inchi_stream.str();
		*out_inchis_text = cpp_result1.c_str();
		END_EXCP_CODE
	}

    API_EXPORT int rinchilib_rinchi_from_inchis(
            const char* reactant_inchis, const char* product_inchis, const char* agent_inchis, const char** out_rinchi_string, const char** out_rinchi_auxinfo
    )
    {
        BEGIN_EXCP_CODE
        rinchi::Reaction rxn;
        rinchi::RInChIReader reader;

        std::string reactant_inchis_str;
        std::string product_inchis_str;
        std::string agent_inchis_str;
        if (reactant_inchis != nullptr) reactant_inchis_str = reactant_inchis;
        if (product_inchis  != nullptr) product_inchis_str  = product_inchis;
        if (agent_inchis    != nullptr) agent_inchis_str    = agent_inchis;

        reader.add_inchis_to_reaction(reactant_inchis_str, product_inchis_str, agent_inchis_str, rxn);

        cpp_result1 = rxn.rinchi_string();
        cpp_result2 = rxn.rinchi_auxinfo();

        *out_rinchi_string  = cpp_result1.c_str();
        *out_rinchi_auxinfo = cpp_result2.c_str();
        END_EXCP_CODE
    }

	API_EXPORT int rinchilib_rinchikey_from_rinchi(
		const char* rinchi_string, const char* key_type, const char** out_rinchi_key
	)
	{
		BEGIN_EXCP_CODE
#ifdef LIB_RINCHI_DEBUG
		std::cout << "RInChI string:\n" << rinchi_string << std::endl;
#endif
		rinchi::Reaction rxn;
		rinchi::RInChIReader reader;

		// Cut input at first LF or CR/LF.
		std::string rinchi_input = rinchi_string;
		size_t lf_pos = rinchi_input.find('\n');
		if (lf_pos != std::string::npos)
			rinchi_input.erase(lf_pos);
		if (!rinchi_input.empty() && rinchi_input.at(rinchi_input.length() - 1) == '\r')
			rinchi_input.erase(rinchi_input.length() - 1);

		reader.split_into_reaction(rinchi_input, "", rxn);

		cpp_result1 = calculate_key(rxn, key_type);
		*out_rinchi_key = cpp_result1.c_str();
		END_EXCP_CODE
	}

}
