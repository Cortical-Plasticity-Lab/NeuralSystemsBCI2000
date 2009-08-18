////////////////////////////////////////////////////////////////////////////////
// $Id: gMOBIlabADC.h 1439 2007-07-19 09:44:43Z mellinger $
// Author: schalk@wadsworth.org
// Description: BCI2000 Source Module for gMOBIlab devices.
//
// (C) 2000-2009, BCI2000 Project
// http://www.bci2000.org
////////////////////////////////////////////////////////////////////////////////
#ifndef gMOBIlabPlus_ADC_H
#define gMOBIlabPlus_ADC_H

#include "GenericADC.h"
#include "gMOBIlabThread.h"

class gMOBIlabPlusADC : public GenericADC
{
 public:
               gMOBIlabPlusADC();
  virtual      ~gMOBIlabPlusADC();

  virtual void Preflight( const SignalProperties&, SignalProperties& ) const;
  virtual void Initialize( const SignalProperties&, const SignalProperties& );
  virtual void Process( const GenericSignal&, GenericSignal& );
  virtual void Halt();

 private:
  bool            mEnableDigOut;
  HANDLE          mDev;
  int mDigState;
  gMOBIlabThread* mpAcquisitionThread;
};

#endif // GMOBILAB_ADC_H

