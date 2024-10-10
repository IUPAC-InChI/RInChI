# Rewrite license headers in all source files.
# Clears license headers if command line parameter is "clear" or "CLEAR".
# Must be run with current directory being the one where the script resides.

import sys
import os
import datetime
import re

processed_file_count = 0

def process_source_file(a_filename):
	global RINCHI_LICENSE_TXT
	global RINCHI_LICENSE_TXT_PYTHON

	source_txt = open(a_filename).readlines()
	start_line = 0
	while start_line < len(source_txt) and source_txt[start_line].find(LICENSE_MARKER_BEGIN) < 0:
		start_line = start_line + 1

	if start_line < len(source_txt):
		stop_line = start_line + 1
		while stop_line < len(source_txt) and source_txt[stop_line].find(LICENSE_MARKER_END) < 0:
			stop_line = stop_line + 1

		if stop_line >= len(source_txt):
			raise Exception, "File '" + a_filename + "' has a license-begin marker but no end marker."

		# print(a_filename)
		license_path = re.sub("/[a-zA-Z0-9_-]*?/", "/../", a_filename)
		license_path = re.sub("/[a-zA-Z0-9_-]*?/", "/../", license_path)
		license_path = license_path[0:license_path.rfind("/")] + "/LICENCE.txt"
		# Convert first parent folder reference to current folder reference.
		license_path = license_path[1:]

		if a_filename.endswith('.py'):
			new_txt = source_txt[0:start_line + 1] + [s.replace("%LICENSE_PATH%", license_path) for s in RINCHI_LICENSE_TXT_PYTHON] + source_txt[stop_line:]
		else:
			new_txt = source_txt[0:start_line + 1] + [s.replace("%LICENSE_PATH%", license_path) for s in RINCHI_LICENSE_TXT] + source_txt[stop_line:]

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

RINCHI_LICENSE_TXT = open("./sourcefile_license_header.txt").readlines()
current_year = str(datetime.datetime.now().year)

RINCHI_LICENSE_TXT = [s.replace("%CURRENT_YEAR%", current_year) for s in RINCHI_LICENSE_TXT]

# Special version for Python code, since Python doesn't have multi-line comments.
RINCHI_LICENSE_TXT_PYTHON = ["# " + s for s in RINCHI_LICENSE_TXT]
# Normal C/C++ version.
RINCHI_LICENSE_TXT = ["// " + s for s in RINCHI_LICENSE_TXT]
print("".join(RINCHI_LICENSE_TXT))

if len(sys.argv) >= 2 and sys.argv[1].upper() == "CLEAR":
	RINCHI_LICENSE_TXT = [""]
	RINCHI_LICENSE_TXT_PYTHON = [""]

LICENSE_MARKER_BEGIN = "#pragma region RInChI-license"
LICENSE_MARKER_END = "#pragma endregion"

process_dir(BASE_DIR)
print str(processed_file_count) + " files processed."
