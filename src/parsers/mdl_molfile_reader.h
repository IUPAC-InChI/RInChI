#ifndef IUPAC_RINCHI_MDLMOLFILEREADER_HEADER_GUARD
#define IUPAC_RINCHI_MDLMOLFILEREADER_HEADER_GUARD

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

#include <generic_line_reader.h>
#include <rinchi_logger.h>
#include <rinchi_reaction.h>

namespace rinchi {

class MdlMolfileReaderError: public std::runtime_error {
	public:
		explicit MdlMolfileReaderError(const std::string& what) : std::runtime_error(what) { }
};

// X-, Y-, and Z-coordinates are represented as double-s.
typedef double Coordinate;

class MdlMolfileReader: public GenericLineReader<MdlMolfileReaderError> {
	private:
		RInChILogger& m_warning_logger;

		void read_V2000_molecule_from_stream(int atom_count, int bond_count, bool is_chiral, bool has_version_stamp, int prop_line_count, std::istream& input_stream, ReactionComponent& result);
		void read_V3000_molecule_from_stream(bool is_chiral, std::istream& input_stream, ReactionComponent& result);

		void read_molecule_from_stream(std::istream& input_stream, ReactionComponent& result);
	public:
		MdlMolfileReader(RInChILogger& warning_logger = default_warning_logger());

		void read_molecule(const std::string& file_name, ReactionComponent& result, int lines_already_read = 0);
		void read_molecule(std::istream& input_stream, ReactionComponent& result, int lines_already_read = 0);
};

} // end of namespace

#endif
