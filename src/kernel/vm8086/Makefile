include ../../defs.mk

CFLAGS += -I../../include -I..

SRC_FILES = $(DYN_SRC_FILES)
OBJ_FILES = $(DYN_OBJ_FILES)

.PHONY: all

all: vm8086.ar

vm8086.ar: $(OBJ_FILES)
	$(AR) $@ $^

clean:
	$(RM) *.o *.lst *.ar *.s

.PHONY: depend
depend: makedepends.mk
makedepends.mk: $(SRC_FILES)
	makedepend -f- -- $(CFLAGS) -- $(SRC_FILES) > makedepends.mk

include makedepends.mk
