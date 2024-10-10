#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "mdl_molfile.h"
#include "mdl_molfile_reader.h"

#include "rinchi_utils.h"
#include "inchi_api_intf.h"

using namespace std;

namespace rinchi {

namespace {

	typedef map<inchi_Atom*, int> ValenceMap;

	void check(bool condition, const std::string& err_msg)
	{
		rinchi::check<MdlMolfileReaderError>(condition, err_msg.c_str());
	}

	void check_range(int value, int min, int max, const std::string& err_msg)
	{
		rinchi::check_range<MdlMolfileReaderError>(value, min, max, err_msg.c_str());
	}

	typedef std::vector<inchi_Atom*> AtomPointerList;

	void read_V2000_charges(const string& current_line, AtomPointerList& atom_block_charged_atoms, ReactionComponent& result)
	{
		/**
			M  CHGnn8 aaa vvv ...
			aaa:	Atom number
			vvv:	-15 to +15. Default of 0 = uncharged atom. When present, this property supersedes
			all charge and radical values in the atom block, forcing a 0 charge on all atoms not
			listed in an M CHG or M RAD line.
			[JHJe note: Must mean that all radical values should be reset too, to be consistent
			            with the description of "M  RAD" lines.]
		**/
		// Clear any charge and radical codes already set in atom block - "M  CHG" lines take precedence according to ctfile.pdf.
		for (AtomPointerList::iterator atom = atom_block_charged_atoms.begin(); atom != atom_block_charged_atoms.end(); atom++) {
			(*atom)->charge  = 0;
			(*atom)->radical = 0;
		}
		atom_block_charged_atoms.clear();

		int num_atoms = str2int( current_line.substr(6, 3) );
		check_range(num_atoms, 1, 8, "Charge line item count");
		for (int i = 0; i < num_atoms; i++) {
			int atom_index = str2int( current_line.substr(10 + i * 8, 3) );
			check_range(atom_index, 1, (int) result.inchi_input().num_atoms, "Charge line; atom number");
			atom_index += -1;
			int charge = str2int( current_line.substr(14 + i * 8, 3) );
			check_range(charge, -15, 15, "Charge line: charge value");

			result.inchi_input().atom[atom_index].charge = charge;
		}
	}


	void read_V2000_radicals(const string& current_line, AtomPointerList& atom_block_radical_atoms, ReactionComponent& result)
	{
		/**
			M  RADnn8 aaa vvv ...
			aaa:	Atom number
			vvv:	Default of 0 = no radical, 1 = singlet (:), 2 = doublet ( . or ^), 3 = triplet (^^).
			When present, this property supersedes all charge and radical values in the atom block,
			forcing a 0 (zero) charge and radical on all atoms not listed in an M CHG or M RAD line.
		**/
		// Clear any charge and radical codes already set in atom block - "M  CHG" lines take precedence according to ctfile.pdf.
		for (AtomPointerList::iterator atom = atom_block_radical_atoms.begin(); atom != atom_block_radical_atoms.end(); atom++) {
			(*atom)->charge  = 0;
			(*atom)->radical = 0;
		}
		atom_block_radical_atoms.clear();

		int num_atoms = str2int( current_line.substr(6, 3) );
		check_range(num_atoms, 1, 8, "Radical line item count");
		for (int i = 0; i < num_atoms; i++) {
			int atom_index = str2int( current_line.substr(10 + i * 8, 3) );
			check_range(atom_index, 1, (int) result.inchi_input().num_atoms, "Radical line; atom number");
			atom_index += -1;
			int radical = str2int( current_line.substr(14 + i * 8, 3) );
			check_range(radical, 0, 3, "Radical line: charge value");
			/**
				The values of V2000 radicals match the InChI definition of radicals:

				typedef enum tagINCHIRadical {
				   INCHI_RADICAL_NONE    = 0,
				   INCHI_RADICAL_SINGLET = 1,
				   INCHI_RADICAL_DOUBLET = 2,
				   INCHI_RADICAL_TRIPLET = 3
				} inchi_Radical;
			**/
			result.inchi_input().atom[atom_index].radical = radical;
		}
	}

	void read_V2000_isotopes(const string& current_line, AtomPointerList& atom_block_mass_diffs, ReactionComponent& result)
	{
		/**
			M ISOnn8 aaa vvv ...
			aaa:	Atom number
			vvv:	Absolute mass of the atom isotope as a positive integer. When present,
			this property supersedes all isotope values in the atom block. Default (no entry)
			means natural abundance. The difference between this absolute mass value and the
			natural abundance value specified in the PTABLE.DAT file must be within the range
			of -18 to +12.
		**/
		// Clear any mass diffs already set in atom block - "M  ISO" lines take precedence according to ctfile.pdf.
		for (AtomPointerList::iterator iso_atom = atom_block_mass_diffs.begin(); iso_atom != atom_block_mass_diffs.end(); iso_atom++)
			(*iso_atom)->isotopic_mass = 0;
		atom_block_mass_diffs.clear();

		int num_atoms = str2int( current_line.substr(6, 3) );
		check_range(num_atoms, 1, 8, "ISOtope line item count");
		for (int i = 0; i < num_atoms; i++) {
			int atom_index = str2int( current_line.substr(10 + i * 8, 3) );
			check_range(atom_index, 1, (int) result.inchi_input().num_atoms, "ISOtope line; atom number");
			atom_index += -1;
			int abs_mass = str2int( current_line.substr(14 + i * 8, 3) );
			// TODO: Check that abundance_mass - abs_mass in [-18; +12].
			// check_range(abundance_mass - abs_mass, -18.0, +12.0, "ISOtope line: abs. mass value");

			result.inchi_input().atom[atom_index].isotopic_mass = abs_mass;
		}
	}

	BondStereo V2000_stereo_code_to_BondStereo(int stereo)
	{
		switch (stereo) {
			case 0: return RINCHI_BOND_STEREO_NONE;
			case 1: return RINCHI_BOND_STEREO_UP;
			case 6: return RINCHI_BOND_STEREO_DOWN;
			case 3: return RINCHI_BOND_STEREO_DOUBLE_EITHER;
			case 4: return RINCHI_BOND_STEREO_EITHER;
			default: throw MdlMolfileReaderError("Bond stereo flag out of range. Only 0 (NONE), 1 (UP), 6 (DOWN), 3 (DOUBLE_EITHER), or 4 (EITHER) is allowed.");
		}
	}

	void ignore_tag() { }

	void set_inchi_atom_valence(inchi_Atom* atom, int valence, RInChILogger& warning_logger)
	{
		int used_valences = 0;
		for (int i = 0; i < atom->num_bonds; i++) {
			int bond_cardinality = atom->bond_type[i];
			if (bond_cardinality > 3)
				throw MdlMolfileReaderError("Refusing to guess valence of alternating bond (InChI bond type 4 or greater).");
			used_valences += bond_cardinality;
		}
		// InChI accepts exceeded valences, so just log a warning. In case of exceeded valences, force
		// number of hydrogens to be zero.
		int remaining_valences = valence - used_valences;
		if (remaining_valences < 0) {
			warning_logger.log(std::string("Valence exceeded on ") + atom->elname + " atom: " + int2str(used_valences) + " valences used, but molfile wants to restrict the valence to " + int2str(valence) + ".");
			remaining_valences = 0;
		}
		atom->num_iso_H[0] = remaining_valences;
	}

	bool is_special_atom(const std::string& atom_symbol)
	{
		return (atom_symbol == "A" || atom_symbol == "X" || atom_symbol == "R#" || atom_symbol == "R" || atom_symbol == "*");
	}
}

void MdlMolfileReader::read_V2000_molecule_from_stream(int atom_count, int bond_count, bool is_chiral, bool has_version_stamp, int prop_line_count, std::istream& input_stream, ReactionComponent& result)
{
	// Molecules that only consist of a single special atom are considered equivalent to No-Structures.
	if (atom_count == 1) {
		ios::off_type saved_pos = input_stream.tellg();
		get_next_line(input_stream);
		std::string atom_symbol = m_current_line.substr(31, 3);
		trim_right(atom_symbol, " ");
		if (is_special_atom(atom_symbol)) {
			result.initialize(0, is_chiral);
			while (m_current_line != MDL_TAG_MOLFILE_END)
				get_next_line(input_stream);
			return;
		}
		else
			input_stream.seekg(saved_pos);
	}

	result.initialize(atom_count, is_chiral);

	// Read atoms.
	Coordinate x;
	Coordinate y;
	Coordinate z;
	std::string atom_symbol;
	// Keep track of inline charged or radical atoms - they must be reset if CHG or RAD property lines are present.
	AtomPointerList atoms_with_charges_or_radicals;
	// Keep track of atoms with inline mass differences - they must be reset if ISO property lines are present.
	AtomPointerList atoms_with_mass_diffs;
	// Keep track of atoms with special valences. These atoms will have their valences fixed after full connection table has been read.
	ValenceMap atoms_with_special_valences;
	try {
		for (int i = 0; i < atom_count; i++) {
			get_next_line(input_stream);
			check(m_current_line.length() >= 39, "Atom lines are expected to be at least 39 characters long each.");

			x = str2double(m_current_line.substr(0, 10));
			y = str2double(m_current_line.substr(10, 10));
			z = str2double(m_current_line.substr(20, 10));
			atom_symbol = m_current_line.substr(31, 3);
			trim_right(atom_symbol, " ");

			int charge  = 0;
			int radical = 0;
			char charge_code = m_current_line[38];
			if (charge_code != '0') {
				check_range(charge_code, '1', '7', "Charge code");
				// charges according to ctfiles.pdf:
				// 0 = uncharged or value other than these, 1 = +3, 2 = +2, 3 = +1, 4 = doublet radical, 5 = -1, 6 = -2, 7 = -3
				if (charge_code == '4')
					radical = 2;
				else
					charge = '4' - charge_code;

				atoms_with_charges_or_radicals.push_back( &(result.inchi_input().atom[i]) );
			}

			int isotopic_mass = 0;
			std::string massdiff_code = m_current_line.substr(34, 2);
			if (massdiff_code != " 0") {
				isotopic_mass = str2int(massdiff_code);
				check_range(isotopic_mass, -3, 4, "Inline mass difference");
				// mass diff. according to ctfiles.pdf:
				// -3, -2, -1, 0, 1, 2, 3, 4 (0 if value beyond these limits)
				atoms_with_mass_diffs.push_back( &(result.inchi_input().atom[i]) );
				// Indicate delta mass value.
				isotopic_mass += ISOTOPIC_SHIFT_FLAG;
			}

			int valence = 0;
			std::string valence_code;
			// Set default valence in case of SciTegic abridged lines.
			if (m_current_line.length() >= 51)
				valence_code = m_current_line.substr(48, 3);
			else
				valence_code = "  0";
			if (valence_code != "  0") {
				valence = str2int(valence_code);
#ifdef IN_RINCHI_TEST_SUITE
				// The InChI test suite has a molfile with a valence code of 20. This is clearly out of range, but
				// is apparently accepted by InChI. We need to allow it for regression tests to complete even though
				// it violates the MDL molfile spec.
				check_range(valence, 0, 20, "Valence code");
#else
				// But under normal circumstances we will enforce the MDL molfile spec.
				// Valence code according to ctfiles.pdf:
				// valence 0 = no marking (default) (1 to 14) = (1 to 14) 15 = zero valence
				check_range(valence, 0, 15, "Valence code");
#endif
				if (valence != 0) {
					if (valence == 15)
						valence = 0;
					atoms_with_special_valences.insert(std::make_pair( &(result.inchi_input().atom[i]), valence ));
				}
			}

			inchi_set_atom(result.inchi_input(), i, atom_symbol, x, y, z, charge, radical, isotopic_mass);
		}
	}
	catch (exception& e) {
		check(false, "Invalid atom (" + string(e.what()) + ")");
	}

	// Read bonds.
	int from_number;
	int to_number;
	int bond_cardinality;
	int stereo_code;
	try {
		for (int i = 0; i < bond_count; i++) {
			get_next_line(input_stream);
			from_number = str2int(m_current_line, 0, 3);
			to_number   = str2int(m_current_line, 3, 3);
			bond_cardinality = str2int(m_current_line, 6, 3);
			stereo_code      = str2int(m_current_line, 9, 3);
			check_range(bond_cardinality, 1, 8, "Bond type");
			// We will allow a bond cardinality of 4 even though that is also an SSS query feature
			// (aromatic bond). InChI accepts it as a valid bond type, probably because ChemDraw will
			// often produce molfiles that use the aromatic bond in rings instead of storing rings in
			// kekulized form.
			if (bond_cardinality >= 5)
				throw MdlMolfileReaderError ("SSS query bonds are not allowed.");

			inchi_add_bond(result.inchi_input(), from_number - 1, to_number - 1, bond_cardinality, V2000_stereo_code_to_BondStereo(stereo_code));
		}
	}
	catch (exception& e) {
		check(false, "Invalid bond (" + string(e.what()) + ")");
	}

	// Read property lines.
	int prop_line_read_count = 0;
	std::string property_tag;
	while (!input_stream.eof()) {
		if (!has_version_stamp && prop_line_read_count >= prop_line_count)
			break;
		get_next_line(input_stream);
		prop_line_read_count++;

		property_tag = m_current_line.substr(0, 6);
		if (property_tag == MDL_TAG_CHARGE)
			read_V2000_charges(m_current_line, atoms_with_charges_or_radicals, result);
		if (property_tag == MDL_TAG_RADICAL)
			read_V2000_radicals(m_current_line, atoms_with_charges_or_radicals, result);
		else if (property_tag == MDL_TAG_ISOTOPE)
			read_V2000_isotopes(m_current_line, atoms_with_mass_diffs, result);
		// The old-style properties atom alias and group abbreviation require 
		// us to skip the following line that holds the data for the tags.
		else if (
			property_tag.substr(0, 3) == MDL_TAG_ATOM_ALIAS
			|| property_tag.substr(0, 3) == MDL_TAG_GROUP_ABBREVIATION
		) {
			ignore_tag();
			get_next_line(input_stream);
			prop_line_read_count++;
		}
		// End of molfile ?
		else if (m_current_line == MDL_TAG_MOLFILE_END)
			break;
		// Other property lines will just be ignored.
		else
			ignore_tag();
	}

	if (has_version_stamp && m_current_line != MDL_TAG_MOLFILE_END)
		check(false, "Missing '" + MDL_TAG_MOLFILE_END + "' at end - instead found '" + m_current_line + "'");

	// Set any fixed valences.
	for (ValenceMap::const_iterator val = atoms_with_special_valences.begin(); val != atoms_with_special_valences.end(); val++) {
		inchi_Atom* atom = val->first;
		int valence = val->second;
		set_inchi_atom_valence(atom, valence, m_warning_logger);
	}
}

namespace {

	void strip_V30_prefix(string& s)
	{
		if (s.substr(0, MDL_TAG_V30_LINE.length()) != MDL_TAG_V30_LINE)
			throw MdlMolfileReaderError ("Invalid V3000 line - must start with '" + MDL_TAG_V30_LINE + "'.");
		s.erase(0, MDL_TAG_V30_LINE.length());
	}

	// 'key_test' should include the equal sign, e.g. "RAD=".
	bool parse_V3000_property(const std::string& key_value, const std::string& key_test, int& value)
	{
		if (key_value.substr(0, key_test.length()) == key_test) {
			const char* c = key_value.c_str() + key_test.length();
			value = read_int(c);
			return true;
		}
		else
			return false;
	}

	BondStereo V3000_stereo_code_to_BondStereo(int stereo)
	{
		switch (stereo) {
			case 0: return RINCHI_BOND_STEREO_NONE;
			case 1: return RINCHI_BOND_STEREO_UP;
			case 2: return RINCHI_BOND_STEREO_EITHER;
			case 3: return RINCHI_BOND_STEREO_DOWN;
			default: throw MdlMolfileReaderError ("Unsupported V3000 stereo code (" + int2str(stereo) + ").");
		}
	}

}

void MdlMolfileReader::read_V3000_molecule_from_stream(bool is_chiral, std::istream& input_stream, ReactionComponent& result)
{
	get_next_line(input_stream);
	if (m_current_line != MDL_TAG_V30_LINE + MDL_TAG_V30_BLOCK_CTAB_BEGIN)
		throw MdlMolfileReaderError ("Expected V3000 CTAB block start, but got '" + m_current_line + "'");
	get_next_line(input_stream);

	if (m_current_line.substr(0, 13) != MDL_TAG_V30_LINE + MDL_TAG_V30_COUNTS)
		throw MdlMolfileReaderError ("Expected a V3000 CTAB COUNTS line, but got '" + m_current_line + "'");
	const char* c = m_current_line.c_str() + 13;
	while (*c == ' ' && *c != 0) c++;
	int atom_count = read_int(c);
	while (*c == ' ' && *c != 0) c++;
	int bond_count = read_int(c);
	// Skip number of Sgroups.
	while (*c == ' ' && *c != 0) c++;
	int chiral_flag = read_int(c);
	// Skip number of 3D constraints.
	while (*c == ' ' && *c != 0) c++;
	chiral_flag = read_int(c);
	// Read actual chiral flag.
	while (*c == ' ' && *c != 0) c++;
	chiral_flag = read_int(c);
	check_range(chiral_flag, 0, 1, "V3000 chiral flag");
	if (is_chiral && (chiral_flag != 1))
		throw MdlMolfileReaderError("Inconsistent chiral flag: V2000 and V3000 count line disagree on value.");

	result.initialize(atom_count, is_chiral);

	get_next_line(input_stream);
	if (m_current_line != MDL_TAG_V30_LINE + MDL_TAG_V30_BLOCK_ATOM_BEGIN)
		throw MdlMolfileReaderError ("Expected V3000 ATOM block start, but got '" + m_current_line + "'");

	// Molecules that only consist of a special atom are considered equivalent to No-Structures.
	if (atom_count == 1) {
		ios::off_type saved_pos = input_stream.tellg();
		int dummy_atom_number;
		string atom_symbol;
		get_next_line(input_stream);
		strip_V30_prefix(m_current_line);
		stringstream line (m_current_line);
		line >> dummy_atom_number >> atom_symbol;
		if (is_special_atom(atom_symbol)) {
			result.initialize(0, is_chiral);
			while (m_current_line != MDL_TAG_MOLFILE_END)
				get_next_line(input_stream);
			return;
		}
		else
			input_stream.seekg(saved_pos);
	}

	// Read atoms.
	int atom_number;
	string atom_symbol;
	Coordinate x;
	Coordinate y;
	Coordinate z;
	int aa_map;
	string key_value;
	try {
		for (int i = 1; i <= atom_count; i++) {
			get_next_line(input_stream);
			strip_V30_prefix(m_current_line);
			stringstream line (m_current_line);
			line >> atom_number >> atom_symbol >> x >> y >> z >> aa_map;

			if (atom_number != i)
				throw MdlMolfileReaderError ("Cowardly refusing to read V3000 molfile requiring a correspondance matrix for atom numbers - this is not supported yet");

			int charge = 0;
			int valence = 0;
			int radical = 0;
			int isotopic_mass = 0;

			line >> key_value;
			while (line && key_value.length() != 0) {
				if (parse_V3000_property(key_value, MDL_TAG_V30_ATOM_KEY_CHARGE, charge)) {}
				else if (parse_V3000_property(key_value, MDL_TAG_V30_ATOM_KEY_VALENCE, valence)) {}
				else if (parse_V3000_property(key_value, MDL_TAG_V30_ATOM_KEY_RADICAL, radical)) {}
				else if (parse_V3000_property(key_value, MDL_TAG_V30_ATOM_KEY_ISOTOPE, isotopic_mass)) {}
				// else, just ignore for now.
				line >> key_value;
			}

			inchi_set_atom(result.inchi_input(), i - 1, atom_symbol, x, y, z, charge, radical, isotopic_mass);
			if (valence != 0) {
				set_inchi_atom_valence(&(result.inchi_input().atom[i - 1]), valence, m_warning_logger);
			}
		}
	}
	catch (exception& e) {
		check(false, "Invalid atom (" + string(e.what()) + ")");
	}
	get_next_line(input_stream);
	if (m_current_line != MDL_TAG_V30_LINE + MDL_TAG_V30_BLOCK_ATOM_END)
		throw MdlMolfileReaderError ("Expected V3000 ATOM block end, but got '" + m_current_line + "'");

	if (bond_count > 0) {
		get_next_line(input_stream);
		if (m_current_line != MDL_TAG_V30_LINE + MDL_TAG_V30_BLOCK_BOND_BEGIN)
			throw MdlMolfileReaderError ("Expected V3000 BOND block start, but got '" + m_current_line + "'");

		// Read bonds.
		int bond_number;
		int from_atom_number;
		int to_atom_number;
		int bond_cardinality;
		BondStereo bond_stereo;
		try {
			for (int i = 1; i <= bond_count; i++) {
				get_next_line(input_stream);
				strip_V30_prefix(m_current_line);
				stringstream line (m_current_line);
				line >> bond_number >> bond_cardinality >> from_atom_number >> to_atom_number;

				if (bond_number != i)
					throw MdlMolfileReaderError ("Refusing to read V3000 molfile requiring a correspondance matrix for bond numbers - this is not supported yet");

				bond_stereo = RINCHI_BOND_STEREO_NONE;
				line >> key_value;
				while (line && key_value.length() != 0) {
					int stereo_code;
					if (parse_V3000_property(key_value, MDL_TAG_V30_STEREO_CFG_KEY, stereo_code)) {
						bond_stereo = V3000_stereo_code_to_BondStereo(stereo_code);
					}
					// else, just ignore for now.
					line >> key_value;
				}

				check_range(bond_cardinality, 1, 3, "Bond cardinality");
				inchi_add_bond(result.inchi_input(), from_atom_number - 1, to_atom_number - 1, bond_cardinality, bond_stereo);
			}
		}
		catch (exception& e) {
			check(false, "Invalid bond (" + string(e.what()) + ")");
		}
		get_next_line(input_stream);
		if (m_current_line != MDL_TAG_V30_LINE + MDL_TAG_V30_BLOCK_BOND_END)
			throw MdlMolfileReaderError ("Expected V3000 BOND block end, but got '" + m_current_line + "'.");
	}

	// Skip any collections, S-groups, what have you...
	get_next_line(input_stream);
	while (m_current_line.substr(0, MDL_TAG_V30_LINE.length() + 5) == MDL_TAG_V30_LINE + "BEGIN") {
		string end_marker = m_current_line.substr(0, MDL_TAG_V30_LINE.length()) + "END" + m_current_line.substr(MDL_TAG_V30_LINE.length() + 5);
		while (m_current_line != end_marker) {
			get_next_line(input_stream);
			trim_right(m_current_line, " ");
		}
		get_next_line(input_stream);
	}

	// We should now be at the end of the CTAB block.
	if (m_current_line != MDL_TAG_V30_LINE + MDL_TAG_V30_BLOCK_CTAB_END)
		throw MdlMolfileReaderError ("Expected V3000 CTAB block end, but got '" + m_current_line + "'.");

	get_next_line(input_stream);
	if (m_current_line != MDL_TAG_MOLFILE_END)
		check(false, "Missing '" + MDL_TAG_MOLFILE_END + "' at end - instead found '" + m_current_line + "'");
}

void MdlMolfileReader::read_molecule_from_stream(std::istream& input_stream, ReactionComponent& result)
{
	bool has_version_stamp = true;
	try {
		// Skip name line.
		get_next_line(input_stream);
		// Skip source-and-timestamp line.
		get_next_line(input_stream);
		// Skip comment line.
		get_next_line(input_stream);

		get_next_line(input_stream);
		// Not having the V2000 or V3000 version stamp is allowed - for backwards
		// compatibility with very old molfiles.
		if (m_current_line.length() == 33)
			has_version_stamp = false;
		else
			check(m_current_line.length() == 39, "Invalid header line - must be 39 characters long.");
		int atom_count = str2int(m_current_line.substr(0, 3));
		int bond_count = str2int(m_current_line.substr(3, 3));
		int prop_line_count = 0;
		if (!has_version_stamp)
			prop_line_count = str2int(m_current_line.substr(30, 3));

		char chiral_flag = m_current_line[14];
		check(chiral_flag == '0' || chiral_flag == '1', "Invalid chiral flag (must be '0' or '1').");
		bool is_chiral = chiral_flag == '1';

		// Check that line ends with "V2000" or "V3000" - if a version stamp is present
		if (has_version_stamp)
			m_current_line.erase(0, m_current_line.length() - 5);

		if (!has_version_stamp || m_current_line == MDL_TAG_V2000) {
			read_V2000_molecule_from_stream(atom_count, bond_count, is_chiral, has_version_stamp, prop_line_count, input_stream, result);
		}
		else if (m_current_line == MDL_TAG_V3000) {
			check(atom_count == 0, "Invalid V3000 file - V2000 atom count must be zero.");
			check(bond_count == 0, "Invalid V3000 file - V2000 bond count must be zero.");
			read_V3000_molecule_from_stream(is_chiral, input_stream, result);
		}
		else {
			throw MdlMolfileReaderError("Not an MDL " + MDL_TAG_V2000 + " or " + MDL_TAG_V3000 + " file");
		}
	}
	catch (exception& e) {
		throw_error(e);
	}
}

MdlMolfileReader::MdlMolfileReader(RInChILogger& warning_logger)
	: m_warning_logger(warning_logger)
{
}

void MdlMolfileReader::read_molecule(std::istream& input_stream, ReactionComponent& result, int lines_already_read)
{
	m_input_name = "std::istream";
	m_line_number = lines_already_read;

	if (!input_stream)
		throw_error("Input mol stream is not open");

	read_molecule_from_stream(input_stream, result);
}

void MdlMolfileReader::read_molecule(const std::string& file_name, ReactionComponent& result, int lines_already_read)
{
	m_input_name  = file_name;
	m_line_number = lines_already_read;

	ifstream input_stream ( file_name.c_str() );
	if (!input_stream)
		throw_error("Can't open input mol file '" + file_name + "'");

	read_molecule_from_stream(input_stream, result);
}

} // end of namespace
