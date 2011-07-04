#include <algorithm>
#include <stdexcept>
#include <libxml/xmlreader.h>
#include <libxml/xmlIO.h>
#include "parser2.h"
#include "except.h"

// debug
#include <iostream>

namespace iqxmlrpc {

inline std::string
to_string(xmlChar* s)
{
  if (s) {
    std::string retval(reinterpret_cast<const char*>(s));
    xmlFree(s);
    return retval;
  }

  return std::string();
}

//
// BuilderBase
//

BuilderBase::BuilderBase(Parser& p, bool t):
  parser_(p),
  depth_(0),
  expect_text_(t)
{
}

void
BuilderBase::build(bool flat)
{
  depth_ += flat ? 1 : 0;
  parser_.parse(*this);
}

void
BuilderBase::visit_element(const std::string& tag)
{
  depth_++;
  do_visit_element(tag);
}

bool
BuilderBase::visit_element_end(const std::string& tag)
{
  depth_--;
  do_visit_element_end(tag);
  return !depth_;
}

bool
BuilderBase::visit_text(const std::string& text)
{
  return do_visit_text(text);
}

void
BuilderBase::do_visit_element_end(const std::string&)
{
}

bool
BuilderBase::do_visit_text(const std::string&)
{
  if (expect_text_) {
    // proper handler was not implemented
    throw XML_RPC_violation(parser_.context());
  }

  return false;
}

//
// Parser
//

struct Parser::Impl {
public:
  Impl(const std::string& str):
    element_begin(false),
    element_end(false),
    in_element(false),
    is_text(false)
  {
    buf = xmlParserInputBufferCreateMem(str.data(), str.length(), XML_CHAR_ENCODING_NONE);
    // TODO: check buf is not 0
    reader = xmlNewTextReader(buf, 0);
    // TODO: check reader is not 0
  }

  ~Impl()
  {
    xmlFreeTextReader(reader);
    xmlFreeParserInputBuffer(buf);
  }

  bool
  read()
  {
    int code = xmlTextReaderRead(reader);

    if (code > 0) {
      int type = node_type();
      is_text = type == XML_READER_TYPE_TEXT;
      element_begin = type == XML_READER_TYPE_ELEMENT;
      element_end = type == XML_READER_TYPE_END_ELEMENT;
      in_element = element_begin || element_end;
      return true;
    }

    if (code < 0) {
      xmlErrorPtr err = xmlGetLastError();
      throw Parse_error(err ? err->message : "unknown parsing error");
    }

    return false;
  }

  int
  node_type()
  {
    return xmlTextReaderNodeType(reader);
  }

  int
  depth()
  {
    return xmlTextReaderDepth(reader);
  }

  std::string
  tag_name()
  {
    return to_string(xmlTextReaderName(reader));
  }

  std::string
  read_data()
  {
    return to_string(xmlTextReaderReadString(reader));
  }

  std::string
  get_context() const
  {
    xmlNodePtr n = xmlTextReaderCurrentNode(reader);
    return to_string(xmlGetNodePath(n));
  }

  xmlParserInputBufferPtr buf;
  xmlTextReaderPtr reader;
  bool element_begin;
  bool element_end;
  bool in_element;
  bool is_text;
};

Parser::Parser(const std::string& buf):
  impl_(new Parser::Impl(buf))
{
}

void
Parser::parse(BuilderBase& builder)
{
  while (impl_->read()) {
    if (impl_->in_element) {
      std::string name = impl_->tag_name();

      if (impl_->element_begin) {
        builder.visit_element(name);
      } else {
        bool finish_step = builder.visit_element_end(name);
        if (finish_step)
          break;
      }
    }

    if (impl_->is_text && builder.expects_text()) {
      if (builder.visit_text(get_data()))
        break;
    }
  }
}

std::string
Parser::get_data()
{
  return impl_->read_data();
}

bool
Parser::is_text_node()
{
  return impl_->node_type() == XML_READER_TYPE_TEXT;
}

std::string
Parser::context() const
{
  return impl_->get_context();
}

//
// StateMachine
//

StateMachine::StateMachine(const Parser& p, int start_state):
  parser_(p),
  curr_(start_state)
{
}

void
StateMachine::set_transitions(const StateTransition* t)
{
  trans_ = t;
}

int
StateMachine::change(const std::string& tag)
{
  bool found = false;
  size_t i = 0;
  for (; trans_[i].tag != 0; ++i) {
    if (trans_[i].tag == tag && trans_[i].prev_state == curr_) {
      found = true;
      break;
    }
  }

  if (!found) {
    std::string err = "unexpected tag <" + std::string(tag) + "> at " + parser_.context();
    throw XML_RPC_violation(err);
  }

  curr_ = trans_[i].new_state;
  return curr_;
}

void
StateMachine::set_state(int new_state)
{
  curr_ = new_state;
}

} // namespace iqxmlrpc
// vim:sw=2:ts=2:et:
