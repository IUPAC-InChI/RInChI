#ifndef IUPAC_RINCHI_UNITTEST_HEADER_GUARD
#define IUPAC_RINCHI_UNITTEST_HEADER_GUARD

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

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace rinchi {
	namespace unit_test {

class TestFailure: public std::runtime_error {
	public:
		explicit TestFailure(const std::string& what) : std::runtime_error(what) { }
};

typedef boost::function<void ()> TestFunction;

class TestStats {

public:
	int cases_total;
	int cases_completed;
	// Number of cases failed can be seen from err_msg_list.size().
	std::vector<std::string> err_msg_list;

	TestStats(): cases_total(0), cases_completed(0) { }

	void show_stats() const;

};

const TestStats& GlobalTestStats();

// Derive your own test cases from TestCase.
class TestCase
{
	typedef std::vector<TestFunction> TestList;

	private:
		std::vector<std::string> _all_test_names;
		TestList _all_tests;

		// The working directory that the test should be executed in. Multi-level subdirectories should be
		// registered as multiple separate segments, e.g. a test running in "data/test/subdir" should be
		// defined as ' goto_subdir("data"); goto_subdir("test"); goto_subdir("subdir"); '.
		std::vector<std::string> m_path_segments;

		void log_failure(const std::string& err_msg);
		void chback_dir();
	protected:
		void goto_subdir(const std::string& segment) { m_path_segments.push_back(segment); }

		virtual void setup() {}
		virtual void tear_down() {}

		// Create a_test with boost::bind() like this; boost::bind(&MyClass::MyFunction, this).
		void register_test(const std::string& a_name, TestFunction a_test);

	public:
		virtual ~TestCase() {}

		void run_tests();

};

// Use this macro to easily register a test function.
#define REGISTER_TEST(class_name, test_name) register_test( #test_name, boost::bind(&class_name::test_name, this) )

template <class Value, class Expected>
	void check_is_equal(Value value, Expected expected, std::string errmsg = "")
{
	if (value != expected) {
		std::ostringstream error_stream;
		error_stream << errmsg << ".\n" <<
			"check_is_equal() expected" <<
			"\n  '" << expected << "'\n" <<
			"but got" <<
			"\n  '" << value << "'.";
#ifdef UNIT_TEST_PASSTHROUGH
		// During development of a test case it may be useful to allow the test to pass-through all tests/checks
		// of the test case instead of bailing out on the first error.
		std::cout << std::endl << "**TEST FAILURE** :: " << error_stream.str() << std::endl;
#else
		throw TestFailure( error_stream.str() );
#endif
	}
}

void check_errmsg_has_substr(const std::string& err_msg, const std::string& sub_str);

} } // End of namespaces.

#endif
