#ifndef IUPAC_RINCHI_MDLRXNFILEREADER_HEADER_GUARD
#define IUPAC_RINCHI_MDLRXNFILEREADER_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include "generic_line_reader.h"
#include "rinchi_reaction.h"

namespace rinchi {

class MdlRxnfileReaderError: public std::runtime_error {
	public:
		explicit MdlRxnfileReaderError(const std::string& what) : std::runtime_error(what) { }
};

class MdlRxnfileReader: public GenericLineReader<MdlRxnfileReaderError> {
	private:
		void read_reaction_from_stream(std::istream& input_stream, Reaction& result, bool force_equilibrium);
	public:
		MdlRxnfileReader() {}

		void read_reaction(const std::string& file_name, Reaction& result, bool force_equilibrium = false, int lines_already_read = 0);
		void read_reaction(std::istream& input_stream, Reaction& result, bool force_equilibrium = false, int lines_already_read = 0);
};

} // end of namespace

#endif
