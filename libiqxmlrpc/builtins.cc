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
//  $Id: builtins.cc,v 1.1 2004-09-19 09:39:05 adedov Exp $

#include "builtins.h"

using namespace iqxmlrpc;


Introspector::Meth_info_map Introspector::ibase;

Introspector::const_iterator Introspector::begin()
{
  return Introspector::ibase.begin();
}


Introspector::const_iterator Introspector::end()
{
  return Introspector::ibase.end();
}


Introspector::const_iterator Introspector::find( const std::string& n )
{
  return Introspector::ibase.find( n );
}


void Introspector::register_help_obj( const std::string& n, Method::Help* obj )
{
  iterator i = Introspector::ibase.find( n );
  if( i != end() )
  {
    delete i->second;
    ibase.erase(i);
  }
  
  Introspector::ibase[n] = obj;
}


// ----------------------------------------------------------------------------
Value List_methods_m::Help::signature() const
{
  Array a;
  return a;
}


std::string List_methods_m::Help::help() const 
{
  return "An introspection method, see "
         "http://xmlrpc.usefulinc.com/doc/reserved.html";
}


void List_methods_m::execute( const Param_list& params, Value& response )
{
  Array arr;
  for( const_iterator i = begin(); i != end(); ++i )
    arr.push_back( i->first );
  
  response = arr;
}


// ----------------------------------------------------------------------------
Value Method_signature_m::Help::signature() const
{
  Array a;
  a.push_back( "array, string" );
  return a;
}


std::string Method_signature_m::Help::help() const
{
  return "An introspection method, see "
         "http://xmlrpc.usefulinc.com/doc/sysmethodsig.html";
}


void Method_signature_m::execute( const Param_list& params, Value& response )
{
  if( params.size() != 1 )
    throw Invalid_meth_params();
  
  std::string mname = params.front();
  const_iterator i = find( mname );
  if( i == end() )
    throw Fault( -32099, "Unknown method: " + mname );
  
  response = i->second->signature();
}


// ----------------------------------------------------------------------------
Value Method_help_m::Help::signature() const
{
  Array a;
  a.push_back( "string, string" );
  return a;
}


std::string Method_help_m::Help::help() const 
{
  return "An introspection method, see "
         "http://xmlrpc.usefulinc.com/doc/reserved.html";
}


void Method_help_m::execute( const Param_list& params, Value& response )
{
  if( params.size() != 1 )
    throw Invalid_meth_params();
  
  std::string mname = params.front();
  const_iterator i = find( mname );
  if( i == end() )
    throw Fault( -32099, "Unknown method: " + mname );
  
  response = i->second->help();
}
