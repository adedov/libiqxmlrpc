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
//  $Id: except.cc,v 1.5 2004-10-11 11:02:50 maxim Exp $

#include <libxml++/libxml++.h>
#include <sstream>
#include "except.h"

using namespace iqxmlrpc;


XML_RPC_violation XML_RPC_violation::at_node( const xmlpp::Node* node )
{
  std::stringstream s;
  s << "XML-RPC format violation at line " << node->get_line();
  return XML_RPC_violation( s.str() );
}


XML_RPC_violation XML_RPC_violation::caused( 
  const std::string& s, const xmlpp::Node* node )
{
  std::string errstr(s);
  
  if( node )
  {
    std::stringstream s;
    s << " at line " << node->get_line();
    errstr += s.str();
  }
  
  return XML_RPC_violation( errstr );
}



// ----------------------------------------------------------------------------
Fault::Fault( int c, const std::string& s ):
  Exception( s, c )
{
  if( c >= -32768 && c <= -32000 )
    throw FCI_violation();
}
