#ifndef MSAbsoluteTimerHEADER
#define MSAbsoluteTimerHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSIPC/MSTimer.H>

class MSIPCExport MSAbsoluteTimer : public MSTimer
{
public:
  MSAbsoluteTimer(unsigned long msec_,MSCallback *cb_);
  MSAbsoluteTimer(time_t sec_,long usec_,MSCallback *cb_);
  ~MSAbsoluteTimer(void);
};

#endif
