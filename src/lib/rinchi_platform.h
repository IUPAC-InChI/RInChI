#ifndef IUPAC_RINCHI_PLATFORM_HEADER_GUARD
#define IUPAC_RINCHI_PLATFORM_HEADER_GUARD

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

#ifdef _WIN32
	#define ON_WINDOWS
// TODO: Define MSVC appropriately. And use that condition directly above.
	const char DIR_SEPARATOR = '\\';
#elif defined(__linux__)
	#define ON_LINUX
	const char DIR_SEPARATOR = '/';
	#if defined(__i386__) || defined(__x86_64__)
		#define ON_LINUX_INTEL
	#endif
#elif defined(__sun__) && defined(__SVR4)
	#define ON_SOLARIS
	const char DIR_SEPARATOR = '/';
	#if defined(__sparc__)
		#define ON_BIGENDIAN
	#endif
#elif defined(__MACOSX__)
	// #define ON_MACOSX
	//
	// Since Mac OS X is built on Darwin, which is derived from BSD, the easy
	// option is to build on Apple as-if we were on Linux. Works for us.
	//
	// When on PowerPC Mac we should in principle define ON_BIGENDIAN, but
	// since none of our C++ code depends on endianness (at the time of
	// writing this), we don't need to worry about it.
	#define ON_LINUX
	const char DIR_SEPARATOR = '/';
#elif defined(__APPLE__)
	// #define ON_APPLE
	#define ON_LINUX
	const char DIR_SEPARATOR = '/';
#else
	Error Unsupported_platform
#endif

#endif
