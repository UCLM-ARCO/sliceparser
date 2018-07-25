# -*- mode: makefile-gmake; coding: utf-8 -*-

SHELL = /bin/bash

export LD_LIBRARY_PATH=$(shell pwd)/build/lib.*
export PYTHONPATH=$(shell pwd)/build/lib.*

all:


.PHONY: test
test: all
	-find test -name "*.py" | xargs nosetests3 -s


.PHONY: clean
clean:
	@for pat in "*.so" "*.o" "*~" "*.pyc"; do \
	    find . -name $$pat -print -delete; \
	done
	@$(RM) build -rf
