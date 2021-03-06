////////////////////////////////////////////////////////////////////////////////
// $Id: SimulBCI.h 3798 2012-02-01 18:07:06Z dsarma $
// Author: devsarma@uw.edu
// Description: The SimulBCI Application's Task filter.  Based on FeedbackTask & DFTask
// Adpated: dsarma
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SIMUL_BCI_TASK_H
#define SIMUL_BCI_TASK_H

#include "FeedbackTask.h"
#include "TrialStatistics.h"
#include "Color.h"
#include "TextField.h"
#include "ApplicationWindow.h"

#include "DFBuildScene.h"


class DynamicFeedbackTask : public FeedbackTask
{
 public:
  DynamicFeedbackTask();
  virtual ~DynamicFeedbackTask();

 private:
  // Events to be handled by FeedbackTask descendants.
  //  Events triggered by the GenericFilter interface
  virtual void OnPreflight( const SignalProperties& Input ) const;
  virtual void OnInitialize( const SignalProperties& Input );
  virtual void OnStartRun();
  virtual void OnStopRun();
  virtual void OnHalt()                                           {}
  //  Events triggered during the course of a trial
  virtual void OnTrialBegin();
  virtual void OnTrialEnd();
  virtual void OnFeedbackBegin();
  virtual void OnFeedbackEnd();
  //  Dispatching of the input signal.
  //  Each call to GenericSignal::Process() is dispatched to one of these
  //  events, depending on the phase in the sequence.
  //  There, each handler function corresponds to a phase.
  //  If a handler sets the "progress" argument to true, the application's
  //  state will switch to the next phase.
  virtual void DoPreRun(       const GenericSignal&, bool& doProgress );
  virtual void DoPreFeedback(  const GenericSignal&, bool& doProgress );
  virtual void DoFeedback(     const GenericSignal&, bool& doProgress );
  virtual void DoPostFeedback( const GenericSignal&, bool& doProgress );
  virtual void DoITI(          const GenericSignal&, bool& doProgress );

 private:
  void MoveCursorTo( float x, float y, float z );
  void DisplayMessage( const std::string& );
  void DisplayScore( const std::string& );
 /* void IntToString( int number );*/

  // Graphic objects
  ApplicationWindow& mrWindow;
  DFBuildScene*     mpFeedbackScene;
  int                mRenderingQuality;
  TextField*         mpMessage;
  TextField*         mpMessage2;

  RGBColor mCursorColorFront,
           mCursorColorBack;
  int      mRunCount,
           mTrialCount,
		   mTaskDiff,//task difficulty
		   mCursorAxis,//cursor control axis
           mCurFeedbackDuration,
           mMaxFeedbackDuration;

  float    mCursorSpeedX,
           mCursorSpeedY,
           mCursorSpeedZ,
		   mScore,//Score
		   mScoreCount;//Score counter

  std::vector<int> mVisualCatchTrials;

  bool  mVisualFeedback,
		mIsVisualCatchTrial;

  TrialStatistics mTrialStatistics;
};

#endif // CURSOR_FEEDBACK_TASK_H

