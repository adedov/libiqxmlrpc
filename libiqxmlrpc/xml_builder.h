//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_xml_builder_h_
#define _iqxmlrpc_xml_builder_h_

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
  add_textdata(const std::string&);

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
