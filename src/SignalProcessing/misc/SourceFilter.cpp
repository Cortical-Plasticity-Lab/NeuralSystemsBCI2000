////////////////////////////////////////////////////////////////////////////////
// $Id$
// File:        SourceFilter.h
// Description: A notch filter for removing power line noise, and a high pass
//              collected into a single filter.
// $Log$
// Revision 1.3  2006/10/26 17:05:00  mellinger
// Rewrote IIR filter as a sequence of complex-valued first-order filters to improve numerical stability.
//
// Revision 1.2  2006/05/04 17:17:01  mellinger
// Changed value for passband ripples.
//
// Revision 1.1  2006/05/04 17:06:43  mellinger
// Initial revision.
//
// (C) 2000-2007, BCI2000 Project
// http://www.bci2000.org
////////////////////////////////////////////////////////////////////////////////
#include "PCHIncludes.h"
#pragma hdrstop

#include "SourceFilter.h"
#include "FilterDesign.h"

using namespace std;

RegisterFilter( SourceFilter, 0.1 );

SourceFilter::SourceFilter()
{
  BEGIN_PARAMETER_DEFINITIONS
    "Source int NotchFilter= 0 0 0 2 "
      "// Power line notch filter: 0: disabled, 1: at 50Hz, 2: at 60Hz (enumeration)",
    "Source int HighPassFilter= 0 0 0 1 "
      "// Source high pass filter: 0: disabled, 1: at 0.1Hz (enumeration)",
  END_PARAMETER_DEFINITIONS
}

void
SourceFilter::DesignFilter( real_type& outGain,
                            complex_vector& outZeros,
                            complex_vector& outPoles ) const
{
  outGain = 1.0;
  outZeros.clear();
  outPoles.clear();

  typedef Ratpoly<FilterDesign::Complex> TransferFunction;
  TransferFunction tf( 1.0 );

  { // Configure notch filter
    enum
    {
      disabled = 0,
      at50Hz = 1,
      at60Hz = 2,
    };
    int notchFilter = Parameter( "NotchFilter" );
    real_type corner1 = 0.0,
              corner2 = 0.0;

    switch( notchFilter )
    {
      case disabled:
        break;

      case at50Hz:
        corner1 = 45.0;
        corner2 = 55.0;
        break;

      case at60Hz:
        corner1 = 55.0;
        corner2 = 65.0;
        break;

      default:
        bcierr << "Unknown value of NotchFilter parameter" << endl;
    }

    if( notchFilter != disabled )
    {
      corner1 /= Parameter( "SamplingRate" );
      corner2 /= Parameter( "SamplingRate" );
      if( corner1 >= 0.5 || corner2 >= 0.5 )
      {
        bciout << "Power line frequency is outside sampling bandwidth. "
               << "No filtering will be performed"
               << endl;
      }
      else
      {
       TransferFunction notch =
          FilterDesign::Chebyshev()
          .Ripple_dB( -0.1 )
          .Order( 3 )
          .Bandstop( corner1, corner2 )
          .TransferFunction();
        tf *= notch;
        outGain /= abs( notch.Evaluate( 1.0 ) ); // LF gain
      }
    }
  }

  { // Configure HP filter
    enum
    {
      disabled = 0,
      at01Hz,
    };
    int highPassFilter = Parameter( "HighPassFilter" );
    real_type corner = 0.0;
    switch( highPassFilter )
    {
      case disabled:
        break;

      case at01Hz:
        corner = 0.1;
        break;

      default:
        bcierr << "Unknown value of HighPassFilter parameter" << endl;
    }
    if( highPassFilter != disabled )
    {
      corner /= Parameter( "SamplingRate" ); 
      if( corner >= 0.5 )
      {
        bciout << "High pass corner frequency is outside sampling bandwidth. "
               << "No filtering will be performed"
               << endl;
      }
      else
      {
        TransferFunction hp =
          FilterDesign::Butterworth()
          .Order( 1 )
          .Highpass( corner )
          .TransferFunction();
        tf *= hp;
        outGain /= abs( hp.Evaluate( -1.0 ) ); // HF gain
      }
    }
  }
  outZeros = tf.Numerator().Roots();
  outPoles = tf.Denominator().Roots();
}

