////////////////////////////////////////////////////////////////////////////////
//  $Id$
//  Author:      juergen.mellinger@uni-tuebingen.de
//  Description: A filter that uses arithmetic expressions to compute its
//    output.
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
////////////////////////////////////////////////////////////////////////////////
#ifndef EXPRESSION_FILTER_H
#define EXPRESSION_FILTER_H

#include "GenericFilter.h"
#include "Expression/Expression.h"
#include <vector>

class ExpressionFilter : public GenericFilter
{
 public:
   ExpressionFilter();
   ~ExpressionFilter();

   void Preflight( const SignalProperties&, SignalProperties& ) const;
   void Initialize( const SignalProperties&, const SignalProperties& );
   void StartRun();
   void StopRun();
   void Process( const GenericSignal&, GenericSignal& );

 private:
   typedef std::vector< std::vector<Expression> > ExpressionMatrix;
   typedef std::vector< std::vector< Expression::VariableContainer > > VariablesMatrix;

   void LoadConfig( ExpressionMatrix&, ExpressionMatrix& startRun, ExpressionMatrix& stopRun, VariablesMatrix& ) const;
   static void LoadExpressions( const ParamRef&, ExpressionMatrix& );
   static void EvaluateExpressions( ExpressionMatrix&, VariablesMatrix&, const GenericSignal* = NULL, GenericSignal* = NULL );

   ExpressionMatrix mExpressions,
                    mStartRunExpressions,
                    mStopRunExpressions;
   VariablesMatrix  mVariables;
};
#endif // EXPRESSION_FILTER_H
