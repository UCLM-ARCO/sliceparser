/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Parser.h>
#include <boost/shared_ptr.hpp>

namespace SliceParser {

    class Visitor : public Slice::ParserVisitor {
    public:
	Visitor(boost::python::object pyVisitor);

	bool visitModuleStart(const Slice::ModulePtr&);
	void visitModuleEnd(const Slice::ModulePtr&);
	bool visitClassDefStart(const Slice::ClassDefPtr&);
	void visitClassDefEnd(const Slice::ClassDefPtr&);
	void visitOperation(const Slice::OperationPtr&);

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
