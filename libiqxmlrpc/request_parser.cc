//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "except.h"
#include "request_parser.h"
#include "value_parser.h"
#include <iostream>

namespace iqxmlrpc {

enum RequestBuilderState {
  NONE,
  METHOD_CALL,
  METHOD_NAME,
  PARAMS,
  PARAM,
  VALUE
};

RequestBuilder::RequestBuilder(Parser& parser):
  BuilderBase(parser),
  state_(parser, NONE)
{
  static const StateMachine::StateTransition trans[] = {
    { NONE, METHOD_CALL, "methodCall" },
    { METHOD_CALL, METHOD_NAME, "methodName" },
    { METHOD_NAME, PARAMS, "params" },
    { PARAMS, PARAM, "param" },
    { PARAM, VALUE, "value" },
    { VALUE, PARAM, "param" },
    { 0, 0, 0 }
  };
  state_.set_transitions(trans);
}

void
RequestBuilder::do_visit_element(const std::string& tagname)
{
  switch (state_.change(tagname)) {
  case METHOD_NAME:
    method_name_ = parser_.get_data();
    break;

  case VALUE:
    params_.push_back(sub_build<Value_type*, ValueBuilder>());
    break;
  }
}

Request*
RequestBuilder::get()
{
  return new Request(method_name_, params_);
}

} // namespace iqxmlrpc
