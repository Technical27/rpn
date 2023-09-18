DEBUG = FALSE

GCC = nspire-gcc
AS  = nspire-as
GXX = nspire-g++
LD  = nspire-ld
GENZEHN = genzehn

GCCFLAGS = -Wall -W -marm
LDFLAGS =
ZEHNFLAGS = --clickpad-support false --32MB-support true --240x320-support true --name "rpn" --compress --uses-lcd-blit true

ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -O2
else
	GCCFLAGS += -O0 -g
endif

OBJS = $(patsubst %.c, %.o, $(shell find src -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find src -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find src -name \*.S))
EXE = rpn
DISTDIR = .
vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

all: $(EXE).tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

%.o: %.cpp
	$(GXX) $(GCCFLAGS) -c $< -o $@
	
%.o: %.S
	$(AS) -c $<

$(EXE).elf: $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $(DISTDIR)/$@ $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $(DISTDIR)/$^ --output $(DISTDIR)/$@.zehn $(ZEHNFLAGS)
	make-prg $(DISTDIR)/$@.zehn $(DISTDIR)/$@
	rm $(DISTDIR)/$@.zehn

clean:
	rm -f $(OBJS) $(DISTDIR)/$(EXE).tns $(DISTDIR)/$(EXE).elf $(DISTDIR)/$(EXE).zehn
