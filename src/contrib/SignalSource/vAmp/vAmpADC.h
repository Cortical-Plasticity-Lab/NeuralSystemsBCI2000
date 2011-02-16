////////////////////////////////////////////////////////////////////////////////
// $Id$
// Author: jadamwilson2@gmail.com
// Description: BCI2000 Source Module for BrainProducts V-Amp devices.
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
#ifndef VAMP_ADC_H
#define VAMP_ADC_H

#include "GenericADC.h"
#include "GenericVisualization.h"
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include "PrecisionTime.h"
#include "FirstAmp.h"
#include "vAmpChannelInfo.h"
#include "vAmpThread.h"


class vAmpADC : public GenericADC
{
 public:
         vAmpADC();
  virtual      ~vAmpADC();

  virtual void Preflight( const SignalProperties&, SignalProperties& ) const;
  virtual void Initialize( const SignalProperties&, const SignalProperties& );
  virtual void Process( const GenericSignal&, GenericSignal& );
  virtual void Halt();

 private:
  int mNumEEGchannels;
  std::vector<int> mChList;
  int mTimeoutMs;
  int mDevID;
  bool mHighSpeed;
  std::vector<float> mImpedances;
  bool mImpedanceMode;

  float getSampleRate(float);
  int getDecimation() const;

  friend class vAmpThread;
  vAmpThread *mAcquire;
};

#endif // VAMP_ADC_H




