QMAKE = qmake
MAKEFILE = libresourceqt.make
LIB = build/libresourceqt.so.1.0.0

%.make: %.pro
	$(QMAKE) -o $@ $<

all: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) all
	$(MAKE) -C tests all

clean:
	$(RM) $(MAKEFILE)
	$(RM) -r build	
	$(MAKE) -C tests clean

install: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) install
	$(MAKE) -C tests install
	
tests: force
	$(MAKE) -C tests tests

force: ;

.phony: clean all install tests 
