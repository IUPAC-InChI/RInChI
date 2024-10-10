#ifndef IUPAC_RINCHI_HASHING_HEADER_GUARD
#define IUPAC_RINCHI_HASHING_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
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
