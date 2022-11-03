//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_parser2_h_
#define _iqxmlrpc_parser2_h_

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace iqxmlrpc {

class Parser;

class BuilderBase {
public:
  BuilderBase(Parser&, bool expect_text = false);

  void
  visit_element(const std::string& tag);

  void
  visit_element_end(const std::string& tag);

  void
  visit_text(const std::string&);

  bool
  expects_text() const
  {
    return expect_text_;
  }

  int
  depth() const
  {
    return depth_;
  }

  bool
  wants_exit() const
  {
    return want_exit_;
  }

  void
  build(bool flat = false);

protected:
  template <class R, class BUILDER>
  R
  sub_build(bool flat = false)
  {
    BUILDER b(parser_);
    b.build(flat);
    return b.result();
  }

  void
  want_exit()
  {
    want_exit_ = true;
  }

  virtual void
  do_visit_element(const std::string&) = 0;

  virtual void
  do_visit_element_end(const std::string&);

  virtual void
  do_visit_text(const std::string&);

  Parser& parser_;
  int depth_;
  bool expect_text_;
  bool want_exit_;
};

class Parser {
public:
  Parser(const std::string& buf);

  void
  parse(BuilderBase& builder);

  std::string
  get_data();

  std::string
  context() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

class StateMachine {
public:
  struct StateTransition {
    int prev_state;
    int new_state;
    const char* tag;
  };

  StateMachine(const Parser&, int start_state);

  void
  set_transitions(const StateTransition*);

  int
  get_state() const { return curr_; }

  int
  change(const std::string& tag);

  void
  set_state(int new_state);

private:
  typedef const StateTransition* TransitionMap;

  const Parser& parser_;
  int curr_;
  TransitionMap trans_;
};

} // namespace iqxmlrpc

#endif
// vim:sw=2:ts=2:et:
