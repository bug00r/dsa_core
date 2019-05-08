include ../make_config

all: mkbuilddir

.PHONY: clean mkbuilddir

mkbuilddir:
	mkdir -p $(BUILDDIR)
	
clean:
	-rm -dr $(BUILDROOT)