QMAKE = qmake
MAKEFILE = libresourceqt.make
LIB = build/libresourceqt.so.1.0.0

%.make: %.pro
	$(QMAKE) -o $@ $<

all: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) all
	cd tests; $(MAKE) all

clean:
	$(RM) $(MAKEFILE)
	$(RM) -r build	
	cd tests; $(MAKE) clean

install: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) install
	cd tests; $(MAKE) install
	
tests: force
	cd tests; $(MAKE) tests

force: ;

.phony: clean all install tests 
