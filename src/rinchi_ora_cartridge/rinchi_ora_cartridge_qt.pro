CONFIG   -= app_bundle
CONFIG   -= qt
DEFINES  -= QT_WEBKIT

TARGET = rinchi_ora_cartridge
TEMPLATE = lib

# InChI 1.0.6 required target definition.
DEFINES  += TARGET_API_LIB
QMAKE_CFLAGS += -ansi -DCOMPILE_ANSI_ONLY

DEPENDPATH += \
	./../lib/ \
	./../parsers/ \
	./../rinchi/ \
	./../writers/ \

INCLUDEPATH += \
	./../lib/ \
	./../parsers/ \
	./../rinchi/ \
	./../writers/ \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ \

SOURCES += \
	./../lib/rinchi_utils.cpp \
	./../lib/rinchi_hashing.cpp \
	./../lib/rinchi_logger.cpp \
	./../lib/inchi_api_intf.cpp \
	./../lib/inchi_generator.cpp \
	./../parsers/mdl_molfile.cpp \
	./../parsers/mdl_molfile_reader.cpp \
	./../parsers/mdl_rxnfile_reader.cpp \
	./../parsers/mdl_rdfile_reader.cpp \
	./../parsers/rinchi_reader.cpp \
	./../writers/mdl_rxnfile_writer.cpp \
	./../writers/mdl_rdfile_writer.cpp \
	./../rinchi/rinchi_reaction.cpp \
	./../rinchi/rinchi_consts.cpp \
	./rinchi_ora_cartridge.cpp \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichi_bns.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichi_io.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichican2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichicano.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichicans.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichierr.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiprt3.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiisot.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichimake.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiqueu.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiring.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichimap1.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichimap2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichimap4.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichimak2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichinorm.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiparm.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiprt1.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiprt2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichirvr1.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichirvr2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichirvr3.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichirvr4.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichirvr5.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichirvr6.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichirvr7.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichisort.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichister.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichitaut.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ikey_base26.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ikey_dll.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/mol_fmt1.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/mol_fmt2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/mol_fmt3.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/mol_fmt4.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/readinch.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/runichi.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/runichi2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/runichi3.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/runichi4.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/sha2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/strutil.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/util.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/mol2atom.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_BASE/src/ichiread.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_API/libinchi/src/ichilnct.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_API/libinchi/src/inchi_dll.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_API/libinchi/src/inchi_dll_a.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_API/libinchi/src/inchi_dll_a2.c \
        ./../../../InChI/INCHI-1-SRC/INCHI_API/libinchi/src/inchi_dll_b.c \
