////////////////////////////////////////////////////////////////////////////////
// $Id$
// Authors: juergen.mellinger@uni-tuebingen.de
// Description: State-related object types for the script interpreter.
//
// $BEGIN_BCI2000_LICENSE$
//
// This file is part of BCI2000, a platform for real-time bio-signal research.
// [ Copyright (C) 2000-2012: BCI2000 team and many external contributors ]
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
////////////////////////////////////////////////////////////////////////////////
#ifndef STATE_TYPES_H
#define STATE_TYPES_H

#include "ObjectType.h"

class State;

namespace Interpreter {

class StateType : public ObjectType
{
 protected:
  virtual const char* Name() const { return "State"; }
  virtual const MethodEntry* MethodTable() const { return sMethodTable; }

 public:
  static bool Set( ScriptInterpreter& );
  static bool Get( ScriptInterpreter& );
  static bool Insert( ScriptInterpreter& );
  static bool List( ScriptInterpreter& );

 private:
  static State& GetState( ScriptInterpreter& );

  static const MethodEntry sMethodTable[];
  static StateType sInstance;
};

class StatesType : public ObjectType
{
 protected:
  virtual const char* Name() const { return "States"; }
  virtual const MethodEntry* MethodTable() const { return sMethodTable; }

 public:
  static bool Insert( ScriptInterpreter& );
  static bool List( ScriptInterpreter& );
  static bool Clear( ScriptInterpreter& );

 private:
  static const MethodEntry sMethodTable[];
  static StatesType sInstance;
};

} // namespace

#endif // STATE_TYPES_H
