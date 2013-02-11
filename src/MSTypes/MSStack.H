#ifndef MSStackHEADER
#define MSStackHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSTypes/MSNodeList.H>

class MSTypesExport MSStack : public MSNodeItem
{
public:
  MSStack() {};
  ~MSStack(void) {}

  void push(MSNodeItem*);
  MSNodeItem *pop(void);
};

#endif
