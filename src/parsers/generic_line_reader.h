#ifndef IUPAC_RINCHI_GENERIC_LINE_READER_HEADER_GUARD
#define IUPAC_RINCHI_GENERIC_LINE_READER_HEADER_GUARD

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

#include <sstream>

#include "rinchi_utils.h"

namespace rinchi {

// A base for building a line reader, e.g. MolfileReader, UniprotReader etc.
// Instantiate it with the exception class you want it to throw from
// throw_error(), e.g. class UniprotReader: public GenericLineReader<UniprotReaderError> [...] .

template <typename ExceptionType> class GenericLineReader {
	protected:
		std::string m_input_name;
		std::string m_current_line;
		int m_line_number;

		void get_next_line(std::istream& input_stream)
		{
			if (!input_stream)
				throw_error("Premature end of input");
			rinchi_getline(input_stream, m_current_line);
			m_line_number++;
		}

		void check(bool condition, const std::string& err_msg)
		{
			rinchi::check<ExceptionType>(condition, err_msg.c_str());
		}

		void check_range(int value, int min, int max, const std::string& err_msg)
		{
			rinchi::check_range<ExceptionType>(value, min, max, err_msg.c_str());
		}

		void throw_error(const std::string& msg)
		{
			std::ostringstream err_msg;
			err_msg << "Reading from '" << m_input_name << "'";
			if (m_line_number)
				err_msg << ", line " << m_line_number;
			err_msg << ": " << msg;
			if (msg[msg.length() - 1] != '.')
				err_msg << ".";
			throw ExceptionType(err_msg.str());
		}

		void throw_error(std::exception& e)
		{
			std::ostringstream err_msg;
			err_msg << "Reading from '" << m_input_name << "'";
			if (m_line_number)
				err_msg << ", line " << m_line_number;
			if (typeid(e) != typeid(ExceptionType))
				err_msg << ", " << demangled_class_name(typeid(e).name()) << ": ";
			else
				err_msg << ": ";
			std::string msg = e.what();
			err_msg << msg;
			if (msg[msg.length() - 1] != '.')
				err_msg << ".";
			throw ExceptionType(err_msg.str());
		}

	public:
		GenericLineReader() {}

		// Returns number of lines read from input.
		int lines_read() { return m_line_number; }
};

} // end of namespace

#endif
