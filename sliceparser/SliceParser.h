/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Parser.h>
#include <ec/PyHandle.h>

namespace SliceParser {

    class Visitor : public Slice::ParserVisitor {
    public:
	// ~ParserVisitor() { }
	// bool visitUnitStart(const UnitPtr&);
	// void visitUnitEnd(const UnitPtr&);
	bool visitModuleStart(const Slice::ModulePtr&);
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
    public:
	Parser(std::string filename);
	void accept(PyObject*);
    };

}
