//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "builtins.h"
#include "dispatcher_manager.h"

namespace iqxmlrpc {
namespace builtins {

List_methods::List_methods(Method_dispatcher_manager* disp_manager):
  disp_manager_(disp_manager)
{
}

void List_methods::execute(const Param_list& params, Value& resp)
{
  resp = Array();
  disp_manager_->get_methods_list(resp.the_array());
}

} // namespace builtins
} // namespace iqxmlrpc
