////////////////////////////////////////////////////////////////////////////////
// $Id$
// Authors: schalk@wadsworth.org, juergen.mellinger@uni-tuebingen.de
// Description: A class for graph type visualization windows.
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
#ifndef VIS_DISPLAY_GRAPH_H
#define VIS_DISPLAY_GRAPH_H

#include "VisDisplayBase.h"
#include "DisplayFilter.h"
#include "DecimationFilter.h"
#include "SignalDisplay.h"

class QMenu;
class QAction;
class QMouseEvent;

class VisDisplayGraph : public VisDisplayBase
{
  Q_OBJECT

  enum
  {
    cMaxUserScaling = 4, // The maximum number of scaling steps a user
                        // can take from the default.
    cDisplayOversampling = 10, // The number of data points drawn per pixel, if available.
  };

 public:
  VisDisplayGraph( const std::string& visID );
  virtual ~VisDisplayGraph();

 public slots:
  void HandleSignal( const GenericSignal& );

 protected:
  virtual void SetConfig( VisDisplayBase::ConfigSettings& );
  virtual void Restore();
  virtual void Save() const;

  // User interaction.
 private:
  void BuildContextMenu();

  QMenu*   mpContextMenu,
       *   mpHPMenu,
       *   mpLPMenu,
       *   mpNotchMenu;
  QAction* mpActEnlargeSignal,
         * mpActReduceSignal,
         * mpActFewerSamples,
         * mpActMoreSamples,
         * mpActMoreChannels,
         * mpActFewerChannels,
         * mpActToggleDisplayMode,
         * mpActToggleBaselines,
         * mpActToggleValueUnit,
         * mpActToggleChannelLabels,
         * mpActToggleColor,
         * mpActInvertDisplay,
         * mpActChooseColors;


 private slots:
  // Custom popup menu slot
  void ContextMenu( const QPoint& );
  // Menu action slots
  void EnlargeSignal();
  void ReduceSignal();
  void FewerSamples();
  void MoreSamples();
  void MoreChannels();
  void FewerChannels();
  void ToggleDisplayMode();
  void ToggleBaselines();
  void ToggleValueUnit();
  void ToggleChannelLabels();
  void ToggleColor();
  void InvertDisplay();
  void ChooseColors();
  void SetHPOff();
  void SetHP01();
  void SetHP1();
  void SetHP5();
  void SetLPOff();
  void SetLP30();
  void SetLP40();
  void SetLP70();
  void SetNotchOff();
  void SetNotch50();
  void SetNotch60();

 private:
  bool EnlargeSignal_Enabled() const;
  bool ReduceSignal_Enabled() const;
  bool FewerSamples_Enabled() const;
  bool MoreSamples_Enabled() const;
  bool MoreChannels_Enabled() const;
  bool FewerChannels_Enabled() const;
  bool ToggleBaselines_Enabled() const;
  bool ToggleBaselines_Checked() const;
  bool ToggleValueUnit_Enabled() const;
  bool ToggleValueUnit_Checked() const;
  bool ToggleChannelLabels_Enabled() const;
  bool ToggleChannelLabels_Checked() const;
  bool ToggleColor_Enabled() const;
  bool ToggleColor_Checked() const;
  bool InvertDisplay_Checked() const;
  bool ChooseColors_Enabled() const;
  bool Filter_Enabled() const;
  double FilterUnitToValue( const std::string& inUnit ) const;
  double FilterCaptionToValue( const char* inCaption ) const;
  
  int    NominalDisplaySamples() const;
  void   SetNominalDisplaySamples( int );
  double NominalUnitsPerSample() const;
  void   SetNominalUnitsPerSample( double );

 private:
  int   mNumChannels,
        mSignalElements,
        mUserScaling,
        mUserZoom;
  DisplayFilter    mDisplayFilter;
  DecimationFilter mDecimationFilter;
  SignalDisplay    mDisplay;

 private:
  void SyncDisplay();
 protected:
  // Qt event handlers
  void paintEvent( QPaintEvent* );
  void moveEvent( QMoveEvent* );
  void resizeEvent( QResizeEvent* );
  void keyReleaseEvent( QKeyEvent* );
  void mousePressEvent( QMouseEvent* );
};

#endif // VIS_DISPLAY_GRAPH_H
