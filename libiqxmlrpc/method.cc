//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
//
//  $Id: method.cc,v 1.13 2006-09-07 09:35:42 adedov Exp $

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
