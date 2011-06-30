#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "except.h"
#include "value_parser.h"
#include <iostream>

namespace iqxmlrpc {

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
    retval = proxy_ = new Struct();
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
      break;

    case MEMBER:
      break;

    default:
      throw XML_RPC_violation::caused(parser_.context());
    }
  }

  virtual void
  do_visit_element_end(const std::string& tagname)
  {
    if (tagname == "member") {
      if (state_.get_state() != VALUE_READ) {
        throw XML_RPC_violation::caused(parser_.context());
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
    retval = proxy_ = new Array();
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
      Value_ptr v(new Value(sub_build<Value_type*, ValueBuilder>()));
      proxy_->push_back(v);
    }
  }

  StateMachine state_;
  Array* proxy_;
};

ValueBuilderBase::ValueBuilderBase(Parser& parser, bool expect_text):
  BuilderBase(parser, expect_text),
  retval(0)
{
}

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
    { VALUE,  TIME,   "dateXXX" },
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
    retval = sub_build<Value_type*, StructBuilder>(true);
    break;

  case ARRAY:
    retval = sub_build<Value_type*, ArrayBuilder>(true);
    break;

  case NIL:
    retval = new Nil();
    break;

  default:
    // wait for text
    break;
  }
}

bool
ValueBuilder::do_visit_text(const std::string& text)
{
  using boost::lexical_cast;
  bool finish = false;

  switch (state_.get_state()) {
  case VALUE:
    finish = true;
  case STRING:
    retval = new String(text);
    break;

  case INT:
    retval = new Int(lexical_cast<int>(text));
    break;

  case BOOL:
    retval = new Bool(bool(lexical_cast<int>(text)));
    break;

  case DOUBLE:
    retval = new Double(lexical_cast<double>(text));
    break;

  case BINARY:
  case TIME:
  default:
    throw std::runtime_error("not implemented yet");
  }

  return finish;
}

} // namespace iqxmlrpc

// vim:sw=2:ts=2:et:
