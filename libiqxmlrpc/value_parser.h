//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_value_parser_h_
#define _iqxmlrpc_value_parser_h_

#include "parser2.h"
#include "value.h"

namespace iqxmlrpc {

class ValueBuilderBase: public BuilderBase {
public:
  ValueBuilderBase(Parser& parser, bool expect_text = false);

  Value_type*
  result()
  {
    return retval.release();
  }

protected:
  std::auto_ptr<Value_type> retval;
};

class ValueBuilder: public ValueBuilderBase {
public:
  ValueBuilder(Parser& parser);

private:
  virtual void
  do_visit_element(const std::string&);

  virtual bool
  do_visit_text(const std::string&);

  StateMachine state_;
};

} // namespace iqxmlrpc

#endif
// vim:sw=2:ts=2:et:
