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
//  $Id: builtins.h,v 1.1 2004-09-19 09:39:05 adedov Exp $

#ifndef _iqxmlrpc_buildins_h_
#define _iqxmlrpc_buildins_h_

#include "method.h"

namespace iqxmlrpc 
{
  class Introspector;
  class List_methods_m;
  class Method_signature_m;
  class Method_help_m;
};


class iqxmlrpc::Introspector {
protected:
  typedef std::map<std::string, Method::Help*> Meth_info_map;
  typedef Meth_info_map::iterator iterator;
  typedef Meth_info_map::const_iterator const_iterator;

private:
  static Meth_info_map ibase;

protected:
  static const_iterator begin();
  static const_iterator end();
  static const_iterator find( const std::string& );

public:
  virtual ~Introspector() {}
    
  static void register_help_obj( const std::string&, Method::Help* );
};


class iqxmlrpc::List_methods_m: 
  public iqxmlrpc::Method, 
  public iqxmlrpc::Introspector 
{
public:
  class Help: public Method::Help {
    iqxmlrpc::Value signature() const;
    std::string help() const;
  };

  void execute( const Param_list& params, Value& response );
};


class iqxmlrpc::Method_signature_m: 
  public iqxmlrpc::Method, 
  public iqxmlrpc::Introspector 
{
public:
  class Help: public Method::Help {
    iqxmlrpc::Value signature() const;
    std::string help() const;
  };

  void execute( const Param_list& params, Value& response );
};


class iqxmlrpc::Method_help_m: 
  public iqxmlrpc::Method, 
  public iqxmlrpc::Introspector 
{
public:
  class Help: public Method::Help {
    iqxmlrpc::Value signature() const;
    std::string help() const;
  };

  void execute( const Param_list& params, Value& response );
};


#endif
