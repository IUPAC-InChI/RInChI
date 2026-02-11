#ifndef IUPAC_RINCHI_MDLMOLFILEREADER_HEADER_GUARD
#define IUPAC_RINCHI_MDLMOLFILEREADER_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
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
