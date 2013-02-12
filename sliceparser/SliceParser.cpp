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

void
Visitor::visitOperation(const Slice::OperationPtr& operation) {
    _pyVisitor.attr("visitOperation")(operation.get());
}

} // namespace SliceParser


using namespace boost::python;
typedef std::vector<Slice::ClassDef*> ClassVect;

/*
 * Remove Ice smart pointers from ClassDef
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

/*
 * Remove Ice smart pointer
 */
template<class T>
struct PtrConverter {
    static PyObject* convert(::IceUtil::Handle<T> const& ptr) {
        return incref(object(boost::cref(ptr.get())).ptr());
    }
};

BOOST_PYTHON_MODULE(SliceParser) {

    to_python_converter< Slice::TypePtr, PtrConverter<Slice::Type> >();

    class_<SliceParser::Parser>("Parser", init<std::string>())
        .def("accept", &SliceParser::Parser::accept)
    ;

    class_<Slice::Module>("Module", no_init)
	.def("name", &Slice::Module::name)
	.def("scope", &Slice::Module::scope)
    ;

    typedef Slice::ClassDef* const& (ClassVect::*const_at)(unsigned int) const;
    class_<ClassVect>("_ClassVect", no_init)
	.def("__iter__", iterator< ClassVect >())
	.def("__len__", &ClassVect::size)
	.def("__getitem__", const_at(&ClassVect::at),
	     return_internal_reference<>())
    ;

    class_<Slice::ClassDef, Slice::ClassDef*>("ClassDef", no_init)
	.def("name", &Slice::ClassDef::name)
	.def("scope", &Slice::ClassDef::scope)
	.def("allBases", allBasesWrapp)
    ;

    enum_<Slice::Operation::Mode>("OperationMode")
	.value("Normal", Slice::Operation::Normal)
	.value("Nonmutating", Slice::Operation::Nonmutating)
	.value("Idempotent", Slice::Operation::Idempotent)
	.export_values()
    ;

    class_<Slice::Type, boost::noncopyable>("Type", no_init)
	.def("isLocal", &Slice::Type::isLocal)
	.def("typeId", &Slice::Type::typeId)
	.def("usesClasses", &Slice::Type::usesClasses)
	.def("minWireSize", &Slice::Type::minWireSize)
	.def("isVariableLength", &Slice::Type::isVariableLength)
    ;

    class_<Slice::Operation>("Operation", no_init)
	.def("name", &Slice::Operation::name)
	.def("mode", &Slice::Operation::mode)
	.def("scope", &Slice::Operation::scope)
	.def("returnType", &Slice::Operation::returnType)
    ;

};

