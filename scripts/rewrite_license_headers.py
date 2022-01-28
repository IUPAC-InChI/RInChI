# Rewrite license headers in all source files.
# Clears license headers if command line parameter is "clear" or "CLEAR".
# Must be run with current directory being the one where the script resides.

import sys
import os

processed_file_count = 0

def process_source_file(a_filename):
	global BSD_LICENSE_TXT
	global RINCHI_LICENSE_TXT
	global BSD_LICENSE_TXT_PYTHON
	global RINCHI_LICENSE_TXT_PYTHON

	source_txt = open(a_filename).readlines()
	start_line = 0
	while start_line < len(source_txt) and source_txt[start_line].find(BSD_LICENSE_MARKER_BEGIN) < 0 and source_txt[start_line].find(RINCHI_LICENSE_MARKER_BEGIN) < 0:
		start_line = start_line + 1

	if start_line < len(source_txt):
		stop_line = start_line + 1
		while stop_line < len(source_txt) and source_txt[stop_line].find(LICENSE_MARKER_END) < 0:
			stop_line = stop_line + 1

		if stop_line >= len(source_txt):
			raise Exception, "File '" + a_filename + "' has a license-begin marker but no end marker."

		if source_txt[start_line].find(BSD_LICENSE_MARKER_BEGIN) >= 0:
			if a_filename.endswith('.py'):
				new_txt = source_txt[0:start_line + 1] + BSD_LICENSE_TXT_PYTHON + source_txt[stop_line:]
			else:
				new_txt = source_txt[0:start_line + 1] + BSD_LICENSE_TXT + source_txt[stop_line:]
		else:
			if a_filename.endswith('.py'):
				new_txt = source_txt[0:start_line + 1] + RINCHI_LICENSE_TXT_PYTHON + source_txt[stop_line:]
			else:
				new_txt = source_txt[0:start_line + 1] + RINCHI_LICENSE_TXT + source_txt[stop_line:]

		open(a_filename, "w").write("".join(new_txt))
	else:
		print "WARNING: File '" + a_filename + "' has no license region marker."

	global processed_file_count
	processed_file_count = processed_file_count + 1


def process_dir(a_dir):
	files = os.listdir(a_dir)
	for f in sorted(files):
		if os.path.isdir(a_dir + f):
			process_dir(a_dir + f + "/")
		else:
			if f.endswith('.h') or f.endswith('.cpp') or f.endswith('.py') or f.endswith('.pck'):
				if a_dir <> BASE_DIR:
					process_source_file(a_dir + f)


BASE_DIR = "../src/"

BSD_LICENSE_TXT = [""]
RINCHI_LICENSE_TXT = [""]

if len(sys.argv) < 2:
	raise Exception, "You must pass either a revision date or the command 'CLEAR'."
if sys.argv[1].upper() != "CLEAR":
	BSD_LICENSE_TXT = open("../src/LICENCES/bsd_license.txt").readlines()
	RINCHI_LICENSE_TXT = open("../src/LICENCES/rinchi_license.txt").readlines()
	todays_date = sys.argv[1]
else:
	todays_date = ""

# Replace keywords/variables in LICENSE_TXT texts with values.
rinchi_consts = open("../src/rinchi/rinchi_consts.cpp").readlines()
rinchi_version = [x for x in rinchi_consts if x.find("RINCHI_VERSION =") > 0]
if len(rinchi_version) != 1:
	raise Exception, "Can't determine RINCHI_VERSION."
rinchi_version = rinchi_version[0]
rinchi_version = rinchi_version[rinchi_version.find("="):]
rinchi_version = rinchi_version.replace('"', '').replace("=", "").replace("\r", "").replace("\n", "").replace(";", "").replace(" ", "")

BSD_LICENSE_TXT = [s.replace("%RINCHI_VERSION%", rinchi_version).replace("%TODAY%", todays_date) for s in BSD_LICENSE_TXT]
RINCHI_LICENSE_TXT = [s.replace("%RINCHI_VERSION%", rinchi_version).replace("%TODAY%", todays_date) for s in RINCHI_LICENSE_TXT]

# Special version for Python code, since Python doesn't have multi-line comments.
BSD_LICENSE_TXT_PYTHON = ["#" + s for s in BSD_LICENSE_TXT]
RINCHI_LICENSE_TXT_PYTHON = ["#" + s for s in RINCHI_LICENSE_TXT]

BSD_LICENSE_MARKER_BEGIN = "#pragma region BSD-license"
RINCHI_LICENSE_MARKER_BEGIN = "#pragma region InChI-Trust Licence"
LICENSE_MARKER_END = "#pragma endregion"

process_dir(BASE_DIR)
print str(processed_file_count) + " files processed."
