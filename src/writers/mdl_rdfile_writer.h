#ifndef IUPAC_RINCHI_MDLRDFILEWRITER_HEADER_GUARD
#define IUPAC_RINCHI_MDLRDFILEWRITER_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <rinchi_reaction.h>

namespace rinchi {

class MdlRDfileWriterError: public std::runtime_error {
	public:
		explicit MdlRDfileWriterError(const std::string& what) : std::runtime_error(what) { }
};

class MdlRDfileWriter {
	public:
		MdlRDfileWriter() {}

		void write_reaction(const Reaction& reaction, std::ostream& output_stream);
};

} // end of namespace

#endif
