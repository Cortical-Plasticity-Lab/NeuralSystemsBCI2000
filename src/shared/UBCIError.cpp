////////////////////////////////////////////////////////////////////////////////
//
// File: UBCIError.cpp
//
// Description: Declarations for stream symbols related to error handling.
//              To report an error, write e.g.
//               bcierr << "My error message" << endl;
//              For an informational message, write
//               bciout << "My info message" << endl;
//
// Author: Juergen Mellinger
//
// Date:   Mar 27, 2003
//
// Changes: Apr 16, 2003: Replaced dummy implementations by objects that
//          actually hold messages.
//          Jul 22, 2003: Added implementations for command line tools.
//
////////////////////////////////////////////////////////////////////////////////
#include "PCHIncludes.h"
#pragma hdrstop

#include "UBCIError.h"
#include <iostream>

#ifndef BCI_TOOL
# include "UEnvironment.h"
# include "MessageHandler.h"
# include "UStatus.h"
#endif

using namespace std;
using namespace BCIError;

// Definitions of the actual global objects.
bci_ostream __bcierr;
bci_ostream __bciout;

std::ostream&
bci_ostream::operator()( const char* inInfoHeader )
{
#if 0
  static string lastInfoHeader;
  if( inInfoHeader != lastInfoHeader )
  {
    *this << inInfoHeader << ": ";
    lastInfoHeader = inInfoHeader;
  }
#else
  *this << inInfoHeader << ": ";
#endif
  return ( *this )();
}

void
bci_ostream::bci_stringbuf::SetFlushHandler( bci_ostream::flush_handler f )
{
  if( on_flush )
    on_flush( str() );
  else
    f( str() );
  on_flush = f;
}

int
bci_ostream::bci_stringbuf::sync()
{
  int r = stringbuf::sync();
  ++num_flushes;
  if( on_flush )
  {
    on_flush( str() );
    str( "" );
  }
  return r;
}

#ifdef BCI_TOOL // implementation for a filter wrapper
void
BCIError::Warning( const std::string& message )
{
}

void
BCIError::ConfigurationError( const std::string& message )
{
  if( message.length() > 1 )
    cerr << "Configuration Error: " << message << endl;
}

void
BCIError::RuntimeError( const std::string& message )
{
  if( message.length() > 1 )
    cerr << "Runtime Error: " << message << endl;
}

void
BCIError::LogicError( const std::string& message )
{
  if( message.length() > 1 )
    cerr << "Logic Error: " << message << endl;
}
#else // implementation for a module
// A preliminary implementation of the error display/error handling mechanism
// on the core module side.
struct StatusMessage
{
 void operator()( const std::string& text, int code )
 {
  static ostream* op = NULL;
  if( op == NULL )
    op = ::Environment::Operator;

  // If the connection to the operator does not work, fall back to a local
  // error display.
  if( op != NULL )
  {
    MessageHandler::PutMessage( *op, STATUS( text.c_str(), code ) );
    op->flush();
  }
  if( ( !op || !*op ) && code >= 400 )
#if !defined( _WIN32 ) || defined( __CONSOLE__ )
    cerr << text << endl;
#else
    ::MessageBox( NULL, text.c_str(), "BCI2000 Error",
                    MB_OK | MB_ICONHAND | MB_SYSTEMMODAL | MB_SETFOREGROUND );
#endif
 }
} StatusMessage;

void
BCIError::Warning( const std::string& message )
{
  if( message.length() > 1 )
    StatusMessage( string( "Warning: " ) + message.substr( 0, message.length() - 1 ), 301 );
}

void
BCIError::ConfigurationError( const std::string& message )
{
  if( message.length() > 1 )
    StatusMessage( message.substr( 0, message.length() - 1 ), 408 );
}

void
BCIError::RuntimeError( const std::string& message )
{
  if( message.length() > 1 )
    StatusMessage( message.substr( 0, message.length() - 1 ), 409 );
}

void
BCIError::LogicError( const std::string& message )
{
  if( message.length() > 1 )
    StatusMessage( message.substr( 0, message.length() - 1 ), 499 );
}
#endif // BCI_TOOL
