# -*- mode: python; coding: utf-8 -*-

import tempfile
from doublex import Spy, assert_that, called

import sliceparser as sp


class Source(object):
    def __init__(self, contents):
        self.contents = contents

    def __enter__(self):
        self.file = tempfile.NamedTemporaryFile()
        self.file.write(self.contents)
        return self.file

    def __exit__(self, exc_type, exc_value, traceback):
        self.file.close()


class TestVisitor:
    def test_visit_module(self):
        visitor = sp.Visitor()
        parser = sp.Parser("")
        parser.accept(visitor)

        # with Source("module Test {};") as src:
        #     parser = sp.Parser(src.name)
        #     parser.accept(visitor)

        # assert_that(visitor.visitModuleStart,
        #             called().with_args(sp.Module('Test')))
