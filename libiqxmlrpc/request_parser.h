#ifndef _iqxmlrpc_request_parser_h_
#define _iqxmlrpc_request_parser_h_

#include "parser2.h"
#include "request.h"

namespace iqxmlrpc {

class RequestBuilder: public BuilderBase {
public:
  RequestBuilder(Parser&);

  Request
  get();

private:
  virtual void
  do_visit_element(const std::string&);

  StateMachine state_;
  std::string method_name_;
  Param_list params_;
};

} // namespace iqxmlrpc

#endif
