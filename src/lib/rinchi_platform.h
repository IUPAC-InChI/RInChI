#ifndef IUPAC_RINCHI_PLATFORM_HEADER_GUARD
#define IUPAC_RINCHI_PLATFORM_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
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
