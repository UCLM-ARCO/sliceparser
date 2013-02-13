# -*- mode: python; coding: utf-8 -*-

from unittest import TestCase
import tempfile
from doublex import Spy, ANY_ARG, assert_that, called, is_
from hamcrest import has_length, is_not

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
        assert_that(list(classdef.allBases())[0].name(), is_("IfaceA"))


class TestVisitor_OperationMode(TestBase):
    def test_operation_mode_normal_is_0(self):
        assert_that(sp.OperationMode.Normal, is_(0))

    def test_operation_mode_nonmutating_is_1(self):
        assert_that(sp.OperationMode.Nonmutating, is_(1))

    def test_operation_mode_idempotent_is_2(self):
        assert_that(sp.OperationMode.Idempotent, is_(2))


class TestVisitor_Type(TestBase):
    code = ("module A {                            \n"
            "  interface Iface1 {                  \n"
            "    int start();                      \n"
            "  };                                  \n"
            "  local interface Iface2 {            \n"
            "    Iface2 getIface();                \n"
            "  };                                  \n"
            "};                                    \n")

    def test_type_isLocal_when_non_local(self):
        type = self.get_type()
        assert_that(type.isLocal(), is_(False))

    def test_type_isLocal_when_local(self):
        type = self.get_type(1)
        assert_that(type.isLocal(), is_(True))

    def test_type_typeId_builtin_type(self):
        type = self.get_type()
        assert_that(type.typeId(), is_("int"))

    def test_type_typeId_user_defined(self):
        type = self.get_type(1)
        assert_that(type.typeId(), is_("::A::Iface2"))

    def test_type_usesClasses(self):
        type = self.get_type()
        assert_that(type.usesClasses(), is_(False))

    def test_type_minWireSize(self):
        type = self.get_type()
        assert_that(type.minWireSize(), is_(4))

    def test_type_isVariableLength(self):
        type = self.get_type()
        assert_that(type.isVariableLength(), is_(False))

    def get_type(self, id=0):
        operation = self.visitor.visitOperation.calls[id].args[0]
        return operation.returnType()


class TestVisitor_MinimalOperation(TestBase):
    code = ("module A {                            \n"
            "  interface Iface {                   \n"
            "    void start();                     \n"
            "  };                                  \n"
            "};                                    \n")

    def test_visit_operation(self):
        assert_that(self.visitor.visitOperation, called().times(1))

    def test_visited_operation_name_correct(self):
        operation = self.get_operation()
        assert_that(operation.name(), is_("start"))

    def test_visited_operation_mode_correct(self):
        operation = self.get_operation()
        assert_that(operation.mode(), is_(sp.OperationMode.Normal))

    def test_visited_operation_scope_correct(self):
        operation = self.get_operation()
        assert_that(operation.scope(), is_("::A::Iface::"))

    def test_visited_operation_return_type_correct(self):
        operation = self.get_operation()
        assert_that(operation.returnType(), is_(None))

    def get_operation(self):
        return self.visitor.visitOperation.calls[0].args[0]


class TestVisitor_Operation_MetaData(TestBase):
    code = ('module A {                                \n'
            '  interface Iface {                       \n'
            '    ["amd"] void method();                \n'
            '    ["amd", "protected"] void method2();  \n'
            '  };                                      \n'
            '};                                        \n')

    def test_operation_has_metadata(self):
        meta_data = self.get_operation_metadata()
        assert_that(len(meta_data), is_(1))

    def test_operation_metadata_has_proper_items(self):
        meta_data = self.get_operation_metadata(1)
        assert_that(list(meta_data), is_(['amd', 'protected']))

    def get_operation_metadata(self, opid=0):
        operation = self.visitor.visitOperation.calls[opid].args[0]
        return operation.getMetaData()


class TestVisitor_Operation_Parameters(TestBase):
    code = ('module A {                                        \n'
            '  interface Iface {                               \n'
            '    void method1();                               \n'
            '    void method2(bool param1, float param2);      \n'
            '    void method3(Object* param1);                 \n'
            '    void method4(Iface param1);                   \n'
            '  };                                              \n'
            '};                                                \n')

    def test_operation_has_parameters(self):
        parameters = self.get_operation_parameters()
        assert_that(len(parameters), is_(0))

    def test_operation_has_correct_number_of_parameters(self):
        parameters = self.get_operation_parameters(1)
        assert_that(len(parameters), is_(2))

    def get_operation_parameters(self, opid=0):
        operation = self.visitor.visitOperation.calls[opid].args[0]
        return list(operation.parameters())


class TestVisitor_ParamDecl(TestBase):
    code = ('module A {                                        \n'
            '  interface Iface {                               \n'
            '    void method1(out bool param1);                \n'
            '    void method2(Object* param1);                 \n'
            '    void method3(Iface param1);                   \n'
            '  };                                              \n'
            '};                                                \n')

    def test_paramdecl_name(self):
        param = self.get_param_decl()
        assert_that(param.name(), is_("param1"))

    def test_paramdecl_getMetaData(self):
        param = self.get_param_decl()
        assert_that(list(param.getMetaData()), is_([]))

    def test_paramdecl_type(self):
        param = self.get_param_decl()
        assert_that(param.type(), is_not(None))

    def test_paramdecl_isOutParam(self):
        param = self.get_param_decl()
        assert_that(param.isOutParam(), is_(True))

    def test_paramdecl_containedType(self):
        param = self.get_param_decl()
        assert_that(param.containedType(), is_(sp.ContainedType.ContainedTypeDataMember))

    def test_paramdecl_uses__negative_result(self):
        param = self.get_param_decl()
        module = self.visitor.visitModuleStart.calls[0].args[0]
        assert_that(param.uses(module), is_(False))

    def test_paramdecl_uses__positive_result(self):
        param = self.get_param_decl(2)
        classdef = self.visitor.visitClassDefStart.calls[0].args[0]
        assert_that(param.uses(classdef), is_(True))

    def test_paramdecl_kindOf(self):
        param = self.get_param_decl()
        assert_that(param.kindOf(), is_("parameter declaration"))

    def get_param_decl(self, opid=0):
        operation = self.visitor.visitOperation.calls[opid].args[0]
        return list(operation.parameters())[0]


class TestVisitor_Functions(TestBase):
    code = ('module A {                                        \n'
            '  interface Iface1 {                              \n'
            '    int method1();                                \n'
            '  };                                              \n'
            '  local interface Iface2 {                        \n'
            '    Iface1 method1();                             \n'
            '  };                                              \n'
            '};                                                \n')

    def test_returnTypeToString_without_metaData_or_typeCtx(self):
        operation = self.visitor.visitOperation.calls[1].args[0]
        type = operation.returnType()
        assert_that(sp.returnTypeToString(type), is_("::A::Iface1Ptr"))

    def test_returnTypeToString_without_metaData(self):
        operation = self.visitor.visitOperation.calls[1].args[0]
        type = operation.returnType()
        meta_data = operation.getMetaData()
        assert_that(sp.returnTypeToString(type, meta_data), is_("::A::Iface1Ptr"))

    def test_returnTypeToString(self):
        operation = self.visitor.visitOperation.calls[1].args[0]
        type = operation.returnType()
        meta_data = operation.getMetaData()
        assert_that(sp.returnTypeToString(type, meta_data, 2), is_("::A::Iface1Ptr"))
