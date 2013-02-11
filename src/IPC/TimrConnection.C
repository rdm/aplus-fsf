///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved.
// See .../src/LICENSE for terms of distribution.
//
//
///////////////////////////////////////////////////////////////////////////////

#include <TimrConnection.H>

#include <math.h>
#include <fcntl.h>

#include <MSTypes/MSMethodCallback.H>

I TimrConnection::ServiceType=4;
A TimrConnection::SetableAttrs=(A)0;
A TimrConnection::NonsetableAttrs=(A)0;
S TimrConnection::OnExpireSymbols[10];
MSBoolean TimrConnection::StaticsInitialized=MSFalse;

void TimrConnection::init(A aTimeout_)
{
  if(!StaticsInitialized) initStatics();
  _timer=0;
  _aExpiry=(A)0;
  _secs=_usecs=0;
  _flags=0;
  _eventSymbol=si("timer");
  _aEventData=(A)0;
  _onExpire=Destroy;
  setExpiry(aTimeout_);
}

void TimrConnection::initStatics(void)
{
  SetableAttrs=gvi(Et,5,MS(si("debug")),MS(si("onExpire")),MS(si("expiry")),
		   MS(si("eventSymbol")),MS(si("eventData")));
  NonsetableAttrs=gvi(Et,1,MS(si("pending")));

  int idx=0;
  OnExpireSymbols[idx++]=si("destroy");
  OnExpireSymbols[idx++]=si("restart");
  OnExpireSymbols[idx++]=si("hold");
  OnExpireSymbols[idx++]=si("interval");

  StaticsInitialized=MSTrue;
}


TimrConnection::TimrConnection(A aTimeout_, A cbfunc_)
  : AipcService(cbfunc_)
{
  ipcWarn(0,"%t TimrConnection::TimrConnection\n");
  init(aTimeout_);
}

TimrConnection::~TimrConnection(void)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::~TimrConnection\n");
  if (_timer!=0){delete _timer;_timer=0;}
  if (_aExpiry!=0){dc(_aExpiry);_aExpiry=0;}
  if (_aEventData!=0){dc(_aEventData);_aEventData=0;}
}

void TimrConnection::goOff(void)
{
  _timer=0;
  int myhandle=handle();
  if(Interval==_onExpire){open();}
  A data=_aEventData?getEventData():getExpiry();
  ACallback(_eventSymbol->n,data);
  dc(data);
  if(this!=AipcService::lookup(myhandle,TimrConnection::ServiceType))R;
  if(Destroy==_onExpire){delete this;R;}
  else if(Restart==_onExpire){open();}
}

MSBoolean TimrConnection::setExpiry(A aeobj_)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::setExpiry\n");
  switch(aeobj_->t)
  {
  case It:
    _secs=(0<aeobj_->n)?aeobj_->p[0]:0;
    _usecs=(1<aeobj_->n)?aeobj_->p[1]:0;
    if(2<aeobj_->n&&aeobj_->p[2])turnAbsoluteOn(); else turnAbsoluteOff();
    break;
  case Ft:
    if(0==aeobj_->n)
    {
      _secs=_usecs=0;
      turnAbsoluteOff();
    }
    else
    {
      F fseconds = *(F *)(aeobj_->p);
      _secs = (int)(floor(fseconds));
      _usecs = (int)(1000000.0*(fseconds-floor(fseconds)));
      turnAbsoluteOff();
    }
    break;
  default:
    R MSFalse;
  }
  if(_aExpiry!=0)dc(_aExpiry);
  _aExpiry=(A)ic(aeobj_);
  if(isAbsolute()&&(Restart==_onExpire||Interval==_onExpire))_onExpire=Hold;
  R MSTrue;
}

MSBoolean TimrConnection::setOnExpire(A aOnExpire)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::setOnExpire\n");
  if(1!=aOnExpire->n||!sym(aOnExpire))R MSFalse;
  S oesym=XS(aOnExpire->p[0]);

  if(OnExpireSymbols[0]==oesym) _onExpire=Destroy;
  else if(OnExpireSymbols[1]==oesym)
  {
    if (isAbsolute()) R MSFalse;else  _onExpire=Restart;
  }
  else if(OnExpireSymbols[2]==oesym) _onExpire=Hold;
  else if(OnExpireSymbols[3]==oesym)
  {
    if (isAbsolute()) R MSFalse;else  _onExpire=Interval;
  }
  else R MSFalse;
  R MSTrue;
}

A TimrConnection::getOnExpire(void)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::getOnExpire\n");
  A z=gs(Et);
  z->p[0]=MS(OnExpireSymbols[_onExpire]);
  R z;
}

MSBoolean TimrConnection::setEventSymbol(A newEventSymbol_)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::setEventSymbol\n");
  if(1!=newEventSymbol_->n||!sym(newEventSymbol_))R MSFalse;
  _eventSymbol=XS(newEventSymbol_->p[0]);
  R MSTrue;
}

A TimrConnection::getEventSymbol(void){A z=gs(Et);*z->p=MS(_eventSymbol);R z;}

MSBoolean TimrConnection::setEventData(A newEventData_)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::setEventData\n");
  if(_aEventData!=0)dc(_aEventData);
  _aEventData=(A)ic(newEventData_);
  R MSTrue;
}


void TimrConnection::close(void)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::close\n");
  if (_timer!=0){delete _timer;_timer=0;}
}

void TimrConnection::open(void)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::open\n");
  if(isPending()) R;
  if(isAbsolute())_timer=new MSAbsoluteTimer(_secs,_usecs,
    new MSMethodCallback<TimrConnection>(this,&TimrConnection::goOff));
  else _timer=new MSRegularTimer(_secs,_usecs,
    new MSMethodCallback<TimrConnection>(this,&TimrConnection::goOff));
}


// M: from AipcAttributes

int TimrConnection::timrSetAttrIndex(C *attr_)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::timrSetAttrIndex\n");
  int idx;
  A attrs=timrSetableAttrs();
  I attrsym=MS(si(attr_));
  for(idx=0;idx<attrs->n;++idx)if(attrsym==attrs->p[idx])break;
  return (idx==attrs->n)?-1:idx;
}

int TimrConnection::timrNonsetAttrIndex(C *attr_)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::timrNonsetAttrIndex\n");
  int idx;
  A attrs=timrNonsetableAttrs();
  I attrsym=MS(si(attr_));
  for(idx=0;idx<attrs->n;++idx)if(attrsym==attrs->p[idx])break;
  return (idx==attrs->n)?-1:idx;
}

// M:Attribute interface

A TimrConnection::setableAttrlist(void){return (A)ic(timrSetableAttrs());}

A TimrConnection::getableAttrlist(void)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::getableAttrlist\n");
  int i,idx=0;
  A sattrs=timrSetableAttrs();
  A nsattrs=timrNonsetableAttrs();
  A z=gv(Et,sattrs->n+nsattrs->n);
  for (i=0;i<sattrs->n;++i)z->p[idx++]=sattrs->p[i];
  for (i=0;i<nsattrs->n;++i)z->p[idx++]=nsattrs->p[i];
  R z;
}

MSBoolean TimrConnection::setAttr(C *attr_,A aval_)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::setAttr\n");
  int idx=timrSetAttrIndex(attr_);
  I ival;
  switch(idx) {
    CSBOOL(0,aval_,turnDebugOn,turnDebugOff);
    CSR(1,R setOnExpire(aval_));
    CSR(2,R setExpiry(aval_));
    CSR(3,R setEventSymbol(aval_));
    CSR(4,R setEventData(aval_));
  default: R MSFalse;
  }
  R MSTrue;
}

A TimrConnection::getAttr(C *attr_)
{
  ipcWarn(wrnlvl(),"%t TimrConnection::getAttr\n");
  int idx=timrSetAttrIndex(attr_);
  if(-1!=idx)
  {
    switch(idx) {
      CSR(0,R gi(isDebug()?1:0));
      CSR(1,R getOnExpire());
      CSR(2,R getExpiry());
      CSR(3,R getEventSymbol());
      CSR(4,R getEventData());
    default: R aplus_nl;
    }
  } else {
    int idx=timrNonsetAttrIndex(attr_);
    if(-1!=idx)
    {
      switch(idx) {
	CSR(0,R gi(isPending()?1:0));
      default: R aplus_nl;
      }
    }
    else R aplus_nl; /* subclasses call parent class here */
  }
}

