#ifndef _iqxmlrpc_method_h_
#define _iqxmlrpc_method_h_

#include <string>
#include <map>
#include <libiqxmlrpc/value.h>
#include <libiqxmlrpc/except.h>


namespace iqxmlrpc
{
  //! Vector of Value objects.
  typedef std::vector<Value> Param_list;

  class Method;
  class Method_factory_base;
  template <class T> class Method_factory;
  class Method_dispatcher;
};


//! Abstract base for server method. 
//! Inherit it to create actual server method.
class iqxmlrpc::Method {
  friend class Method_dispatcher;
  std::string name_;
  
public:
  virtual ~Method() {}

  const std::string& name() const { return name_; }
  
  //! Replace it with your actual code.
  virtual void execute( const Param_list& params, Value& response ) = 0;
};


//! Abstract factory for Method. 
/*! Method_dispatcher uses it to create Method object on demand. 
    Inherit it to create your specific factory.
    \see Method_factory
*/
class iqxmlrpc::Method_factory_base {
public:
  virtual ~Method_factory_base() {}
    
  virtual Method* create() = 0;
};
  

//! Template for simple Method factory.
/*!
Usage:
  \code
  Method_factory_base* myfactory = new Method_factory<MyMethod>;
  \endcode
*/
template <class T>
class iqxmlrpc::Method_factory: public Method_factory_base {
public:
  T* create() { return new T(); }
};


//! Method dispatcher.
/*! This class responsible for methods dispatching by their names.
    User must register his own methods in Method_dispatcher.
    \code
    // Usage example:
    class MyMethod;
    Method_dispatcher disp;
    disp.register_method( "my_method", new Method_factory<MyMethod> );
    \endcode
*/
class iqxmlrpc::Method_dispatcher {
public:
  //! Exception is being thrown when user tries to create 
  //! Method object for unregistered name.
  class Unknown_method: public Exception {
    enum { code = Fault_code::unknown_method };
  public:
    Unknown_method( const std::string& name ): 
      Exception( "Unknown method '" + name + "'.", code ) {}
  };
  
private:
  typedef std::map<std::string, Method_factory_base*> Factory_map;
  Factory_map fs;
  
public:
  virtual ~Method_dispatcher();

  //! Register Method with it fabric. 
  /*! Method_dispatcher responsible for fabric deletion. */
  void register_method( const std::string& name, Method_factory_base* );

  //! Create method object according to specified name. 
  //! Returns 0 if no Method registered for name.
  Method* create_method( const std::string& name );
};


/*! 
\page define_methods Defining server behaviour

An actual server behaviour is defined by number of server methods. An 
application developer is responsible for creation of such methods' set.
Each server method is a class that must be a child of iqxmlrpc::Method class.
One must at least define abstract method iqxmlrpc::Method::execute in child 
class.

Here is simple method example:
\code
  class Chk_password: public iqxmlrpc::Method {
    enum { my_fault = Fault_code::last+1 };

  public:
    void execute( const iqxmlrpc::Param_list& pl, iqxmlrpc::Value& retval )
    {
      if( !chk_password( pl[0] )
        throw Fault( my_fault, "Password is incorrect." );
    }

  private:
    bool chk_password( const std::string& );
  };
\endcode

\note When user throws libiqxml::Fault exception library forms fault response
to a remote client as proposed by XML-RPC standard. So feel free throw 
iqxmlrpc::Fault object or its childs when you want to inform remote user about
method's fault. \see \ref exceptions

\note (????) Please note that depending on a type of transport all methods can be
executed in a single thread (iqxmlrpc::Http_server) or in a multithreaded
environment (iqxmlrpc::Https_server, not implemented yet).

\section method_disp Method dispatcher
Library dispatches and executes methods. The iqxmlrpc::Method_dispatcher 
class is responsible for holding a set of methods and creating an instance of
particular method on demand. To enable method to be dispatched by a 
Method_dispatcher one should register it with dispatcher object:
\code
  iqxmlrpc::Method_dispatcher disp;
  disp.register_method( "check_password", new Method_factory<Chk_password> );
\endcode

As you can see \b Method_dispatcher::register_method function takes \e name
of method as first argument and \e method \e factory as second argument.

\par Method name
Method \e name is an unique string by which remote client can invoke specific 
method through a XML-RPC call.

\par Method factory
\e Method \e factory is an object wich constructs specific method object. Since
Method_dispatcher know nothing about concrete nethod's type it requires some
fabric to construct concrect Method object at runtime. Library has template 
class iqxmlrpc::Method_factory. One can use it in almost all cases, as you could 
see in previous example. If you need more flexibility then inherit 
iqxmlrpc::Method_factory_base.

\see \ref network_server
*/

#endif
