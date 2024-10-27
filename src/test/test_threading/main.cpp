#include <iostream>
#include <thread>
#include <atomic>

#include "rinchi_reaction.h"
#include "inchi_generator.h"
#include "mdl_molfile_reader.h"

// Include file defines data in the "example_data" table.
// Also defines NUM_EXAMPLES int constant.
#include "example_data.cpp.inc"


// Start flag for threads.
std::atomic_bool start_flag;

// The more threads, the more likely it is to discover errors. But just
// two threads are usually enough when at least 50 loops are run.
const int NUM_THREADS = 4;
const int NUM_LOOPS = 1000;

void run_examples(){
    std::cout << "Thread " << std::this_thread::get_id() << " created." << std::endl;

    try {
        rinchi::MdlMolfileReader mr;
        rinchi::ReactionComponent rc[NUM_EXAMPLES];

        // Read in molfiles before starting test. We are stress-testing
        // concurrency of the InChI library, not of the MDL molfile reader.
        for (int i = 0; i < NUM_EXAMPLES; ++i) {
            std::stringstream ss (example_table[i].molfile);
            mr.read_molecule(ss, rc[i]);
        }

        // Primitive spin lock on start flag. For some tests
        // you can cause a crash earlier if all threads start
        // as concurrently as possible.
        while(!start_flag.load());

        rinchi::InChIGenerator ig;
        std::string inchi_string;
        std::string aux_info;
        for (int loops = 0; loops < NUM_LOOPS; ++loops) {
            for (int i = 0; i < NUM_EXAMPLES; ++i) {
                ig.calculate_inchi(rc[i].inchi_input(), inchi_string, aux_info);
                if (inchi_string != example_table[i].expected_inchi_string)
                    std::cout << "DATA ERROR: " << (i + 1) << ": " << inchi_string << std::endl;
            }
        }
        std::cout << "Thread " << std::this_thread::get_id() << " successfully done." << std::endl;
	} catch (std::exception& e) {
		std::cerr << "ERROR: Thread " << std::this_thread::get_id() << ": " << rinchi::demangled_class_name(typeid(e).name()) << ": " << e.what() << std::endl;
	}
}

int main()
{
    start_flag.store(false);
    std::vector<std::thread> threads;

    for(int i = 0; i < NUM_THREADS; ++i) {
        threads.push_back(std::thread(run_examples));
    }
    std::cout << "Threads created." << std::endl;

    // Let worker threads initialize and reach spin lock loop.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // Release worker threads.
    start_flag.store(true);

    for(auto& thread : threads) {
        thread.join();
    }

    return 0;
}
