///////////////////////////////////////////////////////////////////////////////
// $Id$
// File:   convert_bciprm.cpp
// Author: juergen.mellinger@uni-tuebingen.de
// Date:   Dec 15, 2006
// Description: A Matlab (mex) subroutine that converts BCI2000 parameters
//  from Matlab struct into string representation and back.
//
//   parameter_lines = convert_bciprm( parameter_struct );
//
//  converts a BCI2000 parameter struct (as created by load_bcidat)
//  into a cell array of strings containing valid BCI2000 parameter
//  definition strings.
//
//  When the input is a cell array rather than a Matlab struct, convert_bciprm
//  will interpret the input as a list of BCI2000 parameter definition strings:
//
//   parameter_struct = convert_bciprm( parameter_lines );
//
///////////////////////////////////////////////////////////////////////////////
#pragma hdrstop

#include "mex.h"
#include "mexutils.h"
#include "UParameter.h"
#include <sstream>

using namespace std;

mxArray* StringsToStruct( const mxArray* );
mxArray* StructToStrings( const mxArray* );

void
mexFunction( int nargout, mxArray* varargout[],
             int nargin,  const mxArray* varargin[] )
{
  if( nargin < 1 )
    mexErrMsgTxt( "No input given." );

  switch( mxGetClassID( varargin[ 0 ] ) )
  {
    case mxCELL_CLASS:
      varargout[ 0 ] = StringsToStruct( varargin[ 0 ] );
      break;

    case mxSTRUCT_CLASS:
      varargout[ 0 ] = StructToStrings( varargin[ 0 ] );
      break;

    default:
      mexErrMsgTxt( "Expected a cell array or a struct as input." );
  }
}

mxArray*
StringsToStruct( const mxArray* inStrings )
{
  PARAMLIST params;
  int numStrings = mxGetNumberOfElements( inStrings );
  for( int i = 0; i < numStrings; ++i )
  {
    const mxArray* cell = mxGetCell( inStrings, i );
    if( mxGetClassID( cell ) == mxCHAR_CLASS )
    {
      char* line = mxArrayToString( cell );
      try
      {
        params.Add( PARAM( line ) );
      }
      catch( const char* s )
      {
        ostringstream oss;
        oss << "Error: " << s << " when processing \"" << line << "\".";
        mexErrMsgTxt( oss.str().c_str() );
      }
      mxFree( line );
    }
    else
      mexErrMsgTxt( "Expected a cell array of strings as input." );
  }
  return ParamlistToStruct( params );
}

mxArray*
StructToStrings( const mxArray* inStruct )
{
  PARAMLIST params;
  StructToParamlist( inStruct, params );
  size_t numParams = params.Size();
  mxArray* strings = mxCreateCellMatrix( numParams, 1 );
  for( size_t i = 0; i < numParams; ++i )
  {
    ostringstream oss;
    oss << params[ i ];
    mxSetCell( strings, i, mxCreateString( oss.str().c_str() ) );
  }
  return strings;
}