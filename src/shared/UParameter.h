/******************************************************************************
 * Program:   BCI2000                                                         *
 * Module:    UParameter.h                                                    *
 * Comment:   This unit provides support for system-wide parameters           *
 *            and parameter lists                                             *
 * Version:   0.22                                                            *
 * Authors:   Gerwin Schalk, Juergen Mellinger                                *
 * Copyright: (C) Wadsworth Center, NYSDOH                                    *
 ******************************************************************************
 * Version History:                                                           *
 *                                                                            *
 * V0.08 - 03/30/2000 - First commented version                               *
 * V0.10 - 05/12/2000 - added AddParam2List(char *paramline)                  *
 *                      CloneParameter2List now updates the values of an      *
 *                      existing parameter                                    *
 * V0.11 - 06/09/2000 - Archive flag to each parameter                        *
 *                      better validity check in ParseParameter()             *
 * V0.13 - 08/09/2000 - Parameter supports datatype matrix                    *
 * V0.14 - 09/25/2000 - load and save parameter files                         *
 * V0.16 - 04/30/2001 - sorting of parameter lists; numerous other changes    *
 * V0.17 - 06/20/2002 - introduction of const, private/protected              *
 *                      juergen.mellinger@uni-tuebingen.de                    *
 * V0.18 - 01/31/2003 - fixed bug in SaveParameterList()                      *
 * V0.19 - 01/09/2003 - completely rewrote implementation based on STL,       *
 *                      juergen.mellinger@uni-tuebingen.de                    *
 * V0.20 - 05/07/2003 - Added textual index labels for matrices and lists, jm *
 * V0.21 - 05/15/2003 - Fixed invalid iterator problem in SaveParameterList(),*
 *                      jm                                                    *
 * V0.22 - 05/30/2003 - Fixed index synchronization bug in                    *
 *                      PARAM::SetNumValues(), jm                             *
 * V0.23 - 11/24/2003 - Fixed parsing of matrices with 0x0 size               *
 *                      Preserve existing values in SetDimensions, jm         *
 * V0.24 - 11/28/2003 - Added aliases for some functions that contain         *
 *                      dimension names, e.g. PARAM::GetNumRows(),            *
 *                      PARAM::RowLabels(), jm                                *
 ******************************************************************************/
#ifndef UParameterH
#define UParameterH

#include <locale>
#include <string>
#include <vector>
#include <map>
#include "EncodedString.h"

class PARAM
{
  friend class PARAMLIST;

 public:
   // A helper class to handle string labels for indexing matrices
   // and lists.
   typedef std::map<EncodedString, size_t> indexer_base;
   typedef std::vector<indexer_base::key_type> indexer_reverse;
   class labelIndexer
   {
     friend class PARAM;
     private:
      labelIndexer() : needSync( false ) { clear(); }
      ~labelIndexer() {}

     public:
      // Forward lookup.
      indexer_base::mapped_type operator[]( const std::string& ) const;
      // We need this because the above operator must return 0 for nonexisting
      // labels.
      bool Exists( const std::string& ) const;
      // A reverse lookup operator.
      const std::string& operator[]( size_t ) const;
      std::string& operator[]( size_t );

      bool IsTrivial() const;
      static const std::string& TrivialLabel( size_t );

      // Stream I/O.
      std::ostream& WriteToStream( std::ostream& ) const;
      std::istream& ReadFromStream( std::istream& );

     private:
      void resize( size_t );
      size_t size() const { return reverseIndex.size(); }
      void sync() const;
      void clear()
      {
        reverseIndex.clear();
        forwardIndex.clear();
        resize( 1 );
      }

     private:
      // This is the maintained index.
      indexer_reverse reverseIndex;
      // This is a cache for the more probable lookup direction.
      mutable bool needSync;
      mutable indexer_base forwardIndex;
   };

  // A class that represents a single parameter value entry, accommodating
  // strings and subparameters.
  public:
   class paramValue
   {
    public:
     enum
     {
       Null,
       Single,
       List,
       Matrix
     };

     paramValue()
       : mpString( new EncodedString ), mpParam( NULL )
       {}
     paramValue( const paramValue& p )
       : mpString( NULL ), mpParam( NULL )
       { Assign( p ); }
     paramValue( const char* s )
       : mpString( new EncodedString( s ) ), mpParam( NULL )
       {}
     paramValue( const std::string& s )
       : mpString( new EncodedString( s ) ), mpParam( NULL )
       {}
     paramValue( const PARAM& p )
       : mpString( NULL ), mpParam( new PARAM( p ) )
       {}
     ~paramValue()
       { delete mpString; delete mpParam; }

     const paramValue& operator=( const paramValue& p )
       { Assign( p ); return *this; }
     const paramValue& operator=( const char* s )
       { Assign( std::string( s ) ); return *this; }
     const paramValue& operator=( const std::string& s )
       { Assign( s ); return *this; }
     const paramValue& operator=( const PARAM& p )
       { Assign( p ); return *this; }

     operator const char*() const
       { return ToString().c_str(); }
     operator const std::string&() const
       { return ToString(); }
     operator PARAM*()
       { return ToParam(); }
     operator const PARAM*() const
       { return ToParam(); }
     PARAM* operator->()
       { return ToParam(); }
     const PARAM* operator->() const
       { return ToParam(); }

     int Kind() const;

     void Assign( const paramValue& );
     void Assign( const std::string& );
     void Assign( const PARAM& );
     const std::string& ToString() const;
     const PARAM*       ToParam() const;
     PARAM*             ToParam();
     
     std::ostream& WriteToStream( std::ostream& os ) const;
     std::istream& ReadFromStream( std::istream& is );

    private:
     void ConstructParamBuf() const;

     EncodedString* mpString;
     PARAM*         mpParam;

     static PARAM       sParamBuf;
     static std::string sStringBuf;
  };

 public:
        PARAM();
        PARAM( const char* name,
               const char* section,
               const char* type,
               const char* value,
               const char* defaultvalue,
               const char* lowrange,
               const char* highrange,
               const char* comment );
        explicit PARAM( const char* paramstring );
        ~PARAM() {}

        PARAM& operator=( const PARAM& );

        void    SetSection( const std::string& s )
                { mSection = s; }
        void    SetType( const std::string& s )
                { mType = s; tolower( mType ); }
 private:
        // Changing the name without changing its index in the list would be
        // a bad idea, so this function is private.
        void    SetName( const std::string& s )
                { mName = s; }
 public:
        void    SetNumValues( size_t n );
        void    SetValue( const std::string& s )
                { SetValue( s, 0 ); }
        void    SetValue( const std::string&, size_t );
        void    SetValue( const std::string& s, size_t n, size_t m )
                { SetValue( s, n * GetNumValuesDimension2() + m ); }
        void    SetValue( const std::string& s,
                          const std::string& label )
                { return SetValue( s, mDim1Index[ label ] ); }
        void    SetValue( const std::string& s,
                          const std::string& label_dim1, const std::string& label_dim2 )
                { return SetValue( s, mDim1Index[ label_dim1 ], mDim2Index[ label_dim2 ] ); }
        void    SetValue( const std::string& s,
                          size_t index_dim1, const std::string& label_dim2  )
                { return SetValue( s, index_dim1, mDim2Index[ label_dim2 ] ); }
        void    SetValue( const std::string& s,
                          const std::string& label_dim1, size_t index_dim2 )
                { return SetValue( s, mDim1Index[ label_dim1 ], index_dim2 ); }

  const char*   GetSection() const
                { return mSection.c_str(); }
  const char*   GetType() const
                { return mType.c_str(); }
  const char*   GetName() const
                { return mName.c_str(); }
  const char*   GetDefaultValue() const
                { return mDefaultvalue.c_str(); }
  const char*   GetLowRange() const
                { return mLowrange.c_str(); }
  const char*   GetHighRange() const
                { return mHighrange.c_str(); }
  const char*   GetComment() const
                { return mComment.c_str(); }
        size_t  GetNumValues() const
                { return mValues.size(); }
        size_t  GetNumValuesDimension1() const
                { return GetNumValues() / GetNumValuesDimension2(); }
        size_t  GetNumRows() const
                { return GetNumValuesDimension1(); }
        size_t  GetNumValuesDimension2() const
                { return mDim2Index.size(); }
        size_t  GetNumColumns() const
                { return GetNumValuesDimension2(); }
        void    SetDimensions( size_t, size_t );
  const char*   GetValue() const
                { return GetValue( 0 ); }
  const char*   GetValue( size_t ) const;
  const char*   GetValue( size_t n, size_t m ) const
                { return GetValue( n * GetNumValuesDimension2() + m ); }
  const char*   GetValue( const std::string& label ) const
                { return GetValue( mDim1Index[ label ] ); }
  const char*   GetValue( const std::string& label_dim1, const std::string& label_dim2 ) const
                { return GetValue( mDim1Index[ label_dim1 ], mDim2Index[ label_dim2 ] ); }
  const char*   GetValue( size_t index_dim1, const std::string& label_dim2  ) const
                { return GetValue( index_dim1, mDim2Index[ label_dim2 ] ); }
  const char*   GetValue( const std::string& label_dim1, size_t index_dim2 ) const
                { return GetValue( mDim1Index[ label_dim1 ], index_dim2 ); }

  // Accessors for bounds-checked, recursive access to parameter entries.
  const paramValue& Value( size_t idx ) const
                { return mValues.at( idx ); }
  paramValue& Value( size_t idx )
                { return mValues.at( idx ); }

  const paramValue& Value( size_t n, size_t m ) const
                { return Value( n * GetNumValuesDimension2() + m ); }
  paramValue& Value( size_t n, size_t m )
                { return Value( n * GetNumValuesDimension2() + m ); }

  const paramValue& Value( const std::string& label ) const
                { return Value( mDim1Index[ label ] ); }
  paramValue& Value( const std::string& label )
                { return Value( mDim1Index[ label ] ); }

  const paramValue& Value( size_t index_dim1, const std::string& label_dim2  ) const
                { return Value( index_dim1, mDim2Index[ label_dim2 ] ); }
  paramValue& Value( size_t index_dim1, const std::string& label_dim2 )
                { return Value( index_dim1, mDim2Index[ label_dim2 ] ); }

  const paramValue& Value( const std::string& label_dim1, size_t index_dim2 ) const
                { return Value( mDim1Index[ label_dim1 ], index_dim2 ); }
  paramValue& Value( const std::string& label_dim1, size_t index_dim2 )
                { return Value( mDim1Index[ label_dim1 ], index_dim2 ); }

  const paramValue& Value( const std::string& label_dim1, const std::string& label_dim2 ) const
                { return Value( mDim1Index[ label_dim1 ], mDim2Index[ label_dim2 ] ); }
  paramValue& Value( const std::string& label_dim1, const std::string& label_dim2 )
                { return Value( mDim1Index[ label_dim1 ], mDim2Index[ label_dim2 ] ); }

  labelIndexer& LabelsDimension1()
                { mChanged = true; return mDim1Index; }
  labelIndexer& RowLabels()
                { return LabelsDimension1(); }
  const labelIndexer& LabelsDimension1() const
                { return mDim1Index; }
  const labelIndexer& RowLabels() const
                { return LabelsDimension1(); }
  labelIndexer& LabelsDimension2()
                { mChanged = true; return mDim2Index; }
  labelIndexer& ColumnLabels()
                { return LabelsDimension2(); }
  const labelIndexer& LabelsDimension2() const
                { return mDim2Index; }
  const labelIndexer& ColumnLabels() const
                { return LabelsDimension2(); }
  labelIndexer& Labels()
                { mChanged = true; return mDim1Index; }
  const labelIndexer& Labels() const
                { return mDim1Index; }
        bool    Changed() const
                { return mChanged; }
        void    Unchanged()
                { mChanged = false; }
        bool&   Archive()
                { return mArchive; }
        bool    Archive() const
                { return mArchive; }
        bool&   Tag()
                { return mTag; }
        bool    Tag() const
                { return mTag; }

        std::ostream& WriteToStream( std::ostream& ) const;
        std::istream& ReadFromStream( std::istream& );
        std::ostream& WriteBinary( std::ostream& ) const;
        std::istream& ReadBinary( std::istream& );

 private:
        EncodedString mSection,
                      mName,
                      mType,
                      mDefaultvalue,
                      mLowrange,
                      mHighrange;
        std::string   mComment;
        labelIndexer  mDim1Index,
                      mDim2Index;
        typedef std::vector<paramValue> values_type;
        values_type   mValues;
        bool          mChanged,
                      mArchive,
                      mTag;  // used for parameter save/load filters


 // Case insensitive string handling components.
 private:
  static const std::ctype<char>& ct;

 public:
  static void tolower( std::string& s )
  { ct.tolower( s.begin(), s.end() ); }
  static void toupper( std::string& s )
  { ct.toupper( s.begin(), s.end() ); }

  static bool ciless( char a, char b )
  { return ct.toupper( a ) < ct.toupper( b ); }
  static bool ciequal( char a, char b )
  { return ct.toupper( a ) == ct.toupper( b ); }

  static bool strciless( const std::string& a, const std::string& b )
  { return namecmp()( a, b ); }
  static bool strciequal( const std::string& a, const std::string& b )
  { return !strciless( a, b ) && !strciless( b, a ); }

  class namecmp
  {
   public:
    bool operator()( const std::string& a, const std::string& b ) const
    { return std::lexicographical_compare(
                        a.begin(), a.end(), b.begin(), b.end(), ciless ); }
  };
};


typedef std::map<std::string, PARAM, PARAM::namecmp> param_container;

class PARAMLIST : public param_container
{
 public:
        PARAM*  GetParamPtr( const char* name );
  const PARAM*  GetParamPtr( const char* name ) const;

        size_t  GetNumParameters() const
                { return size(); }
        void    ClearParamList()
                { clear(); }
        void    DeleteParam( const char* name )
                { erase( name ); }

        bool    AddParameter2List( const char* paramstring,
                                   size_t paramlen = 0 );
        bool    SaveParameterList( const char* filename,
                                   bool usetags = false ) const;
        bool    LoadParameterList( const char* filename,
                                   bool usetags = false,
                                   bool importnonexisting = true );

  // These contain all formatted I/O functionality.
        std::ostream& WriteToStream( std::ostream& ) const;
        std::istream& ReadFromStream( std::istream& );

  // These define binary I/O.
        std::ostream& WriteBinary( std::ostream& ) const;
        std::istream& ReadBinary( std::istream& );

  // These are for compatibility.
        PARAM*  GetParamPtr( size_t );
  const PARAM*  GetParamPtr( size_t ) const;
};


inline std::ostream& operator<<( std::ostream& s, const PARAM::labelIndexer& i )
{
  return i.WriteToStream( s );
}

inline std::istream& operator>>( std::istream& s, PARAM::labelIndexer& i )
{
  return i.ReadFromStream( s );
}

inline std::ostream& operator<<( std::ostream& s, const PARAM::paramValue& p )
{
  return p.WriteToStream( s );
}

inline std::istream& operator>>( std::istream& s, PARAM::paramValue& p )
{
  return p.ReadFromStream( s );
}

inline std::ostream& operator<<( std::ostream& s, const PARAM& p )
{
  return p.WriteToStream( s );
}

inline std::istream& operator>>( std::istream& s, PARAM& p )
{
  return p.ReadFromStream( s );
}

inline std::ostream& operator<<( std::ostream& s, const PARAMLIST& p )
{
  return p.WriteToStream( s );
}

inline std::istream& operator>>( std::istream& s, PARAMLIST& p )
{
  return p.ReadFromStream( s );
}

#endif // UParameterH

