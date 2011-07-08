//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_dispatcher_manager_
#define _iqxmlrpc_dispatcher_manager_

#include "method.h"

#include <boost/noncopyable.hpp>

namespace iqxmlrpc {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4275)
#endif

//! Class that encapsulate control of multiple method dispatchers.
/*! It also provides default method dispatcher that support usual
 *  register_method operation and optionally system one, which holds
 *  server's built-in methods
 */
class LIBIQXMLRPC_API Method_dispatcher_manager: boost::noncopyable {
  class Impl;
  Impl* impl_;

public:
  Method_dispatcher_manager();
  ~Method_dispatcher_manager();

  //! Registers method factory in default method dispatcher.
  void register_method(const std::string& name, Method_factory_base*);

  //! Add dispatcher at the end of dispatchers list. Grabs ownership.
  void push_back(Method_dispatcher_base*);

  //! Create method.
  /*! It calls each registered method dispatcher in the same order as
      they registered. First non-zero result will be returned.
   */
  Method* create_method(const Method::Data&);

  //! Return list of methods provided by all registered dispatchers.
  void get_methods_list(Array&) const;

  //! Turns on introspection.
  void enable_introspection();
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqxmlrpc

#endif
