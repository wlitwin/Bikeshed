include ../../defs.mk

CFLAGS += -I../../include -I../../ulib -I../../kernel/lib -I.. -I../../kernel/bios -I../../applications -I../../
CXXFLAGS += -I../../include -I../../ulib -I../../kernel/lib -I.. -I../../kernel/data_structures -I../../kernel/memory -I../../ -I../../kernel/input

SRC_FILES = $(DYN_SRC_FILES)
OBJ_FILES = $(DYN_OBJ_FILES)

.PHONY: all

all: video.ar

video.ar: $(OBJ_FILES)
	$(AR) $@ $^

clean:
	$(RM) *.o *.lst *.ar
