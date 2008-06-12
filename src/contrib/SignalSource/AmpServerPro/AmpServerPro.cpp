/* (C) 2000-2008, BCI2000 Project
/* http://www.bci2000.org
/*/
#include "PCHIncludes.h"
#pragma hdrstop

#include <vcl.h>
#include "CoreModuleVCL.h"
#include <iostream.h>


WINAPI
WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{                               

  try
  {
    Application->Initialize();
    Application->Title = "AmpServerPro Signal Source";
    CoreModuleVCL().Run( _argc, _argv );
  }
  catch (Exception &exception)
  {
    Application->ShowException(&exception);
  }
  return 0;

}
