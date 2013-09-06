//////////////////////////////////////////////////////////////////////
// $Id: OSThreadLocal.h 4105 2012-06-13 15:24:54Z mellinger $
// Author: juergen.mellinger@uni-tuebingen.de
// Description: A shared memory wrapper class.
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
///////////////////////////////////////////////////////////////////////
#include "OSSharedMemory.h"

#include "RandomGenerator.h"
#include "OSError.h"
#include "BCIException.h"
#include "FileUtils.h"

#if _WIN32
# include <Windows.h>
#else
# include <sys/mman.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
#endif // _WIN32

#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>

using namespace std;

namespace {

LCRandomGenerator RG;

enum
{
  none = 0,
  shm,
  file,
};

const struct ProtocolEntry { const char* name; int id; }
Protocols[] =
{
  { "file://", file },
  { "shm://", shm },
  { 0, shm }
};

} // namespace

OSSharedMemory::OSSharedMemory( const std::string& inName, size_t inSize )
: mName( inName ),
  mServer( inSize != 0 ),
  mSize( inSize ),
  mpMemory( 0 ),
  mProtocol( none )
{
  Initialize();
}

OSSharedMemory::OSSharedMemory( size_t inSize )
: mServer( true ),
  mSize( inSize ),
  mpMemory( 0 ),
  mProtocol( none )
{
  Initialize();
}

OSSharedMemory::~OSSharedMemory()
{
  UnmapMemory();
  Close();
  if( mServer )
    Destroy();
}

void
OSSharedMemory::Initialize()
{
  mHandle.fd = -1;
  ParseProtocol();
  NormalizeName();
  if( mServer )
    Create();
  else
    Open();
  MapMemory();
}
  
void
OSSharedMemory::ParseProtocol()
{
  size_t nameOffset = 0;
  const ProtocolEntry* p = Protocols;
  while( p->name )
  {
    if( mProtocol == none )
    {
      size_t len = ::strlen( p->name );
      if( mName.length() >= len && !::stricmp( mName.substr( 0, len ).c_str(), p->name ) )
      {
        mProtocol = p->id;
        nameOffset = len;
      }
    }
    ++p;
  }
  if( mProtocol == none )
  {
    mProtocol = p->id;
    nameOffset = 0;
  }
  p = Protocols;
  while( p->name && p->id != mProtocol )
    ++p;
  if( p->name )
    mProtocolString = p->name;
  mName = mName.substr( nameOffset );
}

void
OSSharedMemory::NormalizeName()
{
  if( mProtocol == shm )
  {
    if( mName.empty() )
      mName = RG.RandomName( 16 );
    if( mName[0] != '/' )
      mName = '/' + mName;
    size_t pos = 1;
    while( pos = mName.find_first_of( ":/\\<>|", pos ) != string::npos )
      mName[pos++] = '_';
    if( mName.length() > 255 )
      mName = mName.substr( 128 );
  }
  else if( mProtocol == file )
  {
    if( mServer && mName.empty() )
    {
      do
        mName = FileUtils::TemporaryFile::GenerateName();
      while( FileUtils::IsFile( mName ) );
    }
  }
}

void
OSSharedMemory::Create()
{
  if( mProtocol == shm )
  {
#if _WIN32
    mHandle.h = 0;
    while( mHandle.h == 0 )
    {
      mHandle.h = ::CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, mSize, mName.c_str() + 1 );
      if( ::GetLastError() == ERROR_ALREADY_EXISTS )
      {
        mHandle.h = 0;
        mName += RG.RandomCharacter();
        NormalizeName();
      }
    }
#else // _WIN32
    errno = 0;
    mHandle.fd = -1;
    while( mHandle.fd < 0 && errno == 0 )
    {
      mHandle.fd = ::shm_open( mName.c_str(), O_RDRW | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR );
      if( mHandle.fd < 0 && errno == EEXIST )
      {
        errno = 0;
        mName += RG.RandomCharacter();
        NormalizeName();
      }
      if( mHandle.fd >= 0 && ::ftruncate( mHandle.fd, mSize ) )
      {
        ::close( mHandle.fd );
        mHandle.fd = -1;
      }
    }
#endif // _WIN32
  }
  else if( mProtocol == file )
  {
    fstream f( mName, ios::binary | ios::out | ios::trunc );
    f.fill( '%' );
    f << setw( mSize ) << f.fill() << flush;
    f.close();
    Open();
  }
}

void
OSSharedMemory::Destroy()
{
  if( mProtocol == shm )
  {
#if _WIN32
    /* will be destroyed after all handles are closed */
#else
    ::shm_unlink( mName.c_str()  );
#endif
  }
  else if( mProtocol == file )
    FileUtils::RemoveFile( mName );
}

void
OSSharedMemory::Open()
{
#if _WIN32
  if( mProtocol == shm )
    mHandle.h = ::OpenFileMappingA( FILE_MAP_READ | FILE_MAP_WRITE, FALSE, mName.c_str() + 1 );
  else if( mProtocol == file )
  {
    HANDLE hFile = ::CreateFileA(
      mName.c_str(), 
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      0,
      OPEN_EXISTING,
      0,
      0
    );
    if( hFile )
      mHandle.h = ::CreateFileMappingA( hFile, NULL, PAGE_READWRITE, 0, 0, 0 );
    if( !mHandle.h )
      throw std_runtime_error( OSError().Message() );
  }
#else // _WIN32
  switch( mProtocol )
  {
    case shm:
      mHandle.fd = ::shm_open( mName.c_str(), O_RDRW, S_IRUSR | S_IWUSR );
      break;
    case file:
      mHandle.fd = ::open( mName.c_str(), O_RDRW, S_IRUSR | S_IWUSR );
      break;
  }
  if( mHandle.fd >= 0 )
  {
    struct stat s = { 0 };
    if( !::fstat( mHandle.fd, &s ) )
      mSize = s.st_size;
  }
#endif // _WIN32
}

void
OSSharedMemory::Close()
{
#if _WIN32
  ::CloseHandle( mHandle.h );
#else // _WIN32
  ::close( mHandle.fd );
#endif // _WIN32
}

void
OSSharedMemory::MapMemory()
{
  if( mpMemory )
    return;
#if _WIN32
  if( mHandle.h )
    mpMemory = ::MapViewOfFile( mHandle.h, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
#else // _WIN32
  if( mHandle.fd >= 0 )
    mpMemory = ::mmap( 0, 0, PROT_READ | PROT_WRITE, MAP_SHARED, mHandle.fd, 0 );
#endif // _WIN32
}

void
OSSharedMemory::UnmapMemory()
{
  if( !mpMemory )
    return;
#if _WIN32
  ::UnmapViewOfFile( mpMemory );
#else // _WIN32
  ::munmap( mpMemory, mSize );
#endif // _WIN32
  mpMemory = 0;
}
