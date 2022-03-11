TARGET = c_api_usage
CONFIG   += console
CONFIG   += no_autoqmake
CONFIG   -= app_bundle
CONFIG   -= qt
DEFINES  -= QT_WEBKIT

TEMPLATE = app

DEPENDPATH += \
	./../../rinchi_lib/ \
	./../../lib/ \
	./../../parsers/ \
	./../../rinchi/ \
	./../../writers/ \

INCLUDEPATH += \
	./../../rinchi_lib/ \
	./../../lib/ \
	./../../parsers/ \
	./../../rinchi/ \
	./../../writers/ \
	./../../../INCHI-1-API/INCHI_API/inchi_dll \

SOURCES += \
	./../../lib/rinchi_utils.cpp \
	./../../lib/rinchi_logger.cpp \
	./../../lib/inchi_api_intf.cpp \
	./../../lib/inchi_generator.cpp \
	./../../lib/rinchi_hashing.cpp \
	./../../parsers/mdl_molfile.cpp \
	./../../parsers/mdl_molfile_reader.cpp \
	./../../parsers/mdl_rxnfile_reader.cpp \
	./../../parsers/mdl_rdfile_reader.cpp \
	./../../parsers/rinchi_reader.cpp \
	./../../writers/mdl_rxnfile_writer.cpp \
	./../../writers/mdl_rdfile_writer.cpp \
	./../../rinchi/rinchi_reaction.cpp \
	./../../rinchi/rinchi_consts.cpp \
	./../../rinchi_lib/rinchi_lib.cpp \
	./c_api_usage.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichi_bns.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichi_io.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichican2.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichicano.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichicans.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiisot.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichilnct.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichimak2.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichimake.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichimap1.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichimap2.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichimap4.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichinorm.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiparm.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiprt1.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiprt2.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiprt3.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiqueu.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiread.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichiring.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichirvr1.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichirvr2.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichirvr3.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichirvr4.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichirvr5.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichirvr6.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichirvr7.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichisort.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichister.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ichitaut.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ikey_base26.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/ikey_dll.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/inchi_dll.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/inchi_dll_a.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/inchi_dll_a2.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/inchi_dll_main.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/runichi.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/sha2.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/strutil.c \
	./../../../INCHI-1-API/INCHI_API/inchi_dll/util.c \
