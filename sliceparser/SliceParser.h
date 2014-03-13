/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Parser.h>
#include <boost/shared_ptr.hpp>

namespace SliceParser {

    class Visitor : public Slice::ParserVisitor {
    public:
	Visitor(boost::python::object pyVisitor);

	bool visitUnitStart(const Slice::UnitPtr&);
	void visitUnitEnd(const Slice::UnitPtr&);
	bool visitModuleStart(const Slice::ModulePtr&);
	void visitModuleEnd(const Slice::ModulePtr&);
	void visitClassDecl(const Slice::ClassDeclPtr&);
	bool visitClassDefStart(const Slice::ClassDefPtr&);
	void visitClassDefEnd(const Slice::ClassDefPtr&);
	bool visitExceptionStart(const Slice::ExceptionPtr&);
	void visitExceptionEnd(const Slice::ExceptionPtr&);
	bool visitStructStart(const Slice::StructPtr&);
	void visitStructEnd(const Slice::StructPtr&);
	void visitOperation(const Slice::OperationPtr&);
	void visitParamDecl(const Slice::ParamDeclPtr&);
	void visitDataMember(const Slice::DataMemberPtr&);
	void visitSequence(const Slice::SequencePtr&);
	void visitDictionary(const Slice::DictionaryPtr&);
	void visitEnum(const Slice::EnumPtr&);
	void visitConst(const Slice::ConstPtr&);

    private:
	boost::python::object _pyVisitor;
    };

    typedef boost::shared_ptr<Visitor> VisitorPtr;

    class Parser {
    public:
	Parser(std::string filename);
	void accept(boost::python::object);

    private:
	std::string _filename;
    };

}
