#ifndef IUPAC_RINCHI_MDLRXNFILEREADER_HEADER_GUARD
#define IUPAC_RINCHI_MDLRXNFILEREADER_HEADER_GUARD

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
