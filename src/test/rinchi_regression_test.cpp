#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

#include "rinchi_consts.h"
#include "rinchi_utils.h"
#include "rinchi_reaction.h"
#include "mdl_rxnfile_reader.h"

namespace {

std::string normalize_expected(const std::string& value) {
    std::string normalized = value;

    // std::getline strips '\n' but preserves '\r' on CRLF files.
    while (!normalized.empty() && normalized.back() == '\r') {
        normalized.pop_back();
    }

    const std::string from_header = "RInChI=0.02.1S/";
    const std::string to_header = "RInChI=1.00.1S/";
    const std::string from_aux_header = "RAuxInfo=0.02.1/";
    const std::string to_aux_header = "RAuxInfo=1.00.1/";

    std::size_t pos = 0;
    while ((pos = normalized.find(from_header, pos)) != std::string::npos) {
        normalized.replace(pos, from_header.size(), to_header);
        pos += to_header.size();
    }
    pos = 0;
    while ((pos = normalized.find(from_aux_header, pos)) != std::string::npos) {
        normalized.replace(pos, from_aux_header.size(), to_aux_header);
        pos += to_aux_header.size();
    }

    pos = 0;
    while ((pos = normalized.find("///", pos)) != std::string::npos) {
        normalized.replace(pos, 3, "<>");
        pos += 2;
    }
    pos = 0;
    while ((pos = normalized.find("//", pos)) != std::string::npos) {
        normalized.replace(pos, 2, "!");
        pos += 1;
    }
    return normalized;
}

std::string read_first_line(const std::filesystem::path& path) {
    std::ifstream input(path);
    std::string line;
    std::getline(input, line);
    return line;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: rinchi-regression-test <directory>" << std::endl;
        return 1;
    }

    std::filesystem::path input_dir = std::filesystem::absolute(argv[1]);
    if (!std::filesystem::exists(input_dir) || !std::filesystem::is_directory(input_dir)) {
        std::cerr << "Input directory does not exist: " << input_dir << std::endl;
        return 1;
    }

    std::time_t now = std::time(nullptr);
    std::tm tm_buffer{};
    localtime_r(&now, &tm_buffer);

    std::ostringstream timestamp_stream;
    timestamp_stream << std::put_time(&tm_buffer, "%Y%m%d_%H%M%S");
    std::filesystem::path log_path = input_dir / ("rinchi_compare_" + timestamp_stream.str() + ".log");

    std::ofstream log(log_path);
    if (!log) {
        std::cerr << "Unable to open log file for writing: " << log_path << std::endl;
        return 1;
    }

    log << "RInChI regression tests\n";
    log << "Run on " << std::put_time(&tm_buffer, "%Y-%m-%dT%H:%M:%SZ") << "\n";
    log << "\n";

    std::vector<std::filesystem::path> rxn_files;
    for (const auto& entry : std::filesystem::directory_iterator(input_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".rxn") {
            rxn_files.push_back(entry.path());
        }
    }
    std::sort(rxn_files.begin(), rxn_files.end());

    int passed = 0;
    int failed = 0;
    int missing = 0;
    int total = 0;

    for (const auto& rxn_path : rxn_files) {
        ++total;
        const std::string rxn_name = rxn_path.filename().string();
        const std::filesystem::path expected_path = rxn_path.string() + ".rinchi_strings.txt";

        if (!std::filesystem::exists(expected_path)) {
            ++missing;
            log << "[MISSING] " << rxn_name << "\n";
            log << "  Expected file missing: " << expected_path << "\n\n";
            continue;
        }

        try {
            // Read the RXN file using the rinchi library
            rinchi::Reaction rxn;
            rinchi::MdlRxnfileReader rxn_reader;
            rxn_reader.read_reaction(rxn_path.string(), rxn);

            // Get computed RInChI and RAuxInfo
            std::string computed_rinchi = rxn.rinchi_string();
            std::string computed_auxinfo = rxn.rinchi_auxinfo();

            // Read expected values
            std::ifstream expected_file(expected_path);
            std::string expected_rinchi;
            std::string expected_auxinfo;
            if (std::getline(expected_file, expected_rinchi) && std::getline(expected_file, expected_auxinfo)) {
                expected_rinchi = normalize_expected(expected_rinchi);
                expected_auxinfo = normalize_expected(expected_auxinfo);

                if (computed_rinchi == expected_rinchi && computed_auxinfo == expected_auxinfo) {
                    ++passed;
                    log << "[PASS] " << rxn_name << "\n";
                } else {
                    ++failed;
                    log << "[FAIL] " << rxn_name << "\n";
                    log << "  Expected RInChI: " << expected_rinchi << "\n";
                    log << "  Computed RInChI: " << computed_rinchi << "\n";
                    log << "  Expected RAuxInfo: " << expected_auxinfo << "\n";
                    log << "  Computed RAuxInfo: " << computed_auxinfo << "\n\n";
                }
            } else {
                ++failed;
                log << "[ERROR] " << rxn_name << "\n";
                log << "  Unable to read expected values from " << expected_path << "\n\n";
            }
        } catch (const std::exception& e) {
            ++failed;
            log << "[ERROR] " << rxn_name << "\n";
            log << "  Exception: " << e.what() << "\n\n";
        }
    }

    log << "Summary\n";
    log << "  Passed: " << passed << "\n";
    log << "  Failed: " << failed << "\n";
    log << "  Missing expectation files: " << missing << "\n";
    log << "  Total: " << total << "\n";
    log.flush();

    std::cout << "Wrote log to " << log_path << std::endl;
    std::cout << "Summary: passed=" << passed << " failed=" << failed << " missing=" << missing << " total=" << total << std::endl;

    return (failed > 0 || missing > 0) ? 1 : 0;
}
