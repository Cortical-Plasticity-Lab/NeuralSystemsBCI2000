////////////////////////////////////////////////////////////////////////////////
// $Id$
// Author: juergen.mellinger@uni-tuebingen.de
// Description: A filter that logs keyboard and mouse keypresses into states.
//   Useful for experiments that require tracking of user responses.
//   A "Keyboard" state contains the key's "virtual key code" as defined by
//   the Win32 API, and additionally a bit which is set when a key is released.
//   In the "MouseKeys" state, bit 0 represents left and bit 1 represents the
//   right mouse button.
//   In the MousePosX and MousePosY states, mouse cursor position is stored in
//   device coordinates (i.e. coordinates that are in units of screen pixels).
//
// (C) 2000-2008, BCI2000 Project
// http://www.bci2000.org
////////////////////////////////////////////////////////////////////////////////
#include "PCHIncludes.h"
#pragma hdrstop

#include "KeyLogger.h"

#include "BCIEvent.h"
#include "OSError.h"
#include <sstream>
#include <windows.h>

using namespace std;

static KeyLogger sInstance;

int  KeyLogger::HookThread::sInstances = 0;
bool KeyLogger::HookThread::sKeyPressed[ 1 << KeyLogger::KeyboardBits ];
int  KeyLogger::HookThread::sMouseKeys = 0;
HHOOK KeyLogger::HookThread::sKeyboardHook = NULL;
HHOOK KeyLogger::HookThread::sMouseHook = NULL;

KeyLogger::KeyLogger()
: mLogKeyboard( false ),
  mLogMouse( false ),
  mpThread( NULL )
{
}

KeyLogger::~KeyLogger()
{
  Halt();
}

void
KeyLogger::Publish()
{
  if( ( OptionalParameter( "LogKeyboard" ) != 0 ) )
  {
    BEGIN_PARAMETER_DEFINITIONS
      "Source:Log%20Input int LogKeyboard= 1 0 0 1 "
      " // record keyboard to states (boolean)",
    END_PARAMETER_DEFINITIONS

    BEGIN_STATE_DEFINITIONS
      "KeyDown 8 0 0 0",
      "KeyUp   8 0 0 0",
    END_STATE_DEFINITIONS
  }

  if( ( OptionalParameter( "LogMouse", 0 ) != 0 ) )
  {
    BEGIN_PARAMETER_DEFINITIONS
      "Source:Log%20Input int LogMouse= 1 0 0 1 "
      " // record mouse to states (boolean)",
    END_PARAMETER_DEFINITIONS

    BEGIN_STATE_DEFINITIONS
      "MouseKeys  2 0 0 0",
      "MousePosX 16 0 0 0",
      "MousePosY 16 0 0 0",
    END_STATE_DEFINITIONS
  }
}

void
KeyLogger::Preflight() const
{
  OptionalParameter( "LogKeyboard" );
  OptionalParameter( "LogMouse" );
}

void
KeyLogger::Initialize()
{
  Halt();
  mLogKeyboard = ( OptionalParameter( "LogKeyboard" ) != 0 );
  mLogMouse = ( OptionalParameter( "LogMouse" ) != 0 );
}

void
KeyLogger::StartRun()
{
  if( mLogKeyboard || mLogMouse )
    mpThread = new HookThread( mLogKeyboard, mLogMouse );
}

void
KeyLogger::StopRun()
{
  Halt();
}

void
KeyLogger::Halt()
{
  if( mpThread != NULL )
  {
    mpThread->Terminate();
    while( !mpThread->IsTerminated() )
      ::Sleep( 0 );
    delete mpThread;
    mpThread = NULL;
  }
}


KeyLogger::HookThread::HookThread( bool inLogKeyboard, bool inLogMouse )
: mLogKeyboard( inLogKeyboard ),
  mLogMouse( inLogMouse )
{
  if( ++sInstances > 1 )
    throw "Logic error: More than one instance of KeyLogger::HookThread";
}

KeyLogger::HookThread::~HookThread()
{
  --sInstances;
}

int
KeyLogger::HookThread::Execute()
{
  if( mLogKeyboard )
    InstallKeyboardHook();
  if( mLogMouse )
    InstallMouseHook();
  int result = OSThread::Execute();
  UninstallHooks();
  return result;
}

void
KeyLogger::HookThread::InstallKeyboardHook()
{
  HINSTANCE module = ::GetModuleHandle( NULL );
  if( sKeyboardHook == NULL )
    sKeyboardHook = ::SetWindowsHookEx( WH_KEYBOARD_LL,
      reinterpret_cast<HOOKPROC>( LowLevelKeyboardProc ), module, 0 );
}


void
KeyLogger::HookThread::InstallMouseHook()
{
  HINSTANCE module = ::GetModuleHandle( NULL );
  if( sMouseHook == NULL )
  {
    sMouseKeys = 0;
    sMouseHook = ::SetWindowsHookEx( WH_MOUSE_LL,
      reinterpret_cast<HOOKPROC>( LowLevelMouseProc ), module, 0 );
  }
}


void
KeyLogger::HookThread::UninstallHooks()
{
  if( sKeyboardHook != NULL )
  {
    ::UnhookWindowsHookEx( sKeyboardHook );
    sKeyboardHook = NULL;
  }
  if( sMouseHook != NULL )
  {
    ::UnhookWindowsHookEx( sMouseHook );
    sMouseHook = NULL;
  }
}


LRESULT CALLBACK
KeyLogger::HookThread::LowLevelKeyboardProc( int inCode, WPARAM inWParam, LPARAM inLParam )
{
  if( inCode >= 0 )
  {
    KBDLLHOOKSTRUCT* pData = reinterpret_cast<KBDLLHOOKSTRUCT*>( inLParam );
    switch( inWParam )
    {
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
        if( !sKeyPressed[ pData->vkCode ] )
        {
          sKeyPressed[ pData->vkCode ] = true;
          bcievent << "KeyDown " << pData->vkCode << " 0";
        }
        break;

      case WM_KEYUP:
      case WM_SYSKEYUP:
        sKeyPressed[ pData->vkCode ] = false;
        bcievent << "KeyUp " << pData->vkCode << " 0";
        break;

      default:
        ;
    }
  }
  return ::CallNextHookEx( sKeyboardHook, inCode, inWParam, inLParam );
}


LRESULT CALLBACK
KeyLogger::HookThread::LowLevelMouseProc( int inCode, WPARAM inWParam, LPARAM inLParam )
{
  if( inCode >= 0 )
  {
    switch( inWParam )
    {
      case WM_LBUTTONDOWN:
        sMouseKeys |= LButtonMask;
        break;

      case WM_LBUTTONUP:
        sMouseKeys &= ~LButtonMask;
        break;

      case WM_RBUTTONDOWN:
        sMouseKeys |= RButtonMask;
        break;

      case WM_RBUTTONUP:
        sMouseKeys &= ~RButtonMask;
        break;

      default:
        ;
    }
    switch( inWParam )
    {
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
        bcievent << "MouseKeys " << sMouseKeys;
        break;

      case WM_MOUSEMOVE:
      {
        MSLLHOOKSTRUCT* pData = reinterpret_cast<MSLLHOOKSTRUCT*>( inLParam );
        bcievent << "MousePosX " << pData->pt.x;
        bcievent << "MousePosY " << pData->pt.y;
      } break;

      default:
        ;
    }
  }
  return ::CallNextHookEx( sMouseHook, inCode, inWParam, inLParam );
}

