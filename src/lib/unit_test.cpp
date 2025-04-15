#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <unit_test.h>

#include <math.h>
#include <typeinfo>
#include <fstream>

#include <rinchi_utils.h>

namespace rinchi {
	namespace unit_test {

namespace {

	void log_stats(std::ostream& output_stream, int cases_total, int cases_completed, const std::vector<std::string>& err_msg_list)
	{
		output_stream << "================================\n";
		output_stream << "         TEST STATS\n";
		output_stream << "================================\n";
		output_stream << "Cases in total:\t";
		output_stream << cases_total << "\n";
		output_stream << "Successes:\t";
		output_stream << cases_completed << "\n";
		output_stream << "Failures:\t";
		output_stream << err_msg_list.size() << "\n";
		output_stream << "Overall result:\t";
		output_stream << ceil( cases_completed * 100 / double(cases_completed + err_msg_list.size()) ) << " % completed.\n";
		output_stream << "================================";
		output_stream << std::endl;
		if (err_msg_list.size() > 0) {
			output_stream << std::endl;
			output_stream << "Failure summary:" << std::endl;
			output_stream << "----------------" << std::endl;
			for (std::vector<std::string>::const_iterator s = err_msg_list.begin(); s != err_msg_list.end(); s++)
				output_stream << *s << std::endl << std::endl;
		}
	}

}

void TestStats::show_stats() const
{
	log_stats(std::cout, cases_total, cases_completed, err_msg_list);
	std::ofstream log_file ("./test_stats.log");
	log_stats(log_file, cases_total, cases_completed, err_msg_list);
}

TestStats _GlobalTestStats = TestStats();

const TestStats& GlobalTestStats()
{
	return _GlobalTestStats;
}

// RAII construct to ensure call of a function when the class instance goes out of scope.
class GuaranteedFunctionCall {
	private:
		TestFunction _f;

	public:
		explicit GuaranteedFunctionCall(TestFunction f): _f(f) { }
		~GuaranteedFunctionCall() { _f(); }
};

void TestCase::register_test(const std::string& a_name, TestFunction a_test) {
	_all_test_names.push_back(a_name);
	_all_tests.push_back(a_test);
}

void TestCase::log_failure(const std::string& err_msg)
{
	_GlobalTestStats.err_msg_list.push_back(demangled_class_name(typeid(*this).name()) + "::" + err_msg);

	std::cout << std::endl;
	std::cerr << std::endl << err_msg << std::endl << std::endl << std::endl;
}

void TestCase::chback_dir()
{
	for (unsigned int i = 0; i < m_path_segments.size(); i++)
		rinchi::chdir("..");
}

void TestCase::run_tests() {
	int failure_count = 0;
	int success_count = 0;

	std::string class_name = demangled_class_name(typeid(*this).name());
	std::cout << class_name << ": " << _all_tests.size()
			<< " tests." << std::endl;

	for (unsigned int i = 0; i < _all_tests.size(); i++) {
		TestFunction the_test = _all_tests[i];
		std::string test_name = _all_test_names[i];
		std::cout << "  " << test_name;
		_GlobalTestStats.cases_total++;
		try {
			// Go to test subdirectory, if defined.
			for (std::vector<std::string>::const_iterator dir = m_path_segments.begin(); dir != m_path_segments.end(); dir++)
				rinchi::chdir(dir->c_str());

			setup();
			// Ensure that tear_down is always called.
			// Fails in Visual C8: GuaranteedFunctionCall cleanup = GuaranteedFunctionCall( boost::bind(&TestCase::tear_down, this) );
			GuaranteedFunctionCall cleanup( boost::bind(&TestCase::tear_down, this) );
			GuaranteedFunctionCall chbackdir( boost::bind(&TestCase::chback_dir, this) );

			the_test();
			success_count++;
			_GlobalTestStats.cases_completed++;
			std::cout << " - OK" << std::endl;

		}
		catch (TestFailure& e) {
			log_failure( test_name + ": TEST FAILURE: " + e.what() );
			failure_count++;
		}
		catch (std::exception& e) {
			log_failure( test_name + ": " + demangled_class_name(typeid(e).name()) + " caught: " + e.what() );
			failure_count++;
		}
		catch (...) {
			std::cout << std::endl;
			std::cerr << "OUCH! Unexpected exception occurred." << std::endl;
			throw;
		}
	}

	std::cout << class_name;
	if (failure_count == 0) {
		 std::cout << " all tests OK.";
	}
	else {
		std::cout << " " << round( failure_count * 100 / double(success_count + failure_count) )
				<< "% of tests FAILED.";
	}
	std::cout << std::endl << std::endl;
}

void check_errmsg_has_substr(const std::string& err_msg, const std::string& sub_str, const std::string& additional_context)
{
	if (err_msg.find(sub_str) == std::string::npos)
		throw TestFailure("unexpected error: " + err_msg + "; expected something containing '" + sub_str + "' [Context: " + additional_context + "].");
}

} } // End of namespaces.
