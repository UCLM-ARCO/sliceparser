/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Preprocessor.h>
#include <boost/python.hpp>

#include "SliceParser.h"

namespace py = boost::python;

namespace SliceParser {

Parser::Parser(std::string filename) :
    _filename(filename) {
}

void
Parser::accept(py::object py_visitor) {
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

Visitor::Visitor(py::object pyVisitor) :
    _pyVisitor(pyVisitor) {
}

bool
Visitor::visitModuleStart(const Slice::ModulePtr& module) {
    return _pyVisitor.attr("visitModuleStart")(module.get());
}

void
Visitor::visitModuleEnd(const Slice::ModulePtr& module) {
    _pyVisitor.attr("visitModuleEnd")(module.get());
}

bool
Visitor::visitClassDefStart(const Slice::ClassDefPtr& classdef) {
    return _pyVisitor.attr("visitClassDefStart")(classdef.get());
}

void
Visitor::visitClassDefEnd(const Slice::ClassDefPtr& classdef) {
    _pyVisitor.attr("visitClassDefEnd")(classdef.get());
}

} // namespace SliceParser


typedef std::vector<Slice::ClassDef*> ClassVect;

/*
 * Remove Ice smart pointers from class definitions
 */
ClassVect
allBasesWrapp(Slice::ClassDef s) {
    Slice::ClassList items = s.allBases();
    ClassVect retval;

    for (Slice::ClassList::iterator i = items.begin(); i != items.end(); i++) {
	retval.push_back(i->get());
    }

    return retval;
}

using namespace boost::python;
BOOST_PYTHON_MODULE(SliceParser) {
    class_<SliceParser::Parser>("Parser", init<std::string>())
        .def("accept", &SliceParser::Parser::accept)
    ;

    class_<Slice::Unit>("Unit", no_init)
    ;

    class_<Slice::Module>("Module", no_init)
	.def("name", &Slice::Module::name)
	.def("scope", &Slice::Module::scope)
    ;

    class_<ClassVect>("_ClassVect", no_init)
	.def("__iter__", iterator< ClassVect >())
	.def("__len__", &ClassVect::size)
	// .def("__getitem__", &ClassVect::at)
    ;

    class_<Slice::ClassDef, Slice::ClassDef*>("ClassDef", no_init)
	.def("name", &Slice::ClassDef::name)
	.def("scope", &Slice::ClassDef::scope)
	.def("allBases", allBasesWrapp)
    ;
};
