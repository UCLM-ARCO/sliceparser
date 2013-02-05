/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Parser.h>

namespace SliceParser {

    class Visitor : public Slice::ParserVisitor {
	// ~ParserVisitor() { }
	// bool visitUnitStart(const UnitPtr&);
	// void visitUnitEnd(const UnitPtr&);
	// bool visitModuleStart(const ModulePtr&);
	// void visitModuleEnd(const ModulePtr&);
	// void visitClassDecl(const ClassDeclPtr&);
	// bool visitClassDefStart(const ClassDefPtr&);
	// void visitClassDefEnd(const ClassDefPtr&);
	// bool visitExceptionStart(const ExceptionPtr&);
	// void visitExceptionEnd(const ExceptionPtr&);
	// bool visitStructStart(const StructPtr&);
	// void visitStructEnd(const StructPtr&);
	// void visitOperation(const OperationPtr&);
	// void visitParamDecl(const ParamDeclPtr&);
	// void visitDataMember(const DataMemberPtr&);
	// void visitSequence(const SequencePtr&);
	// void visitDictionary(const DictionaryPtr&);
	// void visitEnum(const EnumPtr&);
	// void visitConst(const ConstPtr&);
    };

    class Parser {
	void accept(Visitor& v);
    };

}
