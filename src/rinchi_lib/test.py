#pragma region RInChI-license
# Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
# This file is part of the RInChI source code.
# The contents are covered by the terms of the MIT license
# included in the file ./../../LICENCE.txt.
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
assert (rinchi.rinchi_from_file_text("RXN", rxndata) == [
    'RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-',
    'RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;<>0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;',
])
assert (rinchi.rinchi_from_file_text("RXN", rxndata, True) == [
    'RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d=',
    'RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;<>0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;',
])
assert (rinchi.rinchi_from_file_text("AUTO", rxndata) == [
    'RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-',
    'RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;<>0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;',
])
print ("===========")

# rddata = open("/home/jhje/RInChI/src/test/RDfiles/Example 01 CCR.rdf").readlines()
rddata = open("../test/RDfiles/Example 01 CCR.rdf").readlines()
rddata = "".join(rddata)

assert (
    rinchi.rinchikey_from_file_text("RD", rddata, "L") ==
    "Long-RInChIKey=SA-BUHFF-OCEYRUMTOAEWEA-UYRXBGFRSA-N--HRQNWWCYHJTAPI-UHFFFAOYSA-N--WEVYAHXRMPXWCK-UHFFFAOYSA-N-WGHUNMFFLAMBJD-UHFFFAOYSA-M")
assert (
    rinchi.rinchikey_from_file_text("AUTO", rddata, "L") ==
    "Long-RInChIKey=SA-BUHFF-OCEYRUMTOAEWEA-UYRXBGFRSA-N--HRQNWWCYHJTAPI-UHFFFAOYSA-N--WEVYAHXRMPXWCK-UHFFFAOYSA-N-WGHUNMFFLAMBJD-UHFFFAOYSA-M")
assert (
    rinchi.rinchikey_from_file_text("RD", rddata, "S") ==
    "Short-RInChIKey=SA-BUHFF-OCEYRUMTOA-HRQNWWCYHJ-ANFMRDWEKN-NDGAC-NUHFF-MUHFF-ZZZ")
assert (
    rinchi.rinchikey_from_file_text("RD", rddata, "W") ==
    "Web-RInChIKey=YVHOQDQUXJQWHSZQW-MSWCGYDXJTZWXSA")

print ("=== RD file read ===")
rinchi_data = rinchi.rinchi_from_file_text("RD", rddata, True)
assert (rinchi_data == [
    'RInChI=1.00.1S/C18H13NO5S2/c1-12-2-4-14(5-3-12)26(21,22)19-13-6-9-18(10-7-13)23-15-8-11-25-16(15)17(20)24-18/h2-11H,1H3/b19-13-<>C18H15NO5S2/c1-12-2-8-15(9-3-12)26(22,23)19-13-4-6-14(7-5-13)24-16-10-11-25-17(16)18(20)21/h2-11,19H,1H3,(H,20,21)<>C2H3N/c1-2-3/h1H3!C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/q+1;/p-1/d=',
    'RAuxInfo=1.00.1/0/N:26,23,24,21,22,8,9,11,4,5,15,25,13,18,6,10,7,1,16,12,19,20,2,3,14,17/E:(2,3)(4,5)(6,7)(9,10)(21,22)/CRV:26.6/rA:26nCOOCCCCCCCCOCSCNSCOOCCCCCC/rB:s1;s1;s1;s1;s2;s3;d4;d5;d6s7;s6;d7;s8s9;s10;d11s14;d13;s16;s17;d17;d17;s18;d18;d21;s22;s23d24;s25;/rC:2.9831,-1.7951,0;1.65,-1.0285,0;2.9831,-3.3081,0;2.9831,-.2553,0;4.3162,-2.5416,0;.3265,-1.7951,0;1.65,-4.0813,0;4.3162,.5112,0;5.6426,-1.7951,0;.3265,-3.3081,0;-1.1738,-1.3285,0;1.65,-5.6211,0;5.6426,-.2553,0;-1.1304,-3.7747,0;-2.0639,-2.5216,0;6.9691,.5112,0;8.3022,-.2553,0;9.6353,-1.0285,0;9.0754,1.1378,0;7.5357,-1.5351,0;9.6353,-2.5483,0;10.9684,-.2553,0;10.9684,-3.3215,0;12.3082,-1.0285,0;12.3082,-2.5483,0;13.6413,-3.3215,0;<>1/N:26,23,24,13,14,11,12,21,22,4,8,25,15,7,18,1,2,6,16,9,10,19,20,3,5,17/E:(2,3)(4,5)(6,7)(8,9)(20,21)(22,23)/CRV:26.6/rA:26nCCOCSCCCOOCCCCCNSCOOCCCCCC/rB:d1;s1;s1;s2;s2;s3;d4s5;s6;d6;s7;d7;d11;s12;s13d14;s15;s16;s17;d17;d17;s18;d18;d21;s22;s23d24;s25;/rC:-10.0504,-6.767,0;-9.6286,-8.2146,0;-9.1595,-5.5177,0;-11.6085,-6.767,0;-10.8357,-9.0711,0;-8.1587,-8.7063,0;-10.0326,-4.2448,0;-12.0823,-8.1749,0;-7.8513,-10.1616,0;-6.9429,-7.6753,0;-9.3653,-2.8527,0;-11.5645,-4.3558,0;-10.2081,-1.5918,0;-12.4292,-3.0869,0;-11.7574,-1.7028,0;-12.6309,-.4297,0;-11.7138,.8195,0;-10.7833,2.0559,0;-13.0173,1.7436,0;-10.5154,-.0808,0;-9.2776,1.889,0;-11.4238,3.4821,0;-8.3605,3.1398,0;-10.4892,4.7258,0;-8.9752,4.5552,0;-8.0705,5.7965,0;<>0/N:2,1,3/rA:3nCCN/rB:s1;t1;/rC:.5157,.2238,0;-1.0243,.2238,0;2.088,.2238,0;!1/N:6,7,8,9,2,3,4,5,1;10,11,12,13,14/E:(1,2,3,4)(5,6,7,8);(2,3,4,5)/CRV:9+1;1.7/rA:14nN+CCCCCCCCClOOOO-/rB:s1;s1;s1;s1;s2;s3;s4;s5;;d10;d10;d10;s10;/rC:-2.3374,.2568,0;-3.8783,.2568,0;-1.8238,1.7084,0;-1.0384,-.5695,0;-3.0446,-1.1091,0;-4.3956,-1.1948,0;-3.0222,2.6761,0;.1749,.3759,0;-1.9913,-2.2332,0;3.0595,.2308,0;3.4763,1.7419,0;3.4949,-1.299,0;4.5222,.2159,0;1.5186,.2308,0;',
])
rinchi_data = rinchi.rinchi_from_file_text("RD", rddata)
assert (rinchi_data == [
    'RInChI=1.00.1S/C18H13NO5S2/c1-12-2-4-14(5-3-12)26(21,22)19-13-6-9-18(10-7-13)23-15-8-11-25-16(15)17(20)24-18/h2-11H,1H3/b19-13-<>C18H15NO5S2/c1-12-2-8-15(9-3-12)26(22,23)19-13-4-6-14(7-5-13)24-16-10-11-25-17(16)18(20)21/h2-11,19H,1H3,(H,20,21)<>C2H3N/c1-2-3/h1H3!C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/q+1;/p-1/d-',
    'RAuxInfo=1.00.1/0/N:26,23,24,21,22,8,9,11,4,5,15,25,13,18,6,10,7,1,16,12,19,20,2,3,14,17/E:(2,3)(4,5)(6,7)(9,10)(21,22)/CRV:26.6/rA:26nCOOCCCCCCCCOCSCNSCOOCCCCCC/rB:s1;s1;s1;s1;s2;s3;d4;d5;d6s7;s6;d7;s8s9;s10;d11s14;d13;s16;s17;d17;d17;s18;d18;d21;s22;s23d24;s25;/rC:2.9831,-1.7951,0;1.65,-1.0285,0;2.9831,-3.3081,0;2.9831,-.2553,0;4.3162,-2.5416,0;.3265,-1.7951,0;1.65,-4.0813,0;4.3162,.5112,0;5.6426,-1.7951,0;.3265,-3.3081,0;-1.1738,-1.3285,0;1.65,-5.6211,0;5.6426,-.2553,0;-1.1304,-3.7747,0;-2.0639,-2.5216,0;6.9691,.5112,0;8.3022,-.2553,0;9.6353,-1.0285,0;9.0754,1.1378,0;7.5357,-1.5351,0;9.6353,-2.5483,0;10.9684,-.2553,0;10.9684,-3.3215,0;12.3082,-1.0285,0;12.3082,-2.5483,0;13.6413,-3.3215,0;<>1/N:26,23,24,13,14,11,12,21,22,4,8,25,15,7,18,1,2,6,16,9,10,19,20,3,5,17/E:(2,3)(4,5)(6,7)(8,9)(20,21)(22,23)/CRV:26.6/rA:26nCCOCSCCCOOCCCCCNSCOOCCCCCC/rB:d1;s1;s1;s2;s2;s3;d4s5;s6;d6;s7;d7;d11;s12;s13d14;s15;s16;s17;d17;d17;s18;d18;d21;s22;s23d24;s25;/rC:-10.0504,-6.767,0;-9.6286,-8.2146,0;-9.1595,-5.5177,0;-11.6085,-6.767,0;-10.8357,-9.0711,0;-8.1587,-8.7063,0;-10.0326,-4.2448,0;-12.0823,-8.1749,0;-7.8513,-10.1616,0;-6.9429,-7.6753,0;-9.3653,-2.8527,0;-11.5645,-4.3558,0;-10.2081,-1.5918,0;-12.4292,-3.0869,0;-11.7574,-1.7028,0;-12.6309,-.4297,0;-11.7138,.8195,0;-10.7833,2.0559,0;-13.0173,1.7436,0;-10.5154,-.0808,0;-9.2776,1.889,0;-11.4238,3.4821,0;-8.3605,3.1398,0;-10.4892,4.7258,0;-8.9752,4.5552,0;-8.0705,5.7965,0;<>0/N:2,1,3/rA:3nCCN/rB:s1;t1;/rC:.5157,.2238,0;-1.0243,.2238,0;2.088,.2238,0;!1/N:6,7,8,9,2,3,4,5,1;10,11,12,13,14/E:(1,2,3,4)(5,6,7,8);(2,3,4,5)/CRV:9+1;1.7/rA:14nN+CCCCCCCCClOOOO-/rB:s1;s1;s1;s1;s2;s3;s4;s5;;d10;d10;d10;s10;/rC:-2.3374,.2568,0;-3.8783,.2568,0;-1.8238,1.7084,0;-1.0384,-.5695,0;-3.0446,-1.1091,0;-4.3956,-1.1948,0;-3.0222,2.6761,0;.1749,.3759,0;-1.9913,-2.2332,0;3.0595,.2308,0;3.4763,1.7419,0;3.4949,-1.299,0;4.5222,.2159,0;1.5186,.2308,0;',
])
print ("=========")
# print (rinchi.file_text_from_rinchi(rinchi_data[0], rinchi_data[1], "RD"))

# Split RInChI into reaction components.
inchi_components = rinchi.inchis_from_rinchi(rinchi_data[0], rinchi_data[1])
assert (inchi_components['Direction'] == "-")
assert (inchi_components['Reactants'] == [
    (
        'InChI=1S/C18H15NO5S2/c1-12-2-8-15(9-3-12)26(22,23)19-13-4-6-14(7-5-13)24-16-10-11-25-17(16)18(20)21/h2-11,19H,1H3,(H,20,21)',
        'AuxInfo=1/1/N:26,23,24,13,14,11,12,21,22,4,8,25,15,7,18,1,2,6,16,9,10,19,20,3,5,17/E:(2,3)(4,5)(6,7)(8,9)(20,21)(22,23)/CRV:26.6/rA:26nCCOCSCCCOOCCCCCNSCOOCCCCCC/rB:d1;s1;s1;s2;s2;s3;d4s5;s6;d6;s7;d7;d11;s12;s13d14;s15;s16;s17;d17;d17;s18;d18;d21;s22;s23d24;s25;/rC:-10.0504,-6.767,0;-9.6286,-8.2146,0;-9.1595,-5.5177,0;-11.6085,-6.767,0;-10.8357,-9.0711,0;-8.1587,-8.7063,0;-10.0326,-4.2448,0;-12.0823,-8.1749,0;-7.8513,-10.1616,0;-6.9429,-7.6753,0;-9.3653,-2.8527,0;-11.5645,-4.3558,0;-10.2081,-1.5918,0;-12.4292,-3.0869,0;-11.7574,-1.7028,0;-12.6309,-.4297,0;-11.7138,.8195,0;-10.7833,2.0559,0;-13.0173,1.7436,0;-10.5154,-.0808,0;-9.2776,1.889,0;-11.4238,3.4821,0;-8.3605,3.1398,0;-10.4892,4.7258,0;-8.9752,4.5552,0;-8.0705,5.7965,0;',
    ),
])
assert (inchi_components['Products'] == [
    (
        'InChI=1S/C18H13NO5S2/c1-12-2-4-14(5-3-12)26(21,22)19-13-6-9-18(10-7-13)23-15-8-11-25-16(15)17(20)24-18/h2-11H,1H3/b19-13-',
        'AuxInfo=1/0/N:26,23,24,21,22,8,9,11,4,5,15,25,13,18,6,10,7,1,16,12,19,20,2,3,14,17/E:(2,3)(4,5)(6,7)(9,10)(21,22)/CRV:26.6/rA:26nCOOCCCCCCCCOCSCNSCOOCCCCCC/rB:s1;s1;s1;s1;s2;s3;d4;d5;d6s7;s6;d7;s8s9;s10;d11s14;d13;s16;s17;d17;d17;s18;d18;d21;s22;s23d24;s25;/rC:2.9831,-1.7951,0;1.65,-1.0285,0;2.9831,-3.3081,0;2.9831,-.2553,0;4.3162,-2.5416,0;.3265,-1.7951,0;1.65,-4.0813,0;4.3162,.5112,0;5.6426,-1.7951,0;.3265,-3.3081,0;-1.1738,-1.3285,0;1.65,-5.6211,0;5.6426,-.2553,0;-1.1304,-3.7747,0;-2.0639,-2.5216,0;6.9691,.5112,0;8.3022,-.2553,0;9.6353,-1.0285,0;9.0754,1.1378,0;7.5357,-1.5351,0;9.6353,-2.5483,0;10.9684,-.2553,0;10.9684,-3.3215,0;12.3082,-1.0285,0;12.3082,-2.5483,0;13.6413,-3.3215,0;',
    ),
])
assert (inchi_components['Agents'] == [
    (
        'InChI=1S/C2H3N/c1-2-3/h1H3',
        'AuxInfo=1/0/N:2,1,3/rA:3nCCN/rB:s1;t1;/rC:.5157,.2238,0;-1.0243,.2238,0;2.088,.2238,0;',
    ),
    (
        'InChI=1S/C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/q+1;/p-1',
        'AuxInfo=1/1/N:6,7,8,9,2,3,4,5,1;10,11,12,13,14/E:(1,2,3,4)(5,6,7,8);(2,3,4,5)/CRV:9+1;1.7/rA:14nN+CCCCCCCCClOOOO-/rB:s1;s1;s1;s1;s2;s3;s4;s5;;d10;d10;d10;s10;/rC:-2.3374,.2568,0;-3.8783,.2568,0;-1.8238,1.7084,0;-1.0384,-.5695,0;-3.0446,-1.1091,0;-4.3956,-1.1948,0;-3.0222,2.6761,0;.1749,.3759,0;-1.9913,-2.2332,0;3.0595,.2308,0;3.4763,1.7419,0;3.4949,-1.299,0;4.5222,.2159,0;1.5186,.2308,0;',
    ),
])
# With blank RAuxInfo.
inchi_components = rinchi.inchis_from_rinchi(rinchi_data[0], "")
assert (inchi_components['Reactants'] == [
    ('InChI=1S/C18H15NO5S2/c1-12-2-8-15(9-3-12)26(22,23)19-13-4-6-14(7-5-13)24-16-10-11-25-17(16)18(20)21/h2-11,19H,1H3,(H,20,21)', ''),
])
assert (inchi_components['Products'] == [
    ('InChI=1S/C18H13NO5S2/c1-12-2-4-14(5-3-12)26(21,22)19-13-6-9-18(10-7-13)23-15-8-11-25-16(15)17(20)24-18/h2-11H,1H3/b19-13-', ''),
])
assert (inchi_components['Agents'] == [
    ('InChI=1S/C2H3N/c1-2-3/h1H3', ''),
    ('InChI=1S/C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/q+1;/p-1', ''),
])

print ("== Print all agent InChIs ==")

agents = inchi_components["Agents"]
assert (agents == [
    ('InChI=1S/C2H3N/c1-2-3/h1H3', ''),
    ('InChI=1S/C8H20N.ClHO4/c1-5-9(6-2,7-3)8-4;2-1(3,4)5/h5-8H2,1-4H3;(H,2,3,4,5)/q+1;/p-1', ''),
])

rinchi_nostructs = "RInChI=1.00.1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d+/u1-2-3"
inchi_components = rinchi.inchis_from_rinchi(rinchi_nostructs, "")
print ("==========")
assert (inchi_components['Reactants'] == [
    ('InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1', ''),
    ('InChI=1S/Na.H2O/h;1H2/q+1;/p-1', ''),
    ('InChI=1S//', ''),
])
assert (inchi_components['No-Structures'] == [1, 2, 3])

# Load InChI text and calculate RInChI.
print ("=== RInChI from InChI input text ===")
assert (rinchi.rinchi_from_inchis("""InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1
AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;
InChI=1S/Na.H2O/h;1H2/q+1;/p-1
AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;""", """InChI=1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1
AuxInfo=1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;""", "") ==
    [
        'RInChI=1.00.1S/C4H8O/c1-3-4(2)5-3/h3-4H,1-2H3/t3-,4?/m0/s1<>C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1!Na.H2O/h;1H2/q+1;/p-1/d-',
        'RAuxInfo=1.00.1/0/N:4,1,3,2,5/E:(1,2)(3,4)/it:im/rA:5nCCCCO/rB:N1;s2;P3;s2s3;/rC:-1.127,-.5635,0;-.4125,-.151,0;.4125,-.151,0;1.127,-.5635,0;0,.5635,0;<>0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;!1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;',
    ]
)

#   Checking error propagation: Blank line in middle should cause "EOF expected" error.
try:
	print (rinchi.rinchi_from_inchis("""InChI=1S/C4H9BrO/c1-3(5)4(2)6/h3-4,6H,1-2H3/t3-,4+/m1/s1
AuxInfo=1/0/N:4,1,3,2,6,5/it:im/rA:6nCCCCOBr/rB:s1;s2;s3;N2;P3;/rC:-.825,-.7557,0;-.4125,-.0412,0;.4125,-.0412,0;.825,.6733,0;-.626,.7557,0;.825,-.7557,0;
InChI=1S/Na.H2O/h;1H2/q+1;/p-1

AuxInfo=1/1/N:1;2/rA:2nNaO/rB:s1;/rC:-.4125,0,0;.4125,0,0;""", "", ""))
except Exception as e:
	assert (str(e) ==
		"rinchi::RInChIReaderError: Line 5: Unexpected trailing data; expected an EOF after previous blank line.")
else:
	assert False, "No error raised as expected, reading InChI input text."


# RInChI => RInChI keys.
print ("=== RInChI keys from RInChI ===")
assert (rinchi.rinchikey_from_rinchi(rinchi_nostructs, "L") ==
    "Long-RInChIKey=SA-BUHFF-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N--JCYSVJNMXBWPHS-DMTCNVIQSA-N-HEMHJVSKTPXQMS-UHFFFAOYSA-M-MOSFIJXAXDLOML-UHFFFAOYSA-N--MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N-MOSFIJXAXDLOML-UHFFFAOYSA-N")
assert (rinchi.rinchikey_from_rinchi(rinchi_nostructs, "S") ==
    "Short-RInChIKey=SA-BUHFF-UHFFFADPSC-LSZRVNUIRJ-UHFFFADPSC-NUHFF-MCQMH-NUHFF-BAC")
assert (rinchi.rinchikey_from_rinchi(rinchi_nostructs, "W") ==
    "Web-RInChIKey=BAUCRGUJNIZBSVGHT-MCQMHLCOYUUCESA")

