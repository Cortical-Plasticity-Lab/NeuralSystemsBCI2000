//////////////////////////////////////////////////////////////////////
// $Id$
// Author: juergen.mellinger@uni-tuebingen.de
// Description: A class that simplifies high-level exception catching.
//   Call its Run() function with a Runnable as argument in order
//   to execute the Runnable in a try block, catching exceptions that
//   occur during execution of the Runnable.
//
//   struct
//   {
//     int arg;
//     int result;
//     void OnRun()
//     { result = SomeFunction( arg ); }
//   } runnable = { arg, -1 };
//   ExceptionCatcher().SetMessage( "aborting" )
//                      Run( runnable );
//
// $BEGIN_BCI2000_LICENSE$
//
// This file is part of BCI2000, a platform for real-time bio-signal research.
// [ Copyright (C) 2000-2011: BCI2000 team and many external contributors ]
//
// BCI2000 is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// BCI2000 is distributed in the hope that it will be useful, but
//                         WITHOUT ANY WARRANTY
// - without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//
// $END_BCI2000_LICENSE$
///////////////////////////////////////////////////////////////////////
#ifndef EXCEPTION_CATCHER_H
#define EXCEPTION_CATCHER_H

#include <string>
#include "Runnable.h"

class ExceptionCatcher
{
 public:
  // Run() returns true when execution finished normally, false when an exception was caught.
  bool Run( Runnable& );
  // The Message property is a string that is appended when reporting an error.
  ExceptionCatcher&  SetMessage( const std::string& inMessage )
                     { mMessage = inMessage; return *this; }
  const std::string& Message() const
                     { return mMessage; }

 private:
  bool Run2( Runnable& );
#if _MSC_VER
  void ReportWin32Exception( int code );
#endif // _MSC_VER

 private:
  std::string mMessage;
};

#endif // EXCEPTION_CATCHER_H