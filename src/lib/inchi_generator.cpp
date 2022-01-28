#ifdef MSVC
#pragma region BSD-license
/*
 * Reaction International Chemical Identifier (RInChI)
 * Version 1
 * Software version 1.00
 * 2022-01-14
 * 
 * This source file is based on work created by Biochemfusion Holding ApS.
 * It is released under a BSD-style license.
 * 
 * Copyright (C) 2010 - 2015, Biochemfusion Holding ApS
 * (http://www.biochemfusion.com). All rights reserved.
 * 
 * Redistribution and use for any purpose in source and binary forms, with or
 * without modification, are permitted, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY Biochemfusion Holding ApS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Biochemfusion Holding ApS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma endregion
#endif

#include <algorithm>
#include <sstream>

#include "inchi_generator.h"

#include "inchi_api_intf.h"
#include "rinchi_consts.h"
#include "rinchi_utils.h"

namespace rinchi {

namespace {

/*
  The state of the lib must be kept in a singleton. This way no harm is done if you accidentally
  instantiate and use multiple InChIGenerator instances.
*/
struct InChIState {
	INCHIGEN_HANDLE generator_handle;
	INCHIGEN_DATA generator_data;
	inchi_Output inchi_result;
	/** From InChI 1.04 API reference:
		szINCHIKey - InChIKey string, null-terminated. The user-supplied buffer szINCHIKey should be at least 28 bytes long.
		szXtra1 - hash extension (up to 256 bits; 1st block) string. Caller should allocate space for 64 characters + trailing NULL.
		szXtra2 - hash extension (up to 256 bits; 2nd block) string. Caller should allocate space for 64 characters + trailing NULL.
	**/
	char inchi_key[32];
	char xtra1[65];
	char xtra2[65];

	InChIState(): generator_handle(0) { }
	~InChIState()
	{
		if (generator_handle == 0)
			return;

		lib_INCHIGEN_Destroy(generator_handle);
	}

} inchi_lib_state;

/*
  Temporary InChI generation call state with guaranteed cleanup.
*/
class InChICallState {
public:
	InChICallState()
	{
		if (inchi_lib_state.generator_handle == 0)
			inchi_lib_state.generator_handle = lib_INCHIGEN_Create();
	}
	~InChICallState()
	{
		lib_INCHIGEN_Reset(inchi_lib_state.generator_handle, &inchi_lib_state.generator_data, &inchi_lib_state.inchi_result);
	}

	static void check_return_code(int return_code)
	{
		if (return_code == inchi_Ret_OKAY)
			; // Success; no errors or warnings
		else if (return_code == inchi_Ret_WARNING)
			; // Success; warning(s) issued /** SET WARNING FLAG **/
		else if (return_code == inchi_Ret_ERROR)
			throw InChIGeneratorError ("Error: no InChI has been created.");
		else if (return_code == inchi_Ret_FATAL)
			throw InChIGeneratorError ("Severe error: no InChI has been created (typically, memory allocation failure).");
		else if (return_code == inchi_Ret_UNKNOWN)
			throw InChIGeneratorError ("InChI library: Unknown program error.");
		else if (return_code == inchi_Ret_BUSY)
			throw InChIGeneratorError ("Previuos call to InChI has not returned yet.");
		else if (return_code == inchi_Ret_EOF)
			throw InChIGeneratorError ("InChI library: No structural data has been provided.");
		else if (return_code == inchi_Ret_SKIP)
			throw InChIGeneratorError ("InChI library: Not used in InChI library (?)");
		else
			throw InChIGeneratorError ("Unexpected return code from InChI library (" + int2str(return_code) + ").");
	}

	void run_inchi_generation(inchi_Input& inchi_input)
	{
		check_return_code(
			lib_INCHIGEN_Setup(inchi_lib_state.generator_handle, &inchi_lib_state.generator_data, &inchi_input)
		);
		check_return_code(
			lib_INCHIGEN_DoNormalization(inchi_lib_state.generator_handle, &inchi_lib_state.generator_data)
		);
		check_return_code(
			lib_INCHIGEN_DoCanonicalization(inchi_lib_state.generator_handle, &inchi_lib_state.generator_data)
		);
		check_return_code(
			lib_INCHIGEN_DoSerialization(inchi_lib_state.generator_handle, &inchi_lib_state.generator_data, &inchi_lib_state.inchi_result)
		);
	}

	static void check_key_return_code(int return_code)
	{
		if (return_code == INCHIKEY_OK)
			; // Success; no errors or warnings
		else if (return_code == INCHIKEY_UNKNOWN_ERROR)
			throw InChIGeneratorError ("InChI library (generating key): Unknown program error.");
		else if (return_code == INCHIKEY_EMPTY_INPUT)
			throw InChIGeneratorError ("InChI library (generating key): Source string is empty.");
		else if (return_code == INCHIKEY_INVALID_INCHI_PREFIX)
			throw InChIGeneratorError ("Invalid InChI prefix or invalid version.");
		else if (return_code == INCHIKEY_NOT_ENOUGH_MEMORY)
			throw InChIGeneratorError ("InChI library (generating key): Not enough memory.");
		else if (return_code == INCHIKEY_INVALID_INCHI)
			throw InChIGeneratorError ("Source InChI has invalid layout.");
		else if (return_code == INCHIKEY_INVALID_STD_INCHI)
			throw InChIGeneratorError ("Source standard InChI has invalid layout.");
		else
			throw InChIGeneratorError ("Unexpected return code from InChI library when generating InChI key (" + int2str(return_code) + ").");
	}

	void run_inchi_key_generation()
	{
		int return_code = lib_GetINCHIKeyFromINCHI(
			inchi_lib_state.inchi_result.szInChI, 0, 0,
			inchi_lib_state.inchi_key, inchi_lib_state.xtra1, inchi_lib_state.xtra2
		);
		check_key_return_code(return_code);
	}

};

} // End of anonymous namespace.

/** InChIGenerator **/

void InChIGenerator::calculate_inchi(inchi_Input& inchi_input, std::string& o_inchi_string, std::string& o_aux_info)
{
/*	** InChI modularized interface crashes if inchi_input is a No-Structure.
    ** The simpler lib_GetINCHI() API is more stable, so we'll use that instead.

	InChICallState call_state;
	call_state.run_inchi_generation(inchi_input);

	o_inchi_string = inchi_lib_state.inchi_result.szInChI;
	o_aux_info     = inchi_lib_state.inchi_result.szAuxInfo;
	*/
	
	inchi_Output inchi_output;
	memset(&inchi_output, 0, sizeof(inchi_output));

	int rc = lib_GetINCHI(&inchi_input, &inchi_output);
	if (rc == inchi_Ret_OKAY || rc == inchi_Ret_WARNING) {
		o_inchi_string = inchi_output.szInChI;
		o_aux_info     = inchi_output.szAuxInfo;
	}
	lib_FreeINCHI(&inchi_output);
	InChICallState::check_return_code(rc);
}

void InChIGenerator::validate_inchi(const std::string& inchi_string)
{
	if (inchi_string == rinchi::NOSTRUCT_INCHI)
		return;

	inchi_Output inchi_output;
	memset(&inchi_output, 0, sizeof(inchi_output));

	inchi_InputINCHI inchi_input;
	memset(&inchi_input, 0, sizeof(inchi_input));

	inchi_input.szInChI = (char*) inchi_string.c_str();
	inchi_input.szOptions = (char *) "";

	int rc = lib_GetINCHIfromINCHI(&inchi_input, &inchi_output);
	if (rc == inchi_Ret_OKAY || rc == inchi_Ret_WARNING) {
		// Even when return code says that all is OK, the output may be NULL.
		// The error message is then returned as part of the log :-(.
		if (inchi_output.szInChI == NULL) {
			std::stringstream log_lines (inchi_output.szLog);
			std::string line;
			std::string err_msg;
			while (true) {
				std::getline(log_lines, line);
				if (!log_lines)
					break;
				if (!err_msg.empty())
					err_msg += "\n" + line;
				else {
					if (line.find("error") != std::string::npos)
						err_msg = line;
				}
			}
			if (err_msg.empty())
				throw InChIGeneratorError("Unknown error validating InChI.");
			else
				throw InChIGeneratorError(err_msg);
		}
	}
	lib_FreeINCHI(&inchi_output);
	InChICallState::check_return_code(rc);
}

std::string InChIGenerator::inchi_key(inchi_Input& inchi_input)
{
	InChICallState call_state;
	call_state.run_inchi_generation(inchi_input);
	call_state.run_inchi_key_generation();

	return inchi_lib_state.inchi_key;
}

std::string InChIGenerator::inchi_string2key(const std::string& inchi_string)
{
	int return_code = lib_GetINCHIKeyFromINCHI(
		inchi_string.c_str(), 0, 0,
		inchi_lib_state.inchi_key, inchi_lib_state.xtra1, inchi_lib_state.xtra2
	);
	InChICallState::check_key_return_code(return_code);

	return inchi_lib_state.inchi_key;
}

/** InChIToStructureConverter **/

InChIToStructureConverter::InChIToStructureConverter()
{
	reset();

	if (inchi_lib_state.generator_handle == 0)
		inchi_lib_state.generator_handle = lib_INCHIGEN_Create();
}

InChIToStructureConverter::~InChIToStructureConverter()
{
	free_inchi_structs();
}

void InChIToStructureConverter::free_inchi_structs()
{
	// Zero pointers to strings that we own - just in case.
	m_input_inchi.szOptions = NULL;

	lib_Free_inchi_Input(&m_input_inchi);
	lib_FreeINCHI(&m_output);
	lib_FreeStructFromINCHI(&m_output_struct);
}

void InChIToStructureConverter::reset()
{
	memset(&m_input_data, 0, sizeof(m_input_data));
	memset(&m_input_inchi, 0, sizeof(m_input_inchi));
	memset(&m_output, 0, sizeof(m_output));

	memset(&m_input_inchi_struct, 0, sizeof(m_input_inchi_struct));
	memset(&m_output_struct, 0, sizeof(m_output_struct));

}

const
	std::string NO_STRUCTURE_MOLFILE =
	  "\n"
	  "  InChIV10\n"
	  "\n"
	  "  0  0  0  0  0  0  0  0  0  0999 V2000\n"
	  "M  END\n";

std::string InChIToStructureConverter::to_molfile(const std::string& inchi_string, const std::string& auxinfo)
{
	if (m_input_data.pInp != NULL) {
		free_inchi_structs();
		reset();
	}

	m_input_data.pInp = &m_input_inchi;

	std::string full_input = inchi_string;
	// We need to handle InChI No-Structures explicitly. Apparently InChI can't convert an empty
	// InChI into a No-Structure molfile.
	if (full_input == rinchi::NOSTRUCT_INCHI) {
		return NO_STRUCTURE_MOLFILE;
	}

	int output_chiral_flag = 0;
	// If we have AuxInfo, then rebuild the structure from that.
	if (!auxinfo.empty()) {
		full_input += "\n" + auxinfo;
		InChICallState::check_return_code(lib_Get_inchi_Input_FromAuxInfo((char*) full_input.c_str(), 0, 0, &m_input_data));
		// Information extracted from AuxInfo on the use of the MDL chiral flag is returned into m_input_data.bChiral.
		// Thanks to Igor Pletnev for clearing that up.
		output_chiral_flag = m_input_data.bChiral;
	}
	else {
		m_input_inchi_struct.szInChI = (char*) full_input.c_str();
		InChICallState::check_return_code(lib_GetStructFromINCHI(&m_input_inchi_struct, &m_output_struct));

		// Take ownership of returned atom data from 'm_output_struct'.
		m_input_inchi.atom         = m_output_struct.atom;
		m_input_inchi.num_atoms    = m_output_struct.num_atoms;
		m_input_inchi.stereo0D     = m_output_struct.stereo0D;
		m_input_inchi.num_stereo0D = m_output_struct.num_stereo0D;
		m_output_struct.atom = NULL;
		m_output_struct.stereo0D = NULL;
	}

	// Convert inchi_Input returned from above to SD record. Differing options are needed to
	// avoid losing chirality information in output.
	// (char*) cast to avoid "warning: deprecated conversion from string constant to ‘char*’" from gcc.
	if (output_chiral_flag == 1)
		m_input_inchi.szOptions = (char*) INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_ON;
	else if (output_chiral_flag == 2)
		m_input_inchi.szOptions = (char*) INCHI_OUTPUT_OPTION_SDF_CHIRAL_FLAG_OFF;
	else
		// No AuxInfo used, or AuxInfo not marked as chiral.
		m_input_inchi.szOptions = (char*) INCHI_OUTPUT_OPTION_SDF;

	InChICallState::check_return_code(lib_GetINCHI(&m_input_inchi, &m_output));

	// When requesting "/OutputSDF" then the SD record is put into 'szInChI' (see
	// e_ichimain.c line 761 and forward).
	// Discard the first line containing an auto-generated structure name.
	char* result_start = m_output.szInChI;
	if (result_start == NULL)
		throw InChIGeneratorError ("NULL string returned from successful GetINCHI call!");
	while (*result_start != '\n' && *result_start != 0)
		result_start++;

	std::string result = result_start;
	// Remove trailing "$$$$\n" line from SD record, giving us the molfile only.
	if (result.length() < 5)
		throw InChIGeneratorError ("Output from GetINCHI() is not recognized as an SD record.");
	result.erase(result.length() - 5);

	return result;
}

} // End of namespace.
