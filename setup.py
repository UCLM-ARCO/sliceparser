#!/usr/bin/python
# -*- mode: python; coding: utf-8 -*-

from distutils.core import setup, Extension

sliceParser = Extension(
    'SliceParser',
    sources = ['sliceparser/SliceParser.cpp'],
    libraries = ['boost_python'],
)

setup(
    name = 'SliceParser',
    version = '1.0',
    description = 'Slice parser library for Python',
    ext_modules = [sliceParser],
)
