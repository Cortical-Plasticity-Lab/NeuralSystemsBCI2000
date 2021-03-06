//////////////////////////////////////////////////////////////////////
// $Id$
// Author: juergen.mellinger@uni-tuebingen.de
// Description: A class that wraps a thread, and allows clients to
//   run code inside that thread. Unlike OSThread, which starts a new
//   thread each time its Start() function is called, an instance of
//   ReusableThread is bound to a single thread during its lifetime, and
//   re-uses that thread for each call to Run(). After calling Run()
//   for a Runnable, call Wait() to wait until execution of the
//   Runnable has finished.
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
#include "ReusableThread.h"
#include "Exception.h"

ReusableThread::ReusableThread()
: mAlive( true ),
  mpRunnable( 0 )
{
  mFinishedEvent.Set();
  Thread::Start();
}

ReusableThread::~ReusableThread()
{
  SharedPointer<Waitable> pTerminationEvent = Thread::Terminate();
  mStartEvent.Set();
  if( !Thread::InOwnThread() )
    pTerminationEvent->Wait();
}

bool
ReusableThread::Run( Runnable& inRunnable )
{
  if( !mAlive )
    throw std_runtime_error( "Thread is no longer available for execution" );
  {
    SpinLock::Lock _( mLock );
    if( mpRunnable )
      return false;
    mpRunnable = &inRunnable;
    mFinishedEvent.Reset();
  }
  mStartEvent.Set();
  return true;
}

bool
ReusableThread::Wait( int inTimeout )
{
  return mFinishedEvent.Wait( inTimeout );
}

bool
ReusableThread::Alive() const
{
  return mAlive;
}

int
ReusableThread::OnExecute()
{
  while( !Thread::IsTerminating() )
  {
    mStartEvent.Wait();
    mStartEvent.Reset();
    if( !Thread::IsTerminating() )
      mpRunnable->Run();
    SpinLock::Lock _( mLock );
    mpRunnable = 0;
    mFinishedEvent.Set();
  }
  mAlive = false;
  return 0;
}

void
ReusableThread::OnFinished()
{
  mAlive = false;
  SpinLock::Lock _( mLock );
  if( mpRunnable )
  {
    mpRunnable = 0;
    mFinishedEvent.Set();
  }
}
