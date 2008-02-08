//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2007 Anton Dedov
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

#ifndef _iqxmlrpc_method_h_
#define _iqxmlrpc_method_h_

#include <string>
#include <map>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include "api_export.h"
#include "value.h"
#include "except.h"
#include "inet_addr.h"

namespace iqxmlrpc
{
class Server;
class Interceptor;
class Method;
class Method_dispatcher_base;

//! Method's parameters type
typedef std::vector<Value> Param_list;

//! Type of pointer to function that can be used as server method.
typedef void (*Method_function)(Method*, const Param_list&, Value&);

//! This clas provides restricted interface of class Server for Method's needs.
class LIBIQXMLRPC_API Server_feedback {
  Server* server_;

public:
  // Do not use objects constructed with default ctor!
  Server_feedback():
    server_(0) {}

  Server_feedback(Server* s):
    server_(s) {}

  void set_exit_flag();
  void log_message( const std::string& );
};

//! Abstract base for server method.
//! Inherit it to create actual server method.
class LIBIQXMLRPC_API Method {
public:
  struct Data {
    std::string      method_name;
    iqnet::Inet_addr peer_addr;
    Server_feedback  server_face;
  };

private:
  friend class Method_dispatcher_base;
  Data data_;
  std::string authname_;

public:
  virtual ~Method() {}

  //! Calls customized execute() and optionally wraps it with interceptors.
  //! Is is called by a server object.
  void process_execution(Interceptor*, const Param_list& params, Value& response);

  const std::string&      name()      const { return data_.method_name; }
  const iqnet::Inet_addr& peer_addr() const { return data_.peer_addr; }
  Server_feedback&        server()          { return data_.server_face; }

  bool                    authenticated() const { return !authname_.empty(); }
  const std::string&      authname()      const { return authname_; }
  void                    authname(const std::string& n) { authname_ = n; }

private:
  //! Replace it with your actual code.
  virtual void execute( const Param_list& params, Value& response ) = 0;
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#endif

//! Interceptor's base class
/*! One can use interceptors in order to wrap actual XML-RPC calls
 *  on server side with code that supports particular aspect.
 *  E.g. logging or catch/re-throw internal exceptions, etc.
 *
 *  Interceptors are stacked inside of server and are being called
 *  in order of LIFO.
 *
 *  Also note that interceptors are shared between method that executed
 *  in the same time. So the synchronization of internal state of
 *  user-defined interceptor is up to it's creator.
 */
class LIBIQXMLRPC_API Interceptor: boost::noncopyable {
public:
  virtual ~Interceptor() {}

  void nest(Interceptor* ic)
  {
    nested.reset(ic);
  }

  //! User defined interceptor's code goes here.
  /*! This method is called by library as soon it resolved target XML-RPC
   * method on a server side. This method must call actual XML-RPC method
   * (along with other stacked interceptors) via calling yield() member
   */
  virtual void process(Method*, const Param_list&, Value&) = 0;

protected:
  //! Yield the control to specific XML-RPC method.
  /*! This function <b>must</b> be called by process() method function
   *  unless it meaningfuly decided not to call actual server method.
   */
  void yield(Method* m, const Param_list& params, Value& result)
  {
    m->process_execution(nested.get(), params, result);
  }

private:
  boost::scoped_ptr<Interceptor> nested;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//! Adapter that allows make server method from plain function.
//! \see Method_function
class LIBIQXMLRPC_API Method_function_adapter: public Method {
public:
  Method_function_adapter(Method_function f):
    function(f) {}

private:
  void execute(const Param_list& params, Value& result)
  {
    function(this, params, result);
  }

  Method_function function;
};

//! Abstract factory for Method.
/*! Method_dispatcher uses it to create Method object on demand.
    Inherit it to create your specific factory.
    \see Method_factory
*/
class LIBIQXMLRPC_API Method_factory_base {
public:
  virtual ~Method_factory_base() {}

  virtual Method* create() = 0;
};


//! Template for simple Method factory.
template <class T>
class Method_factory: public Method_factory_base {
public:
  T* create() { return new T(); }
};


//! Specialization for funciton adapters.
template <>
class Method_factory<Method_function_adapter>: public Method_factory_base {
public:
  Method_factory(Method_function fn):
    function(fn) {}

  Method* create() { return new Method_function_adapter(function); }

private:
  Method_function function;
};


//! Method dispatcher base class.
class LIBIQXMLRPC_API Method_dispatcher_base {
public:
  virtual ~Method_dispatcher_base() {}

  Method* create_method(const Method::Data& data)
  {
    Method *method = do_create_method(data.method_name);
    if (method)
      method->data_ = data;

    return method;
  }

  void get_methods_list(Array& retval) const
  {
    do_get_methods_list(retval);
  }

private:
  virtual Method*
  do_create_method(const std::string&) = 0;

  virtual void
  do_get_methods_list(Array&) const = 0;
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
