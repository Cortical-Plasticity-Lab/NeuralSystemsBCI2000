////////////////////////////////////////////////////////////////////////////////
// $Id$
// Author: juergen.mellinger@uni-tuebingen.de
// Description: A C++ representation of a BCI2000 relevant subset of the EDF
//              data format as defined in Kemp et al, 1992, and the
//              GDF 1.25 data format as defined in Schloegl et al, 1998.
//
// (C) 2000-2008, BCI2000 Project
// http://www.bci2000.org
////////////////////////////////////////////////////////////////////////////////
#include "PCHIncludes.h"
#pragma hdrstop

#include "GDF.h"

#include "BCIError.h"

#include <iomanip>
#include <string>
#include <ctime>
#include <limits>


using namespace std;

void
GDF::EncodedString::WriteToStream( std::ostream& os ) const
{
  if( empty() )
    os.put( 'X' );
  for( const_iterator i = begin(); i != end(); ++i )
    isspace( *i ) ? os.put( '_' ) : os.put( *i );
}

string
GDF::DateTimeToString( signed long long t )
{
  ostringstream oss;
  // Not all OSes interpret negative time_t values correctly as dates before
  // 1970. In this case (birthdays), we just report the corresponding year.
  if( t == cInvalidDate )
    oss << "XX-XXX-XXXX";
  else if( t < 0 || t > numeric_limits<time_t>::max() )
  {
    oss << "XX-XXX-" << 1970 + t / cSecondsPerYear;
  }
  else
  {
    static const char* monthNames[] =
    {
      "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
      "JUL", "AUG", "SEP", "OCT", "NOV", "DEC",
    };
    time_t timeVal = t;
    struct tm* time = localtime( &timeVal );
    oss << setw( 2 ) << setfill( '0' ) << time->tm_mday
        << '-' << monthNames[ time->tm_mon ]
        << '-' << 1900 + time->tm_year;
  }
  return oss.str();
}

GDF::int64::ValueType
GDF::DateTimeToGDFTime( signed long long t )
{
  return ( double( t ) / cSecondsPerDay + cDaysUpTo1970 ) * ( 1LL << 32 );
}

GDF::int64::ValueType
GDF::YearToGDFTime( double y )
{
  return y * cSecondsPerDay / cSecondsPerYear * ( 1LL << 32 );
}

GDF::uint16::ValueType
GDF::PhysicalUnitToGDFUnit( const std::string& inUnit )
{
  // Convert a subset of physical unit in ASCII format into an
  // ISO/IEEE 11073-10101:2004 number code.
  const struct
  {
    const char*       name;
    uint16::ValueType value;
  }
  prefixes[] =
  {
    { "Z",   9 },
    { "E",   8 },
    { "P",   7 },
    { "T",   6 },
    { "G",   5 },
    { "M",   4 },
    { "k",   3 },
    { "h",   2 },
    { "da",  1 },
    { "d",  16 },
    { "c",  17 },
    { "m",  18 },
    { "u",  19 },
    { "mu", 19 },
    { "n",  20 },
    { "p",  21 },
    { "f",  22 },
    { "a",  23 },
    { "z",  24 },
    { "y",  25 },
  },
  units[] =
  {
    { "",      512 },
    { "-",     512 },
    { "m",    1280 },
    { "s",    2176 },
    { "A",    4160 },
    { "V",    4256 },
    { "Ohm",  4288 },
    { "T",   65504 },
  };

  int pos = 0,
      prefixValue = 0;
  for( int i = 0; prefixValue == 0 && i < sizeof( prefixes ) / sizeof( *prefixes ); ++i )
    if( inUnit.find( prefixes[ i ].name ) == 0 )
    {
      pos += ::strlen( prefixes[ i ].name );
      prefixValue = prefixes[ i ].value;
    }

  string unit = inUnit.substr( pos );
  int unitValue = 0;
  for( int i = 0; unitValue == 0 && i < sizeof( units ) / sizeof( *units ); ++i )
    if( unit == units[ i ].name )
      unitValue = units[ i ].value;


  return unitValue + prefixValue;
}


