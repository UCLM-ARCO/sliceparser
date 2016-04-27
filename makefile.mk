# -*- mode: makefile-gmake; coding: utf-8 -*-

all:


.PHONY: test
test: all
	-find test -name "*.py" | xargs nosetests -s


.PHONY: clean
clean:
	@for pat in "*.so" "*.o" "*~" "*.pyc"; do \
	    find . -name $$pat -print -delete; \
	done
	@$(RM) build -rf
