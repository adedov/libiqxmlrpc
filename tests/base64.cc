#include <iostream>
#include <fstream>
#include <stdexcept>
#include <memory>
#include "libiqxmlrpc/libiqxmlrpc.h"

using namespace iqxmlrpc;

void do_encode( const std::string& path )
{
  std::ifstream f( path.c_str(), std::ios_base::binary );
  std::string s;

  if( !f )
    throw std::runtime_error( "Couldnt open file " + path );

  for( unsigned char c = f.get(); f && !f.eof(); c = f.get() )
    s += c;
  
  std::auto_ptr<Binary_data> bin( Binary_data::from_data(s) );
  std::cout << bin->get_base64();
}


void do_decode( const std::string& path )
{
  std::ifstream f( path.c_str(), std::ios_base::binary );
  std::string s;

  if( !f )
    throw std::runtime_error( "Couldnt open file " + path );

  for( unsigned char c = f.get(); f && !f.eof(); c = f.get() )
    s += c;
  
  std::auto_ptr<Binary_data> bin( Binary_data::from_base64(s) );
  std::cout << bin->get_data();
}


int main( int argc, char** argv )
{
  std::string path = argv[0];

  try
  {
    if( argc != 2 )
      throw std::runtime_error( "You must specify a file." );
    
    if( path.find( "encode" ) != std::string::npos )
      do_encode( argv[1] );
    else if( path.find( "decode" ) != std::string::npos )
      do_decode( argv[1] );
    else
      throw std::runtime_error( "Unknown test." );
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
