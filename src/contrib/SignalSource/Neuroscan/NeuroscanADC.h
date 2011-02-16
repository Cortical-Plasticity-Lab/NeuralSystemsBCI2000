/* $BEGIN_BCI2000_LICENSE$
 * 
 * This file is part of BCI2000, a platform for real-time bio-signal research.
 * [ Copyright (C) 2000-2011: BCI2000 team and many external contributors ]
 * 
 * BCI2000 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * BCI2000 is distributed in the hope that it will be useful, but
 *                         WITHOUT ANY WARRANTY
 * - without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * $END_BCI2000_LICENSE$
/*/
#ifndef NeuroscanADCH
#define NeuroscanADCH

#include "SockStream.h"

#include "GenericADC.h"
#include "NeuroscanNetRead.h"

class NeuroscanADC : public GenericADC
{
 public:
               NeuroscanADC();
  virtual      ~NeuroscanADC();

  virtual void Preflight( const SignalProperties&, SignalProperties& ) const;
  virtual void Initialize( const SignalProperties&, const SignalProperties& );
  virtual void Process( const GenericSignal&, GenericSignal& );
  virtual void Halt();

 private:
  bool          ProcessDataMsg(CAcqMessage *pMsg, GenericSignal *);
  void          SendCommand(unsigned short controlcode, unsigned short command);
  sockstream    *server;
  client_tcpsocket *c_tcpsocket;
  int           num_channels, num_markerchannels, blocksize, samplingrate, bitspersample;
  float         LSB;
};

#endif // NeuroscanADCH

