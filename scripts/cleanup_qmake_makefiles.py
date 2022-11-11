# Filter a qmake-generated Makefile so qmake dependencies are removed.
# Arguments: <makefile-name> <Qt project file name>
# Result is output on stdout.

import sys

makefile_filename = sys.argv[1]
qtproject_filename = sys.argv[2]

makefile_txt = open(makefile_filename).readlines()

is_in_dist_block = False
is_in_makefile_dep_block = False

for i in range(0, len(makefile_txt)):
  line = makefile_txt[i].replace('\n', '').replace('\r', '')
  if line.startswith("INCPATH "):
    # Remove Qt include, if it is present and it is the last include.
    if line.find("/qt") > line.rfind("-I"):
      line = line[:line.rfind("-I")].strip()

  if line.startswith("DIST ") and line.find("/qt") > 0:
    is_in_dist_block = True

  if line.startswith(makefile_filename + ":"):
    is_in_makefile_dep_block = True

  if not is_in_dist_block and not is_in_makefile_dep_block:
    print(line)
  else:
    if is_in_dist_block:
      if line.find(qtproject_filename) >= 0:
        line = line[line.find(qtproject_filename) + len(qtproject_filename):].strip()
        print("DIST          = " + line)
        is_in_dist_block = False

    if is_in_makefile_dep_block:
      if line.strip() == "qmake_all: FORCE":
        is_in_makefile_dep_block = False

