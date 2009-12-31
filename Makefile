QMAKE = qmake
MAKEFILE = libresourceqt.make
LIB = build/libresourceqt.so.1.0.0

%.make: %.pro
	$(QMAKE) -o $@ $<

clean: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) clean
	cd tests; $(MAKE) clean
	$(RM) $(MAKEFILE)
	$(RM) -r build

all: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) all
	cd tests; $(MAKE) all

install: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) install
	cd tests; $(MAKE) install
	
tests: force
	cd tests; $(MAKE) tests

force: ;

.phony: clean all install tests 
