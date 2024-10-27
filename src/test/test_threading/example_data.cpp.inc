// Include file for main.cpp with molecule example data.

struct InChIExample {
    std::string molfile;
    std::string expected_inchi_string;

    InChIExample(std::string a_molfile, std::string an_inchi_string)
        : molfile(a_molfile), expected_inchi_string(an_inchi_string)
    {}
};

const int NUM_EXAMPLES = 3;
typedef InChIExample ExampleTable[NUM_EXAMPLES];

const ExampleTable example_table = {
    // Molfiles from the R005.rxn file.
    InChIExample(
    "\n"
    "\n"
    "\n"
    "  6  5  0  0  0  0  0  0  0  0999 V2000\n"
    "   -0.8250   -0.7557    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "   -0.4125   -0.0412    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "    0.4125   -0.0412    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "    0.8250    0.6733    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "   -0.6260    0.7557    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "    0.8250   -0.7557    0.0000 Br  0  0  0  0  0  0  0  0  0  0  0  0\n"
    "  1  2  1  0        0\n"
    "  2  3  1  0        0\n"
    "  3  4  1  0        0\n"
    "  2  5  1  6        0\n"
    "  3  6  1  1        0\n"
    "M  END\n", "InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1"),
    InChIExample(
    "\n"
    "\n"
    "\n"
    "  2  1  0  0  0  0  0  0  0  0999 V2000\n"
    "   -0.4125    0.0000    0.0000 Na  0  0  0  0  0  0  0  0  0  0  0  0\n"
    "    0.4125    0.0000    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "  1  2  1  0        0\n"
    "M  END\n", "InChI=1S/Na.H2O/h;1H2/q+1;/p-1"),
    InChIExample(
    "\n"
    "\n"
    "\n"
    "  5  5  0  0  0  0  0  0  0  0999 V2000\n"
    "   -1.1270   -0.5635    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "   -0.4125   -0.1510    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "    0.4125   -0.1510    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "    1.1270   -0.5635    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "    0.0000    0.5635    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
    "  1  2  1  6        0\n"
    "  2  3  1  0        0\n"
    "  3  4  1  1        0\n"
    "  2  5  1  0        0\n"
    "  3  5  1  0        0\n"
    "M  END\n", "InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1")
};
