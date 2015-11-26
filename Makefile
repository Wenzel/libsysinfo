DIRS := $(shell find . -mindepth 1 -maxdepth 1 -type d)

all:
	+make -C _build

%:
	+make -C _build $@

$(DIRS):
	+make -C _build $@


.PHONY: $(DIRS)
