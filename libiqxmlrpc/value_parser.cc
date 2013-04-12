//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "except.h"
#include "value_parser.h"

namespace iqxmlrpc {

ValueBuilderBase::ValueBuilderBase(Parser& parser, bool expect_text):
  BuilderBase(parser, expect_text)
{
}

namespace {

class StructBuilder: public ValueBuilderBase {
public:
  StructBuilder(Parser& parser):
    ValueBuilderBase(parser),
    state_(parser, NONE),
    value_(0)
  {
    static const StateMachine::StateTransition trans[] = {
      { NONE, MEMBER, "member" },
      { MEMBER, NAME_READ, "name" },
      { NAME_READ, VALUE_READ, "value" },
      { 0, 0, 0 }
    };
    state_.set_transitions(trans);
    retval.reset(proxy_ = new Struct());
  }

private:
  enum State {
    NONE,
    MEMBER,
    NAME_READ,
    VALUE_READ,
  };

  virtual void
  do_visit_element(const std::string& tagname)
  {
    switch (state_.change(tagname)) {
    case NAME_READ:
      name_ = parser_.get_data();
      break;

    case VALUE_READ:
      value_ = sub_build<Value_type*, ValueBuilder>();
      value_ = value_ ? value_ : new String("");
      break;

    case MEMBER:
      break;

    default:
      throw XML_RPC_violation(parser_.context());
    }
  }

  virtual void
  do_visit_element_end(const std::string& tagname)
  {
    if (tagname == "member") {
      if (state_.get_state() != VALUE_READ) {
        throw XML_RPC_violation(parser_.context());
      }

      Value_ptr v(new Value(value_));
      proxy_->insert(name_, v);
      state_.set_state(NONE);
    }
  }

  StateMachine state_;
  std::string name_;
  Value_type* value_;
  Struct* proxy_;
};

class ArrayBuilder: public ValueBuilderBase {
public:
  ArrayBuilder(Parser& parser):
    ValueBuilderBase(parser),
    state_(parser, NONE),
    proxy_(0)
  {
    static const StateMachine::StateTransition trans[] = {
      { NONE, DATA, "data" },
      { DATA, VALUES, "value" },
      { VALUES, VALUES, "value" },
      { 0, 0, 0 }
    };
    state_.set_transitions(trans);
    retval.reset(proxy_ = new Array());
  }

private:
  enum State {
    NONE,
    DATA,
    VALUES
  };

  virtual void
  do_visit_element(const std::string& tagname)
  {
    if (state_.change(tagname) == VALUES) {
      Value_type* tmp = sub_build<Value_type*, ValueBuilder>();
      tmp = tmp ? tmp : new String("");
      Value_ptr v(new Value(tmp));
      proxy_->push_back(v);
    }
  }

  StateMachine state_;
  Array* proxy_;
};

} // anonymous namespace

enum ValueBuilderState {
  VALUE,
  STRING,
  INT,
  BOOL,
  DOUBLE,
  BINARY,
  TIME,
  STRUCT,
  ARRAY,
  NIL
};

ValueBuilder::ValueBuilder(Parser& parser):
  ValueBuilderBase(parser, true),
  state_(parser, VALUE)
{
  static const StateMachine::StateTransition trans[] = {
    { VALUE,  STRING, "string" },
    { VALUE,  INT,    "int" },
    { VALUE,  INT,    "i4" },
    { VALUE,  BOOL,   "boolean" },
    { VALUE,  DOUBLE, "double" },
    { VALUE,  BINARY, "base64" },
    { VALUE,  TIME,   "dateTime.iso8601" },
    { VALUE,  STRUCT, "struct" },
    { VALUE,  ARRAY,  "array" },
    { VALUE,  NIL,    "nil" },
    { 0, 0, 0 }
  };
  state_.set_transitions(trans);
}

void
ValueBuilder::do_visit_element(const std::string& tagname)
{
  switch (state_.change(tagname)) {
  case STRUCT:
    retval.reset(sub_build<Value_type*, StructBuilder>(true));
    break;

  case ARRAY:
    retval.reset(sub_build<Value_type*, ArrayBuilder>(true));
    break;

  case NIL:
    retval.reset(new Nil());
    break;

  default:
    // wait for text within <i4>...</i4>, etc...
    break;
  }

  if (retval.get())
    want_exit();
}

void
ValueBuilder::do_visit_element_end(const std::string&)
{
  if (retval.get())
    return;

  switch (state_.get_state()) {
  case VALUE:
  case STRING:
    retval.reset(new String(""));
    break;

  case BINARY:
    retval.reset(Binary_data::from_data(""));
    break;

  default:
    throw XML_RPC_violation(parser_.context());
  }
}

void
ValueBuilder::do_visit_text(const std::string& text)
{
  using boost::lexical_cast;

  switch (state_.get_state()) {
  case VALUE:
    want_exit();
  case STRING:
    retval.reset(new String(text));
    break;

  case INT:
    retval.reset(new Int(lexical_cast<int>(text)));
    break;

  case BOOL:
    retval.reset(new Bool(lexical_cast<int>(text) != 0));
    break;

  case DOUBLE:
    retval.reset(new Double(lexical_cast<double>(text)));
    break;

  case BINARY:
    retval.reset(Binary_data::from_base64(text));
    break;

  case TIME:
    retval.reset(new Date_time(text));
    break;

  default:
    throw XML_RPC_violation(parser_.context());
  }
}

} // namespace iqxmlrpc

// vim:sw=2:ts=2:et:
