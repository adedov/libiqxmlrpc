//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
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
//  $Id: method.h,v 1.13 2004-10-22 04:13:27 adedov Exp $

#ifndef _iqxmlrpc_method_h_
#define _iqxmlrpc_method_h_

#include <string>
#include <map>
#include "value.h"
#include "except.h"
#include "inet_addr.h"

namespace iqxmlrpc
{
  //! Vector of Value objects.
  typedef std::vector<Value> Param_list;

  class Method;  
  class Method_factory_base;
  template <class T> class Method_factory;
  class Method_dispatcher;
};


//! Abstract base for server method. 
//! Inherit it to create actual server method.
class iqxmlrpc::Method {
public:
  //! Introspection interface class. 
  /*! Create appropriate nested class for each of your method classes
      if you want to support an introspection in your appliaction.
  */
  class Help {
  public:
    virtual ~Help() {}
    //! Returns methods signature
    virtual iqxmlrpc::Value signature() const { return iqxmlrpc::Nil(); }
    //! Returns methods help string
    virtual std::string help() const { return ""; }
  };
  
private:
  friend class Method_dispatcher;
  std::string name_;
  iqnet::Inet_addr peer_addr_;
  
public:
  virtual ~Method() {}

  const std::string&      name()      const { return name_; }
  const iqnet::Inet_addr& peer_addr() const { return peer_addr_; }
  
  //! Replace it with your actual code.
  virtual void execute( const Param_list& params, Value& response ) = 0;
};


//! Abstract factory for Method. 
/*! Method_dispatcher uses it to create Method object on demand. 
    Inherit it to create your specific factory.
    \see Method_factory
*/
class iqxmlrpc::Method_factory_base {
public:
  virtual ~Method_factory_base() {}
    
  virtual Method* create() = 0;
};
  

//! Template for simple Method factory.
/*!
Usage:
  \code
  Method_factory_base* myfactory = new Method_factory<MyMethod>;
  \endcode
*/
template <class T>
class iqxmlrpc::Method_factory: public Method_factory_base {
public:
  T* create() { return new T(); }
};


//! Method dispatcher.
/*! This class responsible for methods dispatching by their names.
    User must register his own methods in Method_dispatcher.
    \code
    // Usage example:
    class MyMethod;
    Method_dispatcher disp;
    disp.register_method( "my_method", new Method_factory<MyMethod> );
    \endcode
*/
class iqxmlrpc::Method_dispatcher {
  typedef std::map<std::string, Method_factory_base*> Factory_map;
  Factory_map fs;
  
public:
  virtual ~Method_dispatcher();

  //! Register Method with it fabric. 
  /*! Method_dispatcher responsible for fabric deletion. */
  void register_method( const std::string& name, Method_factory_base* );

  //! Create method object according to specified name. 
  //! Returns 0 if no Method registered for name.
  Method* create_method( const std::string& name, const iqnet::Inet_addr& peer );
};


#endif
