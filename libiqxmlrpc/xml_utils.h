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
//  $Id$

#ifndef _iqxmlrpc_xml_utils_h_
#define _iqxmlrpc_xml_utils_h_

#include "api_export.h"

#include <boost/utility.hpp>
#include <string>
#include <libxml/xmlwriter.h>

namespace iqxmlrpc {

class XmlBuilder: boost::noncopyable {
public:
  class Node {
  public:
    Node(XmlBuilder&, const char* name);
    ~Node();

    void
    set_textdata(const std::string&);

  private:
    XmlBuilder& ctx;
  };

  XmlBuilder();
  ~XmlBuilder();

  void
  stop();

  std::string
  content() const;

private:
  xmlBufferPtr buf;
  xmlTextWriterPtr writer;
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
