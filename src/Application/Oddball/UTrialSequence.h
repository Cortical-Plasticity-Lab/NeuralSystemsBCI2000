//---------------------------------------------------------------------------

#ifndef UTrialSequenceH
#define UTrialSequenceH
//---------------------------------------------------------------------------

#include <vector>
#include "UGenericVisualization.h"
#include "UserDisplay.h"

#define SEQ_ITI                 1
#define SEQ_PTP                 2
#define SEQ_FEEDBACK            3
#define SEQ_OUTCOME             4
#define SEQ_CONGRATULATIONS     5

class TRIALSEQUENCE
{
private: 	// User declarations
        int     ontime, offtime;                   // in units of SampleBlocks
        bool    cur_on;   
        int     cur_sequence;                           // current sequence
        USERDISPLAY     *userdisplay;
        STATEVECTOR     *statevector;
        GenericVisualization    *vis;
        unsigned short oldrunning;
        TARGET  *selectedtarget;
        void    SuspendTrial();
public:		// User declarations
        TRIALSEQUENCE::TRIALSEQUENCE(PARAMLIST *plist, STATELIST *slist);
        TRIALSEQUENCE::~TRIALSEQUENCE();
        int     Initialize( PARAMLIST *, STATEVECTOR *, USERDISPLAY *);
        TARGET  *Process(const std::vector<float>&);
        void    ResetTrialSequence();
};
#endif
