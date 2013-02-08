# -*- mode: python; coding: utf-8 -*-

import tempfile
from doublex import Spy, assert_that, called

import sliceparser as sp


class TempFile(object):
    def __init__(self, contents, suffix=""):
        self.contents = contents
        self.suffix = suffix

    def __enter__(self):
        self.file = tempfile.NamedTemporaryFile(suffix=self.suffix)
        self.file.write(self.contents)
        self.file.flush()
        return self.file

    def __exit__(self, exc_type, exc_value, traceback):
        self.file.close()


class TestVisitor:
    def test_visit_module(self):
        visitor = Spy(sp.Visitor)

        with TempFile("module Test {};", suffix=".ice") as src:
            parser = sp.Parser(src.name)
            parser.accept(visitor)

        assert_that(visitor.visitModuleStart,
                    called().with_args(sp.Module('Test')))
