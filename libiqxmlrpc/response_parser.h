#ifndef _iqxmlrpc_response_parser_h_
#define _iqxmlrpc_response_parser_h_

#include <boost/optional.hpp>
#include "value.h"
#include "parser2.h"
#include "response.h"

namespace iqxmlrpc {

class ResponseBuilder: public BuilderBase {
public:
  ResponseBuilder(Parser&);

  Response
  get();

private:
  virtual void
  do_visit_element(const std::string&);

  void
  parse_ok();

  void
  parse_fault();

  StateMachine state_;
  boost::optional<Value> ok_;
  int fault_code_;
  boost::optional<std::string> fault_str_;
};

} // namespace iqxmlrpc

#endif
