SUBDIRS=src test

all: $(SUBDIRS)

#
# Main targets
#
.PHONY: $(SUBDIRS) clean distclean depclean

$(SUBDIRS) or_no_subdirs:
	(cd $@ && $(MAKE) all)

test: src

check: $(SUBDIRS)
	(cd test && $(MAKE) check)

#
# Clean targets
#
clean:
	@if test -n "$(SUBDIRS)"; then \
	    for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) $@); done \
	fi
	$(RM) $(OBJ) $(PROGRAM) $(LIBRARY) $(ARCHIVE) $(TESTS)

distclean:
	@if test -n "$(SUBDIRS)"; then \
	    for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) $@); done \
	fi
	$(RM) $(OBJ) $(PROGRAM) $(LIBRARY) $(ARCHIVE) $(TESTS)
	$(RM) $(DEP)
	$(RM) -r $(DISTCLEAN_FILES) *~
