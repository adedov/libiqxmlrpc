//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <string>
#include "value_type_visitor.h"

namespace iqxmlrpc {

class XmlBuilder;

//! Value_type visitor that converts values into XML-RPC representation.
class Value_type_to_xml: public Value_type_visitor {
public:
  Value_type_to_xml(XmlBuilder& builder, bool server_mode = false):
    builder_(builder),
    server_mode_(server_mode) {}

private:
  virtual void do_visit_value(const Value_type&);
  virtual void do_visit_nil();
  virtual void do_visit_int(int);
  virtual void do_visit_double(double);
  virtual void do_visit_bool(bool);
  virtual void do_visit_string(const std::string&);
  virtual void do_visit_struct(const Struct&);
  virtual void do_visit_array(const Array&);
  virtual void do_visit_base64(const Binary_data&);
  virtual void do_visit_datetime(const Date_time&);

  void add_textnode(const char* name, const std::string& data);

  XmlBuilder& builder_;
  bool server_mode_;
};

} // namespace iqxmlrpc
