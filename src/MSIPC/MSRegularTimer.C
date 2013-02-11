///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSIPC/MSTv.H>
#include <MSIPC/MSRegularTimer.H>

MSRegularTimer::MSRegularTimer(unsigned long interval_,MSCallback *cb_) :
MSTimer(MSTimer::Regular,interval_,cb_) {}
MSRegularTimer::MSRegularTimer(time_t sec_, long usec_,MSCallback *cb_) :
MSTimer(MSTimer::Regular,sec_,usec_,cb_) {}
MSRegularTimer::~MSRegularTimer(void) {}
