#!/usr/bin/python
# -*- mode: python; coding: utf-8 -*-

from distutils.core import setup, Extension


def get_version():
    with open('debian/changelog') as chlog:
        return chlog.readline().split()[1][1:-1]


sliceParser = Extension(
    name         = 'SliceParser',
    sources      = ['sliceparser/SliceParser.cpp'],
    libraries    = ['boost_python', 'Slice'],
)

setup(
    name         = 'sliceparser',
    version      = get_version(),
    description  = 'Slice parser library for Python',

    ext_modules  = [sliceParser],
    packages     = ["sliceparser"],
)
