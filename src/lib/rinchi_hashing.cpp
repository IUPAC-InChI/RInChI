#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <sstream>

#include "rinchi_hashing.h"

#include "sha2.h"
#include "ikey_base26.h"

// #define RINCHI_DEBUG_OUTPUT_HASHING 1
#ifdef RINCHI_DEBUG_OUTPUT_HASHING
#include <iostream>
#endif

namespace rinchi {

std::string sha256hex(const std::string& input)
{
	unsigned char chksum[32];

	sha2_csum((unsigned char*) input.c_str(), (int) input.length(), chksum);

	std::ostringstream result;
	result << std::hex;
	for (int i = 0; i < 32; i++)
		result << ((int) chksum[i]);
	
	return result.str();
}

std::string hash04char(const std::string& input)
{
	unsigned char chksum[32];
	sha2_csum((unsigned char*) input.c_str(), (int) input.length(), chksum);

	std::string result;

	result += base26_triplet_1(chksum);
	result += base26_triplet_2(chksum);

	result = result.substr(0, 4);
#ifdef RINCHI_DEBUG_OUTPUT_HASHING
	std::cout << "[#04[" << input << "]#] => " << result << std::endl;
#endif
	
	return result;
}

std::string hash10char(const std::string& input)
{
	std::string result = hash12char(input).substr(0, 10);
#ifdef RINCHI_DEBUG_OUTPUT_HASHING
	std::cout << "[#10[" << input << "]#] => " << result << std::endl;
#endif
	
	return result;
}

std::string hash12char(const std::string& input)
{
	unsigned char chksum[32];
	sha2_csum((unsigned char*) input.c_str(), (int) input.length(), chksum);

	std::string result;

	result += base26_triplet_1(chksum);
	result += base26_triplet_2(chksum);
	result += base26_triplet_3(chksum);
	result += base26_triplet_4(chksum);
#ifdef RINCHI_DEBUG_OUTPUT_HASHING
	std::cout << "[#12[" << input << "]#] => " << result << std::endl;
#endif
	
	return result;
}

std::string hash14char(const std::string& input)
{
	unsigned char chksum[32];
	sha2_csum((unsigned char*) input.c_str(), (int) input.length(), chksum);

	std::string result;

	result += base26_triplet_1(chksum);
	result += base26_triplet_2(chksum);
	result += base26_triplet_3(chksum);
	result += base26_triplet_4(chksum);
	result += base26_dublet_for_bits_56_to_64(chksum);
#ifdef RINCHI_DEBUG_OUTPUT_HASHING
	std::cout << "[#14[" << input << "]#] => " << result << std::endl;
#endif
	
	return result;
}

std::string hash17char(const std::string& input)
{
	unsigned char chksum[32];
	sha2_csum((unsigned char*) input.c_str(), (int) input.length(), chksum);

	std::string result;

	result += base26_triplet_1(chksum);
	result += base26_triplet_2(chksum);
	result += base26_triplet_3(chksum);
	result += base26_triplet_4(chksum);
	result += base26_dublet_for_bits_56_to_64(chksum);
	// chksum[8] = bits 65->...
	result += base26_triplet_1(&chksum[8]);
#ifdef RINCHI_DEBUG_OUTPUT_HASHING
	std::cout << "[#17[" << input << "]#] => " << result << std::endl;
#endif
	return result;
}

const std::string HASH_04_EMPTY_STRING = "UHFF";
const std::string HASH_10_EMPTY_STRING = "UHFFFADPSC";
const std::string HASH_12_EMPTY_STRING = "UHFFFADPSCTJ";
const std::string HASH_14_EMPTY_STRING = "UHFFFADPSCTJAU";
const std::string HASH_17_EMPTY_STRING = "UHFFFADPSCTJAUYIS";

} // end of namespace
