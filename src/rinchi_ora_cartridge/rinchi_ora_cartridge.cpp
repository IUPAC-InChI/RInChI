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

#include "rinchi_platform.h"
#include "rinchi_utils.h"
#include "rinchi_ora_cartridge.h"

#include "rinchi_reaction.h"

#include "rinchi_reader.h"
#include "mdl_rxnfile.h"
#include "mdl_rxnfile_reader.h"
#include "mdl_rdfile_reader.h"

#include "mdl_rxnfile_writer.h"
#include "mdl_rdfile_writer.h"

// Control external symbol export on Linux. NOTE: Requires that all files are compiled
// with '-fvisibility=hidden' to have an effect.
// On Windows we don't need to do this as symbols are explicitly exported by the .DEF file.
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

// Holds the part of input that is larger than MAX_PLSQL_VARCHAR2_LEN which is usually 32k.
std::stringstream cpp_input_buffer;
// Holds latest error message in case a C++ exception is thrown in an exported function.
std::string cpp_error_message;

#define BEGIN_EXCP_CODE try { cpp_error_message.clear();
#define END_EXCP_CODE return RETURN_CODE_SUCCESS; } catch (std::exception& e) { cpp_error_message = rinchi::demangled_class_name(typeid(e).name()) + ": " + e.what(); return RETURN_CODE_ERROR; } 

const char* RINCHI_INPUT_FORMAT_RDFILE = "RD";
const char* RINCHI_INPUT_FORMAT_RXNFILE = "RXN";
const char* RINCHI_INPUT_FORMAT_AUTO = "AUTO";

namespace {

	void load_reaction(const char* input_format, const char* in_file_text, rinchi::Reaction& rxn) {
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
			rd_reader.read_reaction(file_text_stream, rxn);
		}
		else if (file_format == RINCHI_INPUT_FORMAT_RXNFILE) {
			rinchi::MdlRxnfileReader rxn_reader;
			rxn_reader.read_reaction(file_text_stream, rxn);
		}
		else
			throw rinchi::RInChIError(std::string("Unsupported input file format '") + file_format + "'.");
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
		
		// TODO: Add check that 'key_type' is only one character long.
	}

}

inline void reset_stream(std::stringstream& s)
{
	s.str("");
	s.clear();
}

inline void copy_result_to_oracharbuf(const char* result, char* charbuf, int* charbuf_maxlen)
{
	// This produces a warning in MSVC that can be safely ignored
	// to the extent that we trust the maxlen that comes from Oracle.
#pragma warning(push)
#pragma warning(disable: 4996) // "'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead."
	strncpy(charbuf, result, *charbuf_maxlen);
	charbuf[*charbuf_maxlen] = 0; // Necessary, or Oracle will complain about a length mismatch problem.
#pragma warning(pop)
}

extern "C" {

	API_EXPORT const char* rinorca_latest_err_msg()
	{
		return cpp_error_message.c_str();
	}

	API_EXPORT const char* rinorca_debug_input_buffer()
	{
		cpp_error_message = cpp_input_buffer.str();
		return cpp_error_message.c_str();
	}

	API_EXPORT int rinorca_set_input_chunk(
		int append_to_buffer, const char* chunk
	)
	{
		BEGIN_EXCP_CODE
		if (append_to_buffer == 0)
			reset_stream(cpp_input_buffer);

		cpp_input_buffer << chunk;
		END_EXCP_CODE 
	}

	API_EXPORT int rinorca_rinchi_from_file(
		int append_to_buffer, const char* input_file, const char* input_format, int include_auxinfo, char* out_data, int* out_data_maxlen
	)
	{
		BEGIN_EXCP_CODE
		if (append_to_buffer == 0)
			reset_stream(cpp_input_buffer);

		cpp_input_buffer << input_file;

		rinchi::Reaction rxn;
		load_reaction(input_format, cpp_input_buffer.str().c_str(), rxn);

		std::string result = rxn.rinchi_string();
		if (include_auxinfo != 0)
			result += '\n' + rxn.rinchi_auxinfo();
		if ((int) result.length() > *out_data_maxlen)
			throw std::runtime_error("RInChI output length exceeds max. length of Oracle-supplied character buffer.");

		copy_result_to_oracharbuf(result.c_str(), out_data, out_data_maxlen);
		END_EXCP_CODE 
	}

	API_EXPORT int rinorca_rinchikey_from_file(
		int append_to_buffer, const char* input_file, const char* input_format, const char* key_type, char* out_data, int* out_data_maxlen
	)
	{
		BEGIN_EXCP_CODE
		if (append_to_buffer == 0)
			reset_stream(cpp_input_buffer);

		cpp_input_buffer << input_file;

		rinchi::Reaction rxn;
		load_reaction(input_format, cpp_input_buffer.str().c_str(), rxn);

		std::string result = calculate_key(rxn, key_type);
		if ((int) result.length() > *out_data_maxlen)
			throw std::runtime_error("RInChI key output length exceeds max. length of Oracle-supplied character buffer.");

		copy_result_to_oracharbuf(result.c_str(), out_data, out_data_maxlen);
		END_EXCP_CODE
	}

	API_EXPORT int rinorca_rinchikey_from_rinchi(
		int append_to_buffer, const char* rinchi_string, const char* key_type, char* out_data, int* out_data_maxlen
	)
	{
		BEGIN_EXCP_CODE
		if (append_to_buffer == 0)
			reset_stream(cpp_input_buffer);

		cpp_input_buffer << rinchi_string;

		rinchi::Reaction rxn;
		rinchi::RInChIReader reader;

		std::string rinchi_input = cpp_input_buffer.str();
		size_t lf_pos = rinchi_input.find('\n');
		if (lf_pos != std::string::npos)
			rinchi_input.erase(lf_pos);
		if (!rinchi_input.empty() && rinchi_input.at(rinchi_input.length() - 1) == '\r')
			rinchi_input.erase(rinchi_input.length() - 1);

		reader.split_into_reaction(rinchi_input, "", rxn);

		std::string result = calculate_key(rxn, key_type);
		if ((int) result.length() > *out_data_maxlen)
			throw std::runtime_error("RInChI key output length exceeds max. length of Oracle-supplied character buffer.");

		copy_result_to_oracharbuf(result.c_str(), out_data, out_data_maxlen);
		END_EXCP_CODE
	}

	API_EXPORT int rinorca_file_from_rinchi(
		const char* rinchi_string, const char* rinchi_auxinfo, const char* output_format, char* out_data, int* out_data_maxlen
	)
	{
		BEGIN_EXCP_CODE
		std::string the_rauxinfo = rinchi_auxinfo;
		// PL/SQL package will indicate a blank RAuxInfo field by passing a single space (to avoid NULLs).
		if (the_rauxinfo == " ")
			the_rauxinfo.clear();

		rinchi::Reaction rxn;
		rinchi::RInChIReader reader;
		reader.split_into_reaction(rinchi_string, the_rauxinfo, rxn);

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

		if ((int) file_text_stream.str().length() > *out_data_maxlen)
			throw std::runtime_error(std::string("Reaction file in '") + output_format + "' format has a length that exceeds max. length of Oracle-supplied character buffer.");

		copy_result_to_oracharbuf(file_text_stream.str().c_str(), out_data, out_data_maxlen);
		END_EXCP_CODE 
	}

}
