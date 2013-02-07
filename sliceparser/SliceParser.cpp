/* -*- mode: c++; coding: utf-8 -*- */

#include <boost/python.hpp>

#include "SliceParser.h"

using namespace ec::py;


namespace SliceParser {

Parser::Parser(std::string filename) {
}

void
Parser::accept(PyObject* visitor) {
    // this should create a new C++ visitor, and pass this to it
    // C++ visitor will forward each invocation to this visitor
    PyObjectPtr visitorptr(visitor);
}

bool
Visitor::visitModuleStart(const Slice::ModulePtr&) {
    std::cout << "C++ call of visitModuleStart" << std::endl;
}

}


using namespace boost::python;

BOOST_PYTHON_MODULE(SliceParser) {
    class_<SliceParser::Parser>("Parser", init<std::string>())
        .def("accept", &SliceParser::Parser::accept)
    ;

    class_<SliceParser::Visitor>("Visitor");
};
