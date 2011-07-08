//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "method.h"

#include "except.h"
#include "server.h" // Server_feedback
#include "util.h"

#include <algorithm>

using namespace iqxmlrpc;

void Server_feedback::set_exit_flag()
{
  if (!server_) // should never be
    throw Exception("Server_feedback: null pointer access.");

  server_->set_exit_flag();
}

void Server_feedback::log_message( const std::string& msg )
{
  if (!server_) // should never be
    throw Exception("Server_feedback: null pointer access.");

  server_->log_err_msg(msg);
}

// ----------------------------------------------------------------------------
void Method::process_execution(Interceptor* ic, const Param_list& params, Value& result)
{
  if (ic) {
    ic->process(this, params, result);
  } else {
    execute(params, result);
  }
}
