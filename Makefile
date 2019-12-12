include ../make_config

CFLAGS+=-std=c11 -Wall -Wpedantic

_SRC_FILES+=taw dice hero

LIBNAME:=dsa_core
LIBEXT:=a
LIB:=lib$(LIBNAME).$(LIBEXT)
LIB_TARGET:=$(BUILDPATH)$(LIB)

OBJS+=$(patsubst %,$(BUILDPATH)%,$(patsubst %,%.o,$(_SRC_FILES)))

THIRD_PARTY_LIB_DIR=./../_third_/
UTILS_DIR=./../utils/
UTILS_SRC_DIR=$(UTILS_DIR)src
INCLUDEDIR+=$(patsubst %,-I$(THIRD_PARTY_LIB_DIR)%, src pcre2_bin/include libarchive_bin/include libxml_bin/include/libxml2 libxslt_bin/include)
INCLUDEDIR+=-I$(UTILS_SRC_DIR)
THIRD_PARTY_LIBS=exslt xslt xml2 archive crypto nettle regex lzma z lz4 bz2 bcrypt iconv
REGEX_LIBS=pcre2-8
#this c flags is used by regex lib
CFLAGS+=-DPCRE2_STATIC

OS_LIBS=kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 uuid ole32 oleaut32 comctl32 ws2_32 zstd

USED_LIBS=$(patsubst %,-l%, dsa_core utils $(REGEX_LIBS) $(THIRD_PARTY_LIBS) $(OS_LIBS) )

USED_LIBSDIR=-L$(UTILS_DIR)$(BUILDPATH) -L./../math/utils/$(BUILDPATH) -L./$(BUILDPATH) -LC:/dev/opt/msys64/mingw64/lib
USED_LIBSDIR+=$(patsubst %,-L$(THIRD_PARTY_LIB_DIR)%,pcre2_bin/lib libarchive_bin/lib libxml_bin/lib libxslt_bin/lib)

#wc -c < filename => if needed for after compression size of bytes
RES=zip_resource
RES_O=$(RES).o
RES_O_PATH=$(BUILDPATH)$(RES_O)
RES_7Z=$(RES).7z
RES_FILES_PATTERN=./data/*
ZIP=7z
ZIP_ARGS=a -t7z
ZIP_CMD=$(ZIP) $(ZIP_ARGS)

all: mkbuilddir mkzip addzip $(LIB_TARGET)

$(LIB_TARGET): $(_SRC_FILES)
	$(AR) $(ARFLAGS) $(LIB_TARGET) $(OBJS) $(RES_O_PATH)

$(_SRC_FILES):
	$(CC) $(CFLAGS) -c src/$@.c -o $(BUILDPATH)$@.o $(INCLUDEDIR) $(debug)

test_hero: mkbuilddir mkzip addzip 
	$(CC) $(CFLAGS) ./test/test_hero.c ./src/hero.c -o $(BUILDPATH)test_hero.exe -I./src/ $(INCLUDEDIR) $(USED_LIBSDIR) -static $(USED_LIBS) $(debug)
	$(BUILDPATH)test_hero.exe

test_dice:
	$(CC) $(CFLAGS) ./test/test_dice.c ./src/dice.c -o $(BUILDPATH)test_dice.exe -I./src/ $(INCLUDEDIR) $(USED_LIBSDIR) -static $(USED_LIBS) $(debug)
	$(BUILDPATH)test_dice.exe

test_taw:
	$(CC) $(CFLAGS) ./test/test_taw.c ./src/taw.c -o $(BUILDPATH)test_taw.exe -I./src/ $(INCLUDEDIR) $(USED_LIBSDIR) -static $(USED_LIBS) $(debug)
	$(BUILDPATH)test_taw.exe

.PHONY: clean mkbuilddir mkzip addzip
	
addzip:
	cd $(BUILDPATH); \
	ld -r -b binary $(RES_7Z) -o $(RES_O)

mkzip:
	-$(ZIP_CMD) $(BUILDPATH)$(RES_7Z) $(RES_FILES_PATTERN)

mkbuilddir:
	mkdir -p $(BUILDDIR)
	
clean:
	-rm -dr $(BUILDROOT)