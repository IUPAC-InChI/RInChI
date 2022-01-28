#ifndef IUPAC_RINCHI_INCHI_GENERATOR_HEADER_GUARD
#define IUPAC_RINCHI_INCHI_GENERATOR_HEADER_GUARD

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
