#ifndef IUPAC_RINCHI_UTILS_HEADER_GUARD
#define IUPAC_RINCHI_UTILS_HEADER_GUARD

#ifdef MSVC
#pragma region BSD-license
/*
 * Reaction International Chemical Identifier (RInChI)
 * Version 1
 * Software version 1.00
 * 2022-01-14
 * 
 * This source file is based on work created by Biochemfusion Holding ApS.
 * It is released under a BSD-style license.
 * 
 * Copyright (C) 2010 - 2015, Biochemfusion Holding ApS
 * (http://www.biochemfusion.com). All rights reserved.
 * 
 * Redistribution and use for any purpose in source and binary forms, with or
 * without modification, are permitted, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY Biochemfusion Holding ApS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Biochemfusion Holding ApS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma endregion
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <rinchi_platform.h>

#if defined(ON_LINUX) || defined(ON_SOLARIS)
	#include <math.h>
	#include <stdexcept>
	#include <string.h>
#endif

#ifdef __GNUC__
	#include <cxxabi.h>
#endif

namespace rinchi {

void chdir(const char* path);

/**
  Number conversion.
**/

// From http://lists.boost.org/Archives/boost/2004/07/68381.php
// by ~Matt Verona
// matt_verona_at_[hidden]
// Houston, Texas
// Really only necessary on Windows, since Linux already has it in <math.h>.
inline int round(double number)
{
	double result = number < 0.0 ? ceil(number-0.5) : floor(number+0.5);
	// Use explict cast to int to avoid warning C4244 in Visual C8.
	return (int) result;
}

class BadConversion: public std::runtime_error {
	public:
		BadConversion(const std::string& s): std::runtime_error(s) { }
};

std::string int2str(int i);

int str2int(const std::string& s, bool failIfLeftoverChars = true);
int str2int(const std::string& s, int start_pos, int count, bool failIfLeftoverChars = true);

// Parse int located at 'c' until non-digit char or 'stop_c' reached. (**)
int read_int(std::string::const_iterator& c, std::string::const_iterator stop_c);

// Parse int located at 'c' until non-digit char or 0-char reached. 'c' must point to char in a C-string (zero-terminated). (**)
int read_int(const char*& c);

// (**) I know, there is probably a library function that does precisely this, but it was quick to write...
double str2double(const std::string& s, bool failIfLeftoverChars = true);


/**
  String helpers.
**/

typedef std::vector<std::string> StringVector;

// Following trim functions taken from http://www.codeguru.com/forum/archive/index.php/t-271713.html
inline void trim_right(std::string &source, const std::string& t)
{
	source.erase( source.find_last_not_of(t) + 1 );
}

inline void trim_left(std::string &source, const std::string& t)
{
	source.erase(0, source.find_first_not_of(t) );
}

inline std::string uppercase(const std::string& s)
{
	std::string result = s;
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);

	return result;
}

inline std::string lowercase(const std::string& s)
{
	std::string result = s;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);

	return result;
}

/**
  Run-time assertion helpers.
**/

template <class Exception>
	void check(bool condition, const char* err_msg = "Check assertion failed.")
{
	if (!condition)
		throw Exception(err_msg);
}

template <class Exception>
	void check_range(int value, int min, int max, const char* err_msg = "<Unknown>")
{
	if (min > value || value > max)
		throw Exception(
		  std::string(std::string(err_msg) + " out of range (" + int2str(value) + "). " +
		  "Must be " + int2str(min) + ".." + int2str(max) + ".").c_str()
		);
}

/**
  Utility functions.
**/

// Standard strncpy with Windows VC++ strncpy_s() semantics on Linux.
inline void safe_strcpy(char* dst, size_t dst_size, const char* src, size_t char_count)
{
#if defined(ON_LINUX) || defined(ON_SOLARIS)
	if (char_count > dst_size)
		throw std::runtime_error (std::string("safe_strcpy: Trying to copy ") + int2str(char_count) + " bytes into a buffer of size " + int2str(dst_size) + ".");
	::strncpy(dst, src, char_count);
#elif defined(ON_WINDOWS)
	strncpy_s(dst, dst_size, src, char_count);
#else
	Unsupported_platform
#endif
}

// The GCC compiler returns mangled names from typeid().name(). This function fixes it.
/** See http://groups.google.dk/group/gnu.g++.help/browse_thread/thread/ec73e8a6d9b534c8/982c86fc810f4208?lnk=st&q=gcc+demangle+typeid()&rnum=1&hl=da#982c86fc810f4208 **/
const std::string demangled_class_name(std::string class_name);

// Platform-agnostic version of std::getline(), handling Windows-, Mac-, or UNIX-like line feeds.
void rinchi_getline(std::istream& input, std::string& s);

} // end of namespace

#endif
