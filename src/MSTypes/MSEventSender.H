#ifndef MSEventSenderHEADER
#define MSEventSenderHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSTypes/MSEventReceiver.H>
#include <MSTypes/MSNullEvent.H>
#include <new.h>

class MSTypesExport MSEventSender
{
public:
  MSEventSender(void) { _pReceiverList=0; }
  MSEventSender(const MSEventSender&) { _pReceiverList=0; }
  virtual ~MSEventSender(void);

  // protection against pointer bit wise copy 
  MSEventSender& operator=(const MSEventSender&) { return *this; }

  MSBoolean addReceiver(MSEventReceiver *);
  MSBoolean removeReceiver(MSEventReceiver *);
  
  void removeAllReceivers(void);
  void blockEvents(void);
  void unblockEvents(MSBoolean sendEvent_=MSTrue);

protected:
  virtual void sendEvent(MSEvent&);
  virtual void addReceiverNotify(MSEventReceiver *);
  virtual void removeReceiverNotify(MSEventReceiver *);

  unsigned numReceivers() const;
  
public:
  class MSTypesExport List
  {
  public:  
    unsigned         _blockCount;
    unsigned         _allocated;
    MSEventReceiver *_array[1];

    List(void) : _blockCount(0),_allocated(0) {}
    void *operator new(size_t,unsigned);
    void operator delete(void *);
  };
  
protected:
  List *_pReceiverList;

  List *receiverList(void) const { return _pReceiverList; }
};

#endif 











