# -*- mode: python; coding: utf-8 -*-

from SliceParser import (
    Parser, Module, ClassDef, Operation, OperationMode,
    ContainedType, returnTypeToString
)


class Visitor:
    def visitModuleStart(self, module):
        # Note: received a copy of parameter, don't use it after return
        return True

    def visitModuleEnd(self, module):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitClassDefStart(self, classdef):
        # Note: received a copy of parameter, don't use it after return
        return True

    def visitClassDefEnd(self, classdef):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitExceptionStart(self, exception):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitExceptionEnd(self, exception):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitStructStart(self, struct):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitStructEnd(self, struct):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitOperation(self, operation):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitParamDecl(self, paramdecl):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitDataMember(self, datamember):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitSequence(self, sequence):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitDictionary(self, dictionary):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitEnum(self, enum):
        # Note: received a copy of parameter, don't use it after return
        pass

    def visitConst(self, const):
        # Note: received a copy of parameter, don't use it after return
        pass
