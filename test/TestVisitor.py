# -*- mode: python; coding: utf-8 -*-

from unittest import TestCase
import tempfile
from doublex import Spy, ANY_ARG, assert_that, called, is_
from hamcrest import has_length

import sliceparser as sp


class TempFile(object):
    def __init__(self, contents, suffix=""):
        self.contents = contents
        self.suffix = suffix

    def __enter__(self):
        self.file = tempfile.NamedTemporaryFile(suffix=self.suffix)
        self.file.write(self.contents)
        self.file.flush()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.file.close()

    @property
    def path(self):
        return self.file.name


class TestBase(TestCase):
    code = ""

    def setUp(self):
        with Spy(sp.Visitor) as spy:
            spy.visitModuleStart(ANY_ARG).returns(True)
            spy.visitClassDefStart(ANY_ARG).returns(True)
            self.visitor = spy

        with TempFile(self.code, suffix=".ice") as src:
            parser = sp.Parser(src.path)
            parser.accept(self.visitor)


class TestVisitor_Simplest(TestBase):
    code = "module A {};"

    def test_visit_module_start(self):
        assert_that(self.visitor.visitModuleStart, called().times(1))

    def test_visit_module_end(self):
        assert_that(self.visitor.visitModuleEnd, called().times(1))

    def test_visited_module_name_correct(self):
        module = self.visitor.visitModuleStart.calls[0].args[0]
        assert_that(module.name(), is_("A"))

    def test_visited_module_scope_correct(self):
        module = self.visitor.visitModuleStart.calls[0].args[0]
        assert_that(module.scope(), is_("::"))


class TestVisitor_NestedModule(TestBase):
    code = ("module A {    \n"
            "  module B {  \n"
            "  };          \n"
            "};            \n")

    def test_visit_module_start(self):
        assert_that(self.visitor.visitModuleStart, called().times(2))

    def test_visit_module_end(self):
        assert_that(self.visitor.visitModuleEnd, called().times(2))

    def test_visited_outer_module_name_correct(self):
        moduleA = self.visitor.visitModuleStart.calls[0].args[0]
        assert_that(moduleA.name(), is_("A"))

    def test_visited_inner_module_name_correct(self):
        moduleB = self.visitor.visitModuleStart.calls[1].args[0]
        assert_that(moduleB.name(), is_("B"))

    def test_visited_outer_module_scope_correct(self):
        moduleA = self.visitor.visitModuleStart.calls[0].args[0]
        assert_that(moduleA.scope(), is_("::"))

    def test_visited_inner_module_scope_correct(self):
        moduleB = self.visitor.visitModuleStart.calls[1].args[0]
        assert_that(moduleB.scope(), is_("::A::"))


class TestVisitor_Interface(TestBase):
    code = ("module A {              \n"
            "  interface Iface {     \n"
            "  };                    \n"
            "};                      \n")

    def test_visit_class_def_start(self):
        assert_that(self.visitor.visitClassDefStart, called().times(1))

    def test_visit_class_def_end(self):
        assert_that(self.visitor.visitClassDefEnd, called().times(1))

    def test_visited_classdef_name_correct(self):
        classdef = self.visitor.visitClassDefStart.calls[0].args[0]
        assert_that(classdef.name(), is_("Iface"))

    def test_visited_classdef_scope_correct(self):
        classdef = self.visitor.visitClassDefStart.calls[0].args[0]
        assert_that(classdef.scope(), is_("::A::"))

    def test_visited_classdef_basses_correct(self):
        classdef = self.visitor.visitClassDefStart.calls[0].args[0]
        assert_that(classdef.allBases(), has_length(0))


class TestVisitor_InterfaceInheritance(TestBase):
    code = ("module A {                            \n"
            "  interface IfaceA {                  \n"
            "  };                                  \n"
            "  interface IfaceB extends IfaceA {   \n"
            "  };                                  \n"
            "};                                    \n")

    def test_visited_classdef_basses_correct(self):
        classdef = self.visitor.visitClassDefStart.calls[1].args[0]
        assert_that(classdef.allBases(), has_length(1))
        assert_that(classdef.name(), is_("IfaceB"))
        assert_that(classdef.allBases()[0].name(), is_("IfaceA"))
