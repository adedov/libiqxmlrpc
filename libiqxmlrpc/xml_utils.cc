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

#include <stdexcept>
#include "xml_utils.h"

using namespace iqxmlrpc;

//
// XmlBuilder::Node
//

XmlBuilder::Node::Node(XmlBuilder& w, const char* name):
  ctx(w)
{
  xmlTextWriterStartElement(ctx.writer, reinterpret_cast<const xmlChar*>(name)); // TODO check
}

XmlBuilder::Node::~Node()
{
  if (!std::uncaught_exception())
    xmlTextWriterEndElement(ctx.writer);
}

void
XmlBuilder::Node::set_textdata(const std::string& data)
{
  xmlTextWriterWriteString(ctx.writer, reinterpret_cast<const xmlChar*>(data.c_str())); // TODO check
}

//
// XmlBuilder
//

XmlBuilder::XmlBuilder()
{
  buf = xmlBufferCreate(); // TODO check NULL
  writer = xmlNewTextWriterMemory(buf, 0); // TODO check NULL
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL); // TODO
}

XmlBuilder::~XmlBuilder()
{
  xmlFreeTextWriter(writer);
  xmlBufferFree(buf);
}

void
XmlBuilder::stop()
{
  xmlTextWriterEndDocument(writer);
}

std::string
XmlBuilder::content() const
{
  return std::string(reinterpret_cast<const char*>(buf->content), buf->use);
}

// vim:ts=2:sw=2:et
