#pragma region InChI-Trust Licence
#/*
# * Reaction International Chemical Identifier (RInChI)
# * Version 1
# * Software version 1.00
# * 2022-01-14
# * 
# * The RInChI library and programs are free software developed under the
# * auspices of the International Union of Pure and Applied Chemistry (IUPAC).
# * 
# * IUPAC/InChI-Trust Licence No.1.0 for the 
# * Reaction International Chemical Identifier (RInChI) Software version 1.0
# * Copyright (C) IUPAC and InChI Trust Limited
# * 
# * This library is free software; you can redistribute it and/or modify it 
# * under the terms of the IUPAC/InChI Trust InChI Licence No.1.0, 
# * or any later version.
# * 
# * Please note that this library is distributed WITHOUT ANY WARRANTIES 
# * whatsoever, whether expressed or implied.  See the IUPAC/InChI Trust
# * Licence for the International Chemical Identifier (InChI) Software
# * ("IUPAC/InChI-Trust InChI Licence No. 1.0" in "LICENCE.TXT")
# * for more details.
# * 
# * You should have received a copy of the IUPAC/InChI Trust InChI 
# * Licence No. 1.0 with this library; if not, please write to:
# * 
# *     The InChI Trust
# *     8 Cavendish Avenue
# *     Cambridge CB1 7US
# *     UK
# *
# * or email to: alan@inchi-trust.org.
# *
# */
#pragma endregion

import rinchi_lib

# ## Shadow build with Qt creator on Linux.
# rinchi = rinchi_lib.RInChI("/home/jhje/RInChI/qt_build/librinchi_qt-build-desktop-Qt_4_8_2__System__Release")

# ## 64-bit build with Visual Studio.
# rinchi = rinchi_lib.RInChI("./x64/Release")

# Linux build with standard Makefile.
rinchi = rinchi_lib.RInChI("./")

rxndata = open("../test/rxnfiles/R005a.rxn").readlines()
rxndata = "".join(rxndata)

print ("=== RXN file read ===")
print (rinchi.rinchi_from_file_text("RXN", rxndata))
print (rinchi.rinchi_from_file_text("RXN", rxndata, True))
print (rinchi.rinchi_from_file_text("AUTO", rxndata))
print ("===========")

# rddata = open("/home/jhje/RInChI/src/test/RDfiles/Example 01 CCR.rdf").readlines()
rddata = open("../test/RDfiles/Example 01 CCR.rdf").readlines()
rddata = "".join(rddata)

print (rinchi.rinchikey_from_file_text("RD", rddata, "L"))
print (rinchi.rinchikey_from_file_text("AUTO", rddata, "L"))
print ("")
print (rinchi.rinchikey_from_file_text("RD", rddata, "S"))
print (rinchi.rinchikey_from_file_text("RD", rddata, "W"))

print ("=== RD file read ===")
rinchi_data = rinchi.rinchi_from_file_text("RD", rddata, True)
print (rinchi_data)
rinchi_data = rinchi.rinchi_from_file_text("RD", rddata)
print (rinchi_data)
print ("=========")
# print (rinchi.file_text_from_rinchi(rinchi_data[0], rinchi_data[1], "RD"))

# Split RInChI into reaction components.
inchi_components = rinchi.inchis_from_rinchi(rinchi_data[0], rinchi_data[1])
print ("Direction:")
print (inchi_components['Direction'])
print ("Reactants:")
print (inchi_components['Reactants'])
print ("Products:")
print (inchi_components['Products'])
print ("Agents:")
print (inchi_components['Agents'])
# With blank RAuxInfo.
inchi_components = rinchi.inchis_from_rinchi(rinchi_data[0], "")
print ("Reactants:")
print (inchi_components['Reactants'])
print ("Products:")
print (inchi_components['Products'])
print ("Agents:")
print (inchi_components['Agents'])

print ("== Print all agent InChIs ==")

agents = inchi_components["Agents"]
for i in range(0, len(agents)):
	print (agents[i][0])

rinchi_nostructs = "RInChI=1.00.1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d+/u1-2-3"
inchi_components = rinchi.inchis_from_rinchi(rinchi_nostructs, "")
print ("==========")
print (inchi_components['Reactants'])
print (inchi_components['No-Structures'])

# Load InChI text and calculate RInChI.
print ("=== RInChI from InChI input text ===")
print (rinchi.rinchi_from_inchis("""InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1
AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;
InChI=1S/Na.H2O/h;1H2/q+1;/p-1
AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;""", """InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1
AuxInfo=1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;""", ""))
#   Expected: RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-
#   Expected: RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;<>0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;

#   Checking error propagation: Blank line in middle should cause "EOF expected" error.
try:
	print (rinchi.rinchi_from_inchis("""InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1
AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;
InChI=1S/Na.H2O/h;1H2/q+1;/p-1

AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;""", "", ""))
	raise Exception("No error raised as expected, reading InChI input text.")
except Exception as e:
	print (str(e))

# RInChI => RInChI keys.
print ("=== RInChI keys from RInChI ===")
print (rinchi.rinchikey_from_rinchi(rinchi_nostructs, "L"))
print (rinchi.rinchikey_from_rinchi(rinchi_nostructs, "S"))
print (rinchi.rinchikey_from_rinchi(rinchi_nostructs, "W"))

