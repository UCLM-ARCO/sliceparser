/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Preprocessor.h>
#include <boost/python.hpp>
#include <ec/PyFunctions.h>

#include "SliceParser.h"


namespace SliceParser {

Parser::Parser(std::string filename) :
    _filename(filename) {
}

void
Parser::accept(PyObject* py_visitor) {
    VisitorPtr visitor(new Visitor(py_visitor));

    std::string path = "";
    std::vector<std::string> args;
    Slice::PreprocessorPtr icepp =
	Slice::Preprocessor::create(path, _filename, args);

    bool keepComments = false;
    FILE* cppHandle = icepp->preprocess(keepComments);

    bool ignRedefs = false;
    bool all = false;
    bool allowIcePrefix = false;
    bool allowUnderscore = false;
    Slice::StringList defaultGlobalMetadata;
    Slice::UnitPtr unit = Slice::Unit::createUnit(
	 ignRedefs,
	 all,
	 allowIcePrefix,
	 allowUnderscore,
	 defaultGlobalMetadata);

    bool debug = false;
    unit->parse(_filename, cppHandle, debug);
    unit->visit(visitor.get(), true);
}

Visitor::Visitor(ec::py::PyObjectPtr pyVisitor) :
    _pyVisitor(pyVisitor) {
}

bool
Visitor::visitUnitStart(const Slice::UnitPtr&) {
    // ec::py::PyObjectPtr result = callMethod(_pyVisitor, "visitUnitStart", args);
    // return getAsBool(result);
}

void
Visitor::visitUnitEnd(const Slice::UnitPtr&) {
    std::cout << "C++ call of visitUnitEnd" << std::endl;
}

bool
Visitor::visitModuleStart(const Slice::ModulePtr&) {
    std::cout << "C++ call of visitModuleStart" << std::endl;
    return true;
}

} // namespace SliceParser


using namespace boost::python;
BOOST_PYTHON_MODULE(SliceParser) {
    class_<SliceParser::Parser>("Parser", init<std::string>())
        .def("accept", &SliceParser::Parser::accept)
    ;
};
