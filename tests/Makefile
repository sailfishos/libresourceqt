QMAKE = qmake
MAKEFILES = $(patsubst %.pro,%.make,$(wildcard *.pro))

%.make: %.pro
	$(QMAKE) -o $@ $<

tests: all
	find build -type f -name 'test-*' -exec sh -c "if test -x {}; then {}; fi" \;

clean:
	$(RM) $(MAKEFILES)
	$(RM) -r build

all: $(MAKEFILES)
	for makefile in $(MAKEFILES); do $(MAKE) -f "$$makefile" all; done

install: $(MAKEFILES)
	for makefile in $(MAKEFILES); do $(MAKE) -f "$$makefile" install; done

.phony: install tests all clean
