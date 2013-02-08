/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Parser.h>
#include <ec/PyHandle.h>
#include <boost/shared_ptr.hpp>

namespace SliceParser {

    class Visitor : public Slice::ParserVisitor {
    public:
	Visitor(ec::py::PyObjectPtr py_visitor);
	// ~ParserVisitor() { }

	bool visitUnitStart(const Slice::UnitPtr&);
	void visitUnitEnd(const Slice::UnitPtr&);
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

    private:
	ec::py::PyObjectPtr _pyVisitor;
    };

    typedef boost::shared_ptr<Visitor> VisitorPtr;

    class Parser {
    public:
	Parser(std::string filename);
	void accept(PyObject*);

    private:
	std::string _filename;
    };

}
