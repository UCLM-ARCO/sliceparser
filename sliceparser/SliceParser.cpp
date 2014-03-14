/* -*- mode: c++; coding: utf-8 -*- */

#include <Slice/Preprocessor.h>
#include <Slice/CPlusPlusUtil.h>
#include <boost/python.hpp>

#include "SliceParser.h"

namespace py = boost::python;

namespace SliceParser {

Parser::Parser(std::string filename) :
    _filename(filename) {
}

void
Parser::addArgument(std::string arg) {
    _args.push_back(arg);
}

void
Parser::accept(py::object py_visitor) {
    VisitorPtr visitor(new Visitor(py_visitor));

    std::string path = "";
    Slice::PreprocessorPtr icepp =
	Slice::Preprocessor::create(path, _filename, _args);

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
Visitor::visitUnitStart(const Slice::UnitPtr& unit) {
    return _pyVisitor.attr("visitUnitStart")(unit.get());
}

void
Visitor::visitUnitEnd(const Slice::UnitPtr& unit) {
    _pyVisitor.attr("visitUnitEnd")(unit.get());
}

bool
Visitor::visitModuleStart(const Slice::ModulePtr& module) {
    return _pyVisitor.attr("visitModuleStart")(module.get());
}

void
Visitor::visitModuleEnd(const Slice::ModulePtr& module) {
    _pyVisitor.attr("visitModuleEnd")(module.get());
}

void
Visitor::visitClassDecl(const Slice::ClassDeclPtr& classdecl) {
    _pyVisitor.attr("visitClassDecl")(classdecl.get());
}

bool
Visitor::visitClassDefStart(const Slice::ClassDefPtr& classdef) {
    return _pyVisitor.attr("visitClassDefStart")(classdef.get());
}

void
Visitor::visitClassDefEnd(const Slice::ClassDefPtr& classdef) {
    _pyVisitor.attr("visitClassDefEnd")(classdef.get());
}

bool
Visitor::visitExceptionStart(const Slice::ExceptionPtr& exception) {
    return _pyVisitor.attr("visitExceptionStart")(exception.get());
}

void
Visitor::visitExceptionEnd(const Slice::ExceptionPtr& exception) {
    _pyVisitor.attr("visitExceptionEnd")(exception.get());
}

bool
Visitor::visitStructStart(const Slice::StructPtr& struct_) {
    return _pyVisitor.attr("visitStructStart")(struct_.get());
}

void
Visitor::visitStructEnd(const Slice::StructPtr& struct_) {
    _pyVisitor.attr("visitStructEnd")(struct_.get());
}

void
Visitor::visitOperation(const Slice::OperationPtr& operation) {
    _pyVisitor.attr("visitOperation")(operation.get());
}

void
Visitor::visitParamDecl(const Slice::ParamDeclPtr& paramdecl) {
    _pyVisitor.attr("visitParamDecl")(paramdecl.get());
}

void
Visitor::visitDataMember(const Slice::DataMemberPtr& datamember) {
    _pyVisitor.attr("visitDataMember")(datamember.get());
}

void
Visitor::visitSequence(const Slice::SequencePtr& sequence) {
    _pyVisitor.attr("visitSequence")(sequence.get());
}

void
Visitor::visitDictionary(const Slice::DictionaryPtr& dictionary) {
    _pyVisitor.attr("visitDictionary")(dictionary.get());
}

void
Visitor::visitEnum(const Slice::EnumPtr& enum_) {
    _pyVisitor.attr("visitEnum")(enum_.get());
}

void
Visitor::visitConst(const Slice::ConstPtr& const_) {
    _pyVisitor.attr("visitConst")(const_.get());
}

} // namespace SliceParser


using namespace boost::python;

/*
 * Remove Ice smart pointer for to_python conversion
 */
template< class T >
struct RemovePtr {
    static PyObject* convert(::IceUtil::Handle< T > const& handle) {
        return incref(object(boost::cref(handle.get())).ptr());
    }
};

/*
 * Common bindings for std::list types
 */
template < class T >
void classList(std::string name) {
    class_< T >(name.c_str(), no_init)
	.def("__iter__", iterator< T >())
	.def("__len__", &T::size)
    ;
}

/*
 * Append Ice smart pointer for from_python conversion
 */
bool
ParamDecl_uses(const Slice::ParamDecl& self, Slice::Contained& contained) {
    Slice::ContainedPtr container_ptr = Slice::ContainedPtr::dynamicCast(&contained);
    container_ptr->__setNoDelete(true);
    return self.uses(container_ptr);
}

/*
 * Append Ice smart pointer for from_python conversion
 * Add "thin wrappers" for default arguments
 */
std::string
returnTypeToString_1(Slice::Type& type, bool optional) {
    Slice::TypePtr type_ptr = Slice::TypePtr::dynamicCast(&type);
    type_ptr->__setNoDelete(true);
    return Slice::returnTypeToString(type_ptr, optional);
}

std::string
returnTypeToString_2(Slice::Type& type, bool optional, Slice::StringList& meta_data) {
    Slice::TypePtr type_ptr = Slice::TypePtr::dynamicCast(&type);
    type_ptr->__setNoDelete(true);
    return Slice::returnTypeToString(type_ptr, optional, meta_data);
}

std::string
returnTypeToString_3(Slice::Type& type, bool optional,
		     Slice::StringList& meta_data, int type_ctx) {

    Slice::TypePtr type_ptr = Slice::TypePtr::dynamicCast(&type);
    type_ptr->__setNoDelete(true);
    return Slice::returnTypeToString(type_ptr, optional, meta_data, type_ctx);
}

BOOST_PYTHON_MODULE(SliceParser) {

    to_python_converter< Slice::UnitPtr, RemovePtr< Slice::Unit > >();
    to_python_converter< Slice::ParamDeclPtr, RemovePtr< Slice::ParamDecl > >();
    to_python_converter< Slice::ClassDefPtr, RemovePtr< Slice::ClassDef > >();
    to_python_converter< Slice::TypePtr, RemovePtr< Slice::Type > >();
    to_python_converter< Slice::EnumeratorPtr, RemovePtr< Slice::Enumerator > >();
    to_python_converter< Slice::DataMemberPtr, RemovePtr< Slice::DataMember > >();

    enum_< Slice::Operation::Mode >("OperationMode")
	.value("Normal", Slice::Operation::Normal)
	.value("Nonmutating", Slice::Operation::Nonmutating)
	.value("Idempotent", Slice::Operation::Idempotent)
	.export_values()
    ;

    enum_< Slice::Contained::ContainedType >("ContainedType")
	.value("ContainedTypeSequence", Slice::Contained::ContainedTypeSequence)
        .value("ContainedTypeDictionary", Slice::Contained::ContainedTypeDictionary)
        .value("ContainedTypeEnum", Slice::Contained::ContainedTypeEnum)
        .value("ContainedTypeEnumerator", Slice::Contained::ContainedTypeEnumerator)
        .value("ContainedTypeModule", Slice::Contained::ContainedTypeModule)
        .value("ContainedTypeClass", Slice::Contained::ContainedTypeClass)
        .value("ContainedTypeException", Slice::Contained::ContainedTypeException)
        .value("ContainedTypeStruct", Slice::Contained::ContainedTypeStruct)
        .value("ContainedTypeOperation", Slice::Contained::ContainedTypeOperation)
        .value("ContainedTypeParamDecl", Slice::Contained::ContainedTypeParamDecl)
        .value("ContainedTypeDataMember", Slice::Contained::ContainedTypeDataMember)
        .value("ContainedTypeConstant", Slice::Contained::ContainedTypeConstant)
	.export_values()
    ;

    classList< Slice::ClassList >("ClassList");
    classList< Slice::StringList >("StringList");
    classList< Slice::ParamDeclList >("ParamDeclList");
    classList< Slice::EnumeratorList >("EnumeratorList");
    classList< Slice::DataMemberList >("DataMemberList");

    class_< SliceParser::Parser >("Parser", init< std::string >())
	.def("addArgument", &SliceParser::Parser::addArgument)
        .def("accept", &SliceParser::Parser::accept)
    ;

    class_< Slice::Contained, boost::noncopyable >("Contained", no_init)
	.def("name", &Slice::Contained::name)
	.def("scope", &Slice::Contained::scope)
	.def("getMetaData", &Slice::Contained::getMetaData)
    ;

    class_< Slice::Type, boost::noncopyable >("Type", no_init)
	.def("isLocal", &Slice::Type::isLocal)
	.def("typeId", &Slice::Type::typeId)
	.def("usesClasses", &Slice::Type::usesClasses)
	.def("minWireSize", &Slice::Type::minWireSize)
	.def("isVariableLength", &Slice::Type::isVariableLength)
    ;

    class_< Slice::Unit >("Unit", no_init)
    ;

    class_< Slice::Module, bases<Slice::Contained> >("Module", no_init)
    ;

    class_< Slice::Enumerator, bases<Slice::Contained> >("Enumerator", no_init)
    ;

    class_< Slice::Enum, bases<Slice::Contained> >("Enum", no_init)
	.def("getEnumerators", &Slice::Enum::getEnumerators)
    ;

    class_< Slice::Sequence, bases<Slice::Contained> >("Sequence", no_init)
	.def("type", &Slice::Sequence::type)
    ;

    class_< Slice::DataMember, bases<Slice::Contained> >("DataMember", no_init)
	.def("type", &Slice::DataMember::type)
    ;

    class_< Slice::Struct, bases<Slice::Type, Slice::Contained> >("Struct", no_init)
	.def("dataMembers", &Slice::Struct::dataMembers)
	.def("typeId", &Slice::Struct::typeId)
    ;

    class_< Slice::ClassDecl >("ClassDecl", no_init)
    ;

    class_< Slice::ClassDef, bases<Slice::Contained> >("ClassDef", no_init)
	.def("allBases", &Slice::ClassDef::allBases)
    ;

    class_< Slice::Exception, bases<Slice::Contained> >("Exception", no_init)
    ;

    class_< Slice::Operation, bases<Slice::Contained> >("Operation", no_init)
	.def("mode", &Slice::Operation::mode)
	.def("returnType", &Slice::Operation::returnType)
	.def("parameters", &Slice::Operation::parameters)
    ;

    class_< Slice::ParamDecl, bases<Slice::Contained> >("ParamDecl", no_init)
	.def("type", &Slice::ParamDecl::type)
	.def("isOutParam", &Slice::ParamDecl::isOutParam)
	.def("containedType", &Slice::ParamDecl::containedType)
	.def("uses", &ParamDecl_uses)
	.def("kindOf", &Slice::ParamDecl::kindOf)
    ;

    def("returnTypeToString", &returnTypeToString_1);
    def("returnTypeToString", &returnTypeToString_2);
    def("returnTypeToString", &returnTypeToString_3);

};

