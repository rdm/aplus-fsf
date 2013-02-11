#ifndef __AplusApplicaton_H
#define __AplusApplicaton_H

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved.
// See .../src/LICENSE for terms of distribution.
//
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <MSIPC/MSChannel.H>
#include <MSGUI/MSApplication.H>
#include <MSIPC/MSTimer.H>
#include <a/f.h>

class AplusApplication : public MSApplication
{
public:
  AplusApplication(void);
  AplusApplication(int argc_,char **argv_);
  ~AplusApplication(void);
  
private:
  
protected:
};

class AplusMainLoop : public MSApplicationMainLoop
{
public:
  AplusMainLoop(MSApplication *pApplication_);
  ~AplusMainLoop(void);
  
protected:
  virtual void innerLoopFlush(MSBoolean poll_);
  virtual MSBoolean processTimers(void);
  virtual void selectAndProcess(void);
};
 
#endif
