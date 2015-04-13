# -*- mode: makefile-gmake; coding: utf-8 -*-

all:
	python setup.py build
	cp $$(find build/ -name "*.so") sliceparser


# FIXME: use distutils properly
install: DST = $(DESTDIR)/usr/share/pyshared/sliceparser
install:
	install -d $(DST)
	cp sliceparser/*.py $(DST)
	cp sliceparser/*.so $(DST)


.PHONY: test
test: all
	-find test -name "*.py" | xargs nosetests -s


.PHONY: clean
clean:
	@for pat in "*.so" "*.o" "*~" "*.pyc"; do \
	    find . -name $$pat -print -delete; \
	done
	@$(RM) build -rf
