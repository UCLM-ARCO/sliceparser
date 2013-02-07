# -*- mode: python; coding: utf-8 -*-

from SliceParser import Parser


class Visitor:
    def visitModuleStart(self, module):
        print "call to visitModuleStart()"


class Module:
    def __init__(self, name):
        pass
