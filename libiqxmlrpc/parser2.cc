//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <algorithm>
#include <stdexcept>
#include <libxml/xmlreader.h>
#include <libxml/xmlIO.h>
#include "parser2.h"
#include "except.h"
#include <iostream>

namespace iqxmlrpc {

namespace {

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

} // nameless namespace

struct LibxmlInitializer {
  LibxmlInitializer()
  {
    // http://www.xmlsoft.org/threads.html
    xmlInitParser();
  }
};

LibxmlInitializer libxml_init;

//
// BuilderBase
//

BuilderBase::BuilderBase(Parser& p, bool t):
  parser_(p),
  depth_(0),
  expect_text_(t),
  want_exit_(false)
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

void
BuilderBase::visit_element_end(const std::string& tag)
{
  depth_--;
  do_visit_element_end(tag);

  if (!depth_)
    want_exit();
}

void
BuilderBase::visit_text(const std::string& text)
{
  do_visit_text(text);
}

void
BuilderBase::do_visit_element_end(const std::string&)
{
}

void
BuilderBase::do_visit_text(const std::string&)
{
  if (expect_text_) {
    // proper handler was not implemented
    throw XML_RPC_violation(parser_.context());
  }
}

//
// Parser
//

class Parser::Impl {
public:
  Impl(const std::string& str):
    buf(str),
    pushed_back(false)
  {
    const char* buf2 = str.data();
    int sz = static_cast<int>(str.size());
    reader = xmlReaderForMemory(buf2, sz, 0, 0, XML_PARSE_NONET | XML_PARSE_HUGE);
    xmlTextReaderSetParserProp(reader, XML_PARSER_SUBST_ENTITIES, 0); // No XXE
  }

  ~Impl()
  {
    xmlFreeTextReader(reader);
  }

  struct ParseStep {
    bool done;
    bool element_begin;
    bool element_end;
    bool is_empty;
    bool is_text;

    ParseStep():
      done(false),
      is_empty(false)
    {
    }

    ParseStep(int type, xmlTextReaderPtr reader):
      done(false),
      element_begin(type == XML_READER_TYPE_ELEMENT),
      element_end(type == XML_READER_TYPE_END_ELEMENT),
      is_empty(element_begin && xmlTextReaderIsEmptyElement(reader)),
      is_text(type == XML_READER_TYPE_TEXT)
    {
    }
  };

  ParseStep
  read()
  {
    if (pushed_back) {
      pushed_back = false;
      return curr;
    }

    if (curr.is_empty) {
      curr.element_begin = false;
      curr.element_end = true;
      curr.is_empty = false;
      return curr;
    }

    int code = xmlTextReaderRead(reader);
    curr.done = true;

    if (code < 0) {
      xmlErrorPtr err = xmlGetLastError();
      throw Parse_error(err ? err->message : "unknown parsing error");
    }

    if (code > 0) {
      curr = ParseStep(node_type(), reader);
    }

    return curr;
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
    if (!curr.is_text && !curr.element_end)
    {
      read();
      if (!curr.is_text && !curr.element_end) {
        std::string err = "text is expected at " + get_context();
        throw XML_RPC_violation(err);
      }
    }
    return to_string(xmlTextReaderValue(reader));
  }

  std::string
  get_context() const
  {
    xmlNodePtr n = xmlTextReaderCurrentNode(reader);
    return to_string(xmlGetNodePath(n));
  }

  const std::string buf;
  xmlTextReaderPtr reader;
  ParseStep curr;
  bool pushed_back;
};

Parser::Parser(const std::string& buf):
  impl_(new Parser::Impl(buf))
{
}

void
Parser::parse(BuilderBase& builder)
{
  for (Impl::ParseStep p = impl_->read(); !p.done; p = impl_->read()) {
    if (p.element_begin) {
      builder.visit_element(impl_->tag_name());

    } else if (p.element_end) {
      if (!builder.depth()) {
        impl_->pushed_back = true;
        break;
      }

      builder.visit_element_end(impl_->tag_name());

    } else if (p.is_text && builder.expects_text()) {
      builder.visit_text(get_data());
    }

    if (builder.wants_exit())
      break;

  } // for
}

std::string
Parser::get_data()
{
  return impl_->read_data();
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
