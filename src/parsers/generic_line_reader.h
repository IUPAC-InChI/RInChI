#ifndef IUPAC_RINCHI_GENERIC_LINE_READER_HEADER_GUARD
#define IUPAC_RINCHI_GENERIC_LINE_READER_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
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
