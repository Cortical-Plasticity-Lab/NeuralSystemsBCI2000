////////////////////////////////////////////////////////////////////////////////
// $Id$
// Author: juergen.mellinger@uni-tuebingen.de
// Description: A class template that encapsulates list i/o.
//
// (C) 2000-2008, BCI2000 Project
// http://www.bci2000.org
////////////////////////////////////////////////////////////////////////////////
#ifndef VALUE_LIST_H
#define VALUE_LIST_H

#include <iostream>
#include <vector>
#include "Brackets.h"

template<typename T> class ValueList: public std::vector<T>
{
  public:
    ValueList( size_t inSize = 0 ) : std::vector<T>( inSize ) {}

    std::ostream& WriteToStream( std::ostream& ) const;
    std::istream& ReadFromStream( std::istream& );
};

template<typename T>
std::istream&
ValueList<T>::ReadFromStream( std::istream& is )
{
  clear();
  is >> ws;
  char closingBracket = '\0';
  if( Brackets::IsOpening( is.peek() ) )
    closingBracket = Brackets::ClosingMatch( is.get() );
  else
    is.setstate( ios::failbit );
  is >> ws;

  T t;
  while( is.peek() != closingBracket && is >> t >> ws )
    push_back( t );

  if( is.peek() == closingBracket )
    is.get();
  else
    is.setstate( ios::failbit );
  return is;
}

template<typename T>
std::ostream&
ValueList<T>::WriteToStream( std::ostream& os ) const
{
  os << Brackets::OpeningDefault;
  for( const_iterator i = begin(); i != end(); ++i )
    os << *i << " ";
  os << Brackets::ClosingDefault;
  return os;
}

template<typename T>
inline std::ostream&
operator<<( std::ostream& s, const ValueList<T>& v )
{
  return v.WriteToStream( s );
}

template<typename T>
inline std::istream&
operator>>( std::istream& s, ValueList<T>& v )
{
  return v.ReadFromStream( s );
}

#endif // VALUE_LIST_H
