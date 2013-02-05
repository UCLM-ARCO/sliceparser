# -*- mode: makefile-gmake; coding: utf-8 -*-

all:
	python setup.py build
	cp $$(find build/ -name "*.so") sliceparser

.PHONY: clean
clean:
	@for pat in "*.o" "*~" "*.pyc"; do \
	    find . -name $$pat -print -delete; \
	done
	@$(RM) build -rf

