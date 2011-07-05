.SUFFIXES: .xbm .bmp .rcp .dir

ENG=ricochet.prc
TARGETS=$(ENG)
LANG=eng
PROG = ricochet
ZIP = ricochet.zip
SRCTAR = ricochetSrc.tar.gz
SRCS = board.c layout.c main.c move.c random.c shuffle.c
OBJS = board.o layout.o main.o move.o random.o shuffle.o
HDRS = ricochet.h ricochetRsc.h
BMPS = robotActive.bmp \
       robotPassive.bmp \
       target.bmp

RES = ricochetRsc.rcp

ICONS = ricochet1bpp.bmp
DOC = readme.txt

BUILD = Makefile $(SRCS) $(HDRS) $(BMPS) $(RES) $(ICON) $(DOC)

RLOC= rloc0000.$(PROG).grc
GRCS = code0000.$(PROG).grc code0001.$(PROG).grc data0000.$(PROG).grc $(RLOC)
PREF= pref0000.$(PROG).grc

PREFIX = /usr/m68k-palmos
TOOLS= $(PREFIX)/bin
SDKFIND=$(TOOLS)/sdkfind
#TOOLS= /local/src/prc/prc-tools-0.4.2
CC = $(TOOLS)/gcc

#uncomment this if you want to build a gdb debuggable version
#DEFINES = -DDEBUG
DEFINES = -DNDEBUG

CSFLAGS = -O2 -S $(DEFINES)
CFLAGS = -O2 `$(SDKFIND)` $(DEFINES) -Wall -Wstrict-prototypes -fno-builtin

#LIBS = $(LDIR) -lgdb -lc -lmf -lgcc

PILRC = pilrc
TXT2BITM = $(TOOLS)/txt2bitm
OBJRES = $(TOOLS)/obj-res
#OBJRES = $(TOOLS)/obj-res
#BUILDPRC = $(TOOLS)/build-prc
BUILDPRC = build-prc
#BUILDPRC = /local/src/prc/prc-tools-0.4.2/build-prc

ICONTEXT = "Ricochet"
APPID = Rico

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

.xbm.bmp:
	xbmtopbm $< | ppmtobmp -windows > $@

all: $(TARGETS)

$(ZIP): $(TARGETS) $(DOC)
	zip $@ $(TARGETS) $(DOC)

$(SRCTAR): $(BUILD)
	tar czf $(SRCTAR) $(BUILD)

$(ENG): eng
	cd eng && $(BUILDPRC) ../$@ $(ICONTEXT) $(APPID) $(GRCS) *.bin $(PREF)

eng: $(RES) $(BMPS) ricochetRsc.h $(GRCS)
	rm -rf $@
	mkdir $@
	$(PILRC) -L $@ $(RES) $@
	cp $(GRCS) $(PREF) $@

$(IT): it
	cd it && $(BUILDPRC) ../$@ $(ICONTEXT) $(APPID) $(GRCS) *.bin $(PREF)

it: $(RES) $(BMPS) ricochetRsc.h $(GRCS)
	rm -rf $@
	mkdir $@
	$(PILRC) -L $@ $(RES) $@
	cp $(GRCS) $(PREF) $@

$(ICONRES): $(ICON)
	$(TXT2BITM) $(ICON)
	
$(GRCS): $(PROG)
	$(OBJRES) $(PROG)

$(PROG): $(OBJS)
#	$(CC) -Wl,-M -Wl,-r -Wl,-dc $(CFLAGS) $(OBJS) -o $(PROG) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG) $(LIBS)

$(OBJS): $(HDRS)

spider.o spiderette.o: spider.h
klondike.o vegas.o: klondike.h

clean:
	rm -rf MakeOut $(OBJS) $(TARGETS) $(PROG) $(LANG) $(GRCS) $(PREF) $(ZIP) $(SRCTAR)
