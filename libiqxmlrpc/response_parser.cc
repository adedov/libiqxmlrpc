#include <stdexcept>
#include "except.h"
#include "response_parser.h"
#include "value_parser.h"

namespace iqxmlrpc {

enum ResponseBuilderState {
  NONE,
  RESPONSE,
  OK_RESPONSE,
  OK_PARAM,
  OK_PARAM_VALUE,
  FAULT_RESPONSE,
};

ResponseBuilder::ResponseBuilder(Parser& parser):
  BuilderBase(parser),
  state_(parser, NONE)
{
  static const StateMachine::StateTransition trans[] = {
    { NONE, RESPONSE, "methodResponse" },
    { RESPONSE, OK_RESPONSE, "params" },
    { OK_RESPONSE, OK_PARAM, "param" },
    { OK_PARAM, OK_PARAM_VALUE, "value" },
    { RESPONSE, FAULT_RESPONSE, "fault" },
    { 0, 0, 0 }
  };
  state_.set_transitions(trans);
}

void
ResponseBuilder::do_visit_element(const std::string& tagname)
{
  switch (state_.change(tagname)) {
  case OK_PARAM_VALUE:
    parse_ok();
    break;

  case FAULT_RESPONSE:
    parse_fault();
    break;
  }
}

void
ResponseBuilder::parse_ok()
{
  ok_ = sub_build<Value_type*, ValueBuilder>();
}

void
ResponseBuilder::parse_fault()
{
  static const char* fcode = "faultCode";
  static const char* fstr = "faultString";
  Value v = sub_build<Value_type*, ValueBuilder>();

  if (!v.is_struct())
    throw XML_RPC_violation(parser_.context());

  Struct s = v.the_struct();
  if (s.size() != 2 || !s.has_field(fcode) || !s.has_field(fstr))
    throw XML_RPC_violation(parser_.context());

  fault_code_ = s[fcode].get_int();
  fault_str_  = s[fstr].get_string();
}

Response
ResponseBuilder::get()
{
  if (ok_)
    return Response(new Value(ok_.get()));

  if (fault_str_)
    return Response(fault_code_, fault_str_.get());

  throw XML_RPC_violation("No valid response was found");
}

} // namespace iqxmlrpc
