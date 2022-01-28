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

#include <sstream>

#include "rinchi_utils.h"

#ifdef ON_WINDOWS
	// For _chdir().
	#include <direct.h>
#elif defined(ON_LINUX) || defined(ON_SOLARIS)
	#include <unistd.h>
#endif

#ifdef __GNUC__
	// For demangling class names on GCC.
	#include <cxxabi.h>
	#include <cstdlib>
#endif

namespace rinchi {

void chdir(const char* path) {
#ifdef ON_WINDOWS
	if (_chdir(path))
#elif defined(ON_LINUX) || defined(ON_SOLARIS)
	if (::chdir(path))
#else
	Unsupported_platform
#endif
		throw std::runtime_error("chdir() to " + std::string(path) + " failed.");
}

const std::string demangled_class_name(std::string class_name)
{
#ifdef __GNUC__
	// Demangle typeid to get real class name.
	int status;
	size_t length;
	char* real_name = abi::__cxa_demangle(class_name.c_str(), 0, &length, &status);
	if (real_name)
	  class_name = real_name;
	std::free(real_name);
#endif
	return class_name;
}

const char CR = 13;
const char LF = 10;

// Remove CR from Windows-type lines - will be present if we read Windows-linefeed files on UNIX.
inline void remove_prefixed_and_trailing_cr(std::string& line)
{
	if (line.empty())
		return;
	if ( line[line.length() - 1] == CR )
		line.erase(line.length() - 1, 1);
	if (line.empty())
		return;
	if ( line[0] == CR )
		line.erase(0, 1);
}

void rinchi_getline(std::istream& input, std::string& s)
{
	std::getline(input, s);
	remove_prefixed_and_trailing_cr(s);
}

std::string int2str(int i)
{
	std::ostringstream ost;
	ost << i;
	return ost.str();
}

// Implementation from the C++ FAQ at http://www.parashift.com/c++-faq-lite/ section [39.2].
double str2double(const std::string& s, bool failIfLeftoverChars)
{
	std::istringstream i(s);
	double x;
	char c;
	if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
		throw BadConversion("str2double(\"" + s + "\")");
	return x;
}

// As above but for int-s.
int str2int(const std::string& s, bool failIfLeftoverChars)
{
	std::istringstream i(s);
	int x;
	char c;
	if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
		throw BadConversion("str2int: \"" + s + "\" is not a valid integer.");
	return x;
}

int str2int(const std::string& s, int start_pos, int count, bool failIfLeftoverChars)
{
	return str2int(s.substr(start_pos, count), failIfLeftoverChars);
}

int read_int(std::string::const_iterator& c, std::string::const_iterator stop_c)
{
	int result = 0;
	bool is_negative = false;

	while (c != stop_c && isspace(*c))
		c++;
	std::string::const_iterator start = c;
	if (c != stop_c && *c == '+')
		c++;
	else if (c != stop_c && *c == '-') {
		is_negative = true;  c++;
	}

	while (c != stop_c && isdigit(*c)) {
		result = result * 10 + *c - '0';
		c++;
	}
	if (start == c)
		throw BadConversion("Invalid integer. Non-digit or end-of-string reached.");

	if (is_negative)
		return -result;
	else
		return result;
}

int read_int(const char*& c)
{
	int result = 0;
	bool is_negative = false;

	while (isspace(*c))
		c++;
	const char* start = c;
	if (*c == '+')
		c++;
	else if (*c == '-') {
		is_negative = true;  c++;
	}

	while (isdigit(*c)) {
		result = result * 10 + *c - '0';
		c++;
	}
	if (start == c)
		throw BadConversion("Invalid integer. Non-digit or end-of-string reached.");

	if (is_negative)
		return -result;
	else
		return result;
}

} // end of namespace
