#ifndef IUPAC_RINCHI_HASHING_HEADER_GUARD
#define IUPAC_RINCHI_HASHING_HEADER_GUARD

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

namespace rinchi {

/**
  SHA256 checksum of input in various encoded formats. **/

// Full SHA checksum in hexadecimal (32 chars).
std::string sha256hex(const std::string& input);

// 4-char InChI encoding, representing the first 18.66 bits of SHA checksum.
std::string hash04char(const std::string& input);

// 10-char InChI encoding, representing the first 46.66 bits of SHA checksum.
std::string hash10char(const std::string& input);

// 12-char InChI encoding, representing the first 56 bits of SHA checksum.
std::string hash12char(const std::string& input);

// 14-char InChI encoding, representing the first 64 bits of SHA checksum.
std::string hash14char(const std::string& input);

// 17-char InChI encoding, representing the first 78 bits of SHA checksum.
std::string hash17char(const std::string& input);

// Various lengths of InChI hash of an empty string.
extern const std::string HASH_04_EMPTY_STRING;
extern const std::string HASH_10_EMPTY_STRING;
extern const std::string HASH_12_EMPTY_STRING;
extern const std::string HASH_14_EMPTY_STRING;
extern const std::string HASH_17_EMPTY_STRING;

}

#endif
